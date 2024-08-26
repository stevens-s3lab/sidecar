#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <lto | cfi | sidecfi | safestack | sidestack | asan | sideasan> <envsetup | ssl | apr | expat | aprutil | pcre | app | run | all | stop>"
    exit 1
fi

MODE=$1
ACTION=$2

# Determine the root, build, and install directories
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
ROOT_DIR="$(cd -- "${SCRIPT_DIR}/.." &> /dev/null && pwd)"
LLVM_DIR="${ROOT_DIR}/sidecar-llvm"
BUILD_DIR="${ROOT_DIR}/build"
INSTALL_DIR="${ROOT_DIR}/install/llvm-sidecar"
SRC_DIR="${ROOT_DIR}/sidecar-benchmarks/apache"
LLVM_BIN="${INSTALL_DIR}/bin"
CC="${LLVM_BIN}/clang"
CXX="${LLVM_BIN}/clang++"
CLEAN_DIR="${ROOT_DIR}/install/llvm-orig"
CLEAN_BIN="${CLEAN_DIR}/bin"
CLEAN_CC="${CLEAN_BIN}/clang"
CLEAN_CXX="${CLEAN_BIN}/clang++"

# Installation folders
APP_DIR="/usr/local/httpd-${MODE}"
OPENSSL_DIR="/usr/local/openssl-${MODE}"
APR_DIR="/usr/local/apr-${MODE}"
EXPAT_DIR="/usr/local/expat-${MODE}"
PCRE_DIR="/usr/local/pcre-${MODE}"

# Conf
HTTPD_CONF="${APP_DIR}/conf/httpd.conf"
SSL_CONF="${APP_DIR}/conf/extra/httpd-ssl.conf"
MPM_CONF="${APP_DIR}/conf/extra/httpd-mpm.conf"

# Documents
SRC_HTDOCS="${SRC_DIR}/htdocs"
APP_HTDOCS="${APP_DIR}/htdocs"

# Flags
LTO_FLAGS="-fvisibility=default -flto"
CFI_FLAGS="-fsanitize=cfi-vcall,cfi-icall -fsanitize-cfi-cross-dso"
SIDECFI_FLAGS="-fsanitize-cfi-decouple"
SCS_FLAGS="-fsanitize=shadow-call-stack"
SIDESTK_FLAGS="-fsanitize-sidestack"
ASAN_FLAGS="-fsanitize=address -fsanitize-recover=all"
SIDEASAN_FLAGS="-mllvm -asan-decouple"

# Function for setting up environment variables
setup_env() {
    export CC=$CC
    export CXX=$CXX
    
    case $MODE in
        lto)
            export CFLAGS="$LTO_FLAGS"
            export CXXFLAGS="$LTO_FLAGS"
            ;;
        cfi)
            export CFLAGS="$LTO_FLAGS $CFI_FLAGS"
            export CXXFLAGS="$LTO_FLAGS $CFI_FLAGS"
            ;;
        sidecfi)
            export CFLAGS="$LTO_FLAGS $CFI_FLAGS $SIDECFI_FLAGS"
            export CXXFLAGS="$LTO_FLAGS $CFI_FLAGS $SIDECFI_FLAGS"
            ;;
        safestack)
            export CFLAGS="$LTO_FLAGS $SCS_FLAGS"
            export CXXFLAGS="$LTO_FLAGS $SCS_FLAGS"
            ;;
        sidestack)
            export CFLAGS="$LTO_FLAGS $SCS_FLAGS $SIDESTK_FLAGS"
            export CXXFLAGS="$LTO_FLAGS $SCS_FLAGS $SIDESTK_FLAGS"
            ;;
        asan)
            export CFLAGS="$LTO_FLAGS $ASAN_FLAGS"
            export CXXFLAGS="$LTO_FLAGS $ASAN_FLAGS"
	    export CC=$CLEAN_CC
	    export CXX=$CLEAN_CXX
            ;;
        sideasan)
            export CFLAGS="$LTO_FLAGS $ASAN_FLAGS $SIDEASAN_FLAGS"
            export CXXFLAGS="$LTO_FLAGS $ASAN_FLAGS $SIDEASAN_FLAGS"
            ;;
        *)
            echo "Invalid mode: $MODE"
            exit 1
            ;;
    esac
    
    export AR=${LLVM_BIN}/llvm-ar
    export RANLIB=${LLVM_BIN}/llvm-ranlib
    export NM=${LLVM_BIN}/llvm-nm
    export LDFLAGS='-fuse-ld=gold'
}

build_ssl() {
	mkdir -p $BUILD_DIR/apache/${MODE}/openssl-1.1.1
	cd $BUILD_DIR/apache/${MODE}/openssl-1.1.1
	make distclean
	rm *.typemap
	$SRC_DIR/openssl-1.1.1/config --prefix=$OPENSSL_DIR --openssldir=$OPENSSL_DIR shared no-async
	make -j24
	make install -j24
}

build_apr() {
	mkdir -p $BUILD_DIR/apache/${MODE}/apr-1.7.4
	cd $BUILD_DIR/apache/${MODE}/apr-1.7.4
	make distclean
	rm *.typemap
	$SRC_DIR/apr-1.7.4/configure --prefix=$APR_DIR
	make -j24
	make install -j24
}

build_expat() {
	mkdir -p $BUILD_DIR/apache/${MODE}/expat-2.5.0
	cd $BUILD_DIR/apache/${MODE}/expat-2.5.0
	make distclean
	rm *.typemap
	$SRC_DIR/expat-2.5.0/configure --prefix=$EXPAT_DIR
	make -j24
	make install -j24
}

build_aprutil() {
	mkdir -p $BUILD_DIR/apache/${MODE}/apr-util-1.6.3
	cd $BUILD_DIR/apache/${MODE}/apr-util-1.6.3
	make distclean
	rm *.typemap
	$SRC_DIR/apr-util-1.6.3/configure --prefix=$APR_DIR --with-apr=$APR_DIR #--with-expat=$EXPAT_DIR 
	make -j24
	make install -j24
}

build_pcre() {
	mkdir -p $BUILD_DIR/apache/${MODE}/pcre-8.45
	cd $BUILD_DIR/apache/${MODE}/pcre-8.45
	make distclean
	rm *.typemap
	$SRC_DIR/pcre-8.45/configure --prefix=$PCRE_DIR --disable-cpp
	make -j24
	make install -j24
}

apply_httpd_conf_changes() {
	if grep -q "^LoadModule socache_shmcb_module modules/mod_socache_shmcb.so" "$HTTPD_CONF"; then
		echo "Changes to httpd.conf have already been applied."
	else
		sed -i 's/^#LoadModule socache_shmcb_module modules\/mod_socache_shmcb.so/LoadModule socache_shmcb_module modules\/mod_socache_shmcb.so/' "$HTTPD_CONF"
		sed -i 's/^#LoadModule ssl_module modules\/mod_ssl.so/LoadModule ssl_module modules\/mod_ssl.so/' "$HTTPD_CONF"
		sed -i 's/^#Include conf\/extra\/httpd-ssl.conf/Include conf\/extra\/httpd-ssl.conf/' "$HTTPD_CONF"
		sed -i 's/^#Include conf\/extra\/httpd-mpm.conf/Include conf\/extra\/httpd-mpm.conf/' "$HTTPD_CONF"
		sed -i 's/^LoadModule log_config_module modules\/mod_log_config.so/#LoadModule log_config_module modules\/mod_log_config.so/' "$HTTPD_CONF"
		echo "Applied changes to httpd.conf."
	fi
}

apply_ssl_conf_changes() {
	if grep -q "^#ErrorLog" "$SSL_CONF"; then
		echo "Changes to httpd-ssl.conf have already been applied."
	else
		sed -i '/^ErrorLog/s/^/#/' "$SSL_CONF"
		sed -i '/^TransferLog/s/^/#/' "$SSL_CONF"
		sed -i '/^CustomLog/s/^/#/' "$SSL_CONF"
		# TODO: create new keys if needed
		sed -i 's|^SSLCertificateFile.*|SSLCertificateFile "/opt/apache-2.4.58/certs/apache-selfsigned.crt"|' "$SSL_CONF"
		sed -i 's|^SSLCertificateKeyFile.*|SSLCertificateKeyFile "/opt/apache-2.4.58/certs/apache-selfsigned.key"|' "$SSL_CONF"
		echo "Applied changes to httpd-ssl.conf."
	fi
}

apply_mpm_conf_changes() {
	if grep -q "ServerLimit" "$MPM_CONF"; then
		echo "Changes to httpd-mpm.conf have already been applied."
	else
		sed -i '/<IfModule mpm_event_module>/,/<\/IfModule>/d' "$MPM_CONF"
		cat <<EOL >> "$MPM_CONF"
<IfModule mpm_event_module>
    StartServers             1
    MinSpareThreads          1
    MaxSpareThreads          1
    ThreadsPerChild          2
    MaxRequestWorkers        2
    MaxConnectionsPerChild   0
    ServerLimit              1
</IfModule>
EOL
		echo "Applied changes to httpd-mpm.conf."
	fi
}

build_app() {
	export LD_LIBRARY_PATH=${INSTALL_DIR}/lib/:$OPENSSL_DIR/lib#:$EXPAT_DIR/lib:$PCRE_DIR/lib

	mkdir -p $BUILD_DIR/apache/${MODE}/httpd-2.4.58
	cd $BUILD_DIR/apache/${MODE}/httpd-2.4.58
	make distclean
	rm *.typemap
	$SRC_DIR/httpd-2.4.58/configure --prefix=$APP_DIR \
		--with-apr=$APR_DIR/bin/apr-1-config  \
		--with-apr-util=$APR_DIR/bin/apu-1-config \
		--enable-ssl --with-ssl=$OPENSSL_DIR #\
		#--with-pcre=$PCRE_DIR #--disable-brotli
	make -j24
	make install -j24

	apply_httpd_conf_changes
	apply_ssl_conf_changes
	apply_mpm_conf_changes

	# copy files to be served
	cp -r $SRC_HTDOCS/. $APP_HTDOCS/
}

# Function to run the server
run_server() {
    export LD_LIBRARY_PATH=${INSTALL_DIR}/lib/:$OPENSSL_DIR/lib#:$EXPAT_DIR/lib:$PCRE_DIR/lib

    taskset -c 0 $APP_DIR/bin/httpd -k start 
}

stop_server() {
   $APP_DIR/bin/httpd -k stop
}

# Execute the action
case $ACTION in
    envsetup)
        setup_env
        ;;
    ssl)
        setup_env
	build_ssl
	;;
    apr)
        setup_env
        build_apr
        ;;
    expat)
        setup_env
        build_expat
        ;;
    aprutil)
        setup_env
        build_aprutil
        ;;
    pcre)
        setup_env
	build_pcre
	;;
    app)
        setup_env
        build_app
        ;;
    run)
        setup_env
        run_server
        ;;
    stop)
	stop_server
	;;
    all)
        setup_env
	build_ssl
        build_apr
	#build_expat
	build_aprutil
        #build_pcre
        build_app
        #run_server
        ;;
    *)
        echo "Invalid action: $ACTION"
        exit 1
        ;;
esac

