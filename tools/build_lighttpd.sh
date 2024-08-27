#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <lto | cfi | sidecfi | safestack | sidestack | asan | sideasan> <envsetup | ssl | pcre | app | run | all>"
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
CLEAN_DIR="${ROOT_DIR}/install/llvm-orig"
SRC_DIR="${ROOT_DIR}/sidecar-benchmarks/lighttpd"
HTTPD_DIR="${ROOT_DIR}/sidecar-benchmarks/apache"
LLVM_BIN="${INSTALL_DIR}/bin"
CC="${LLVM_BIN}/clang"
CXX="${LLVM_BIN}/clang++"
CLEAN_BIN="${CLEAN_DIR}/bin"
CLEAN_CC="${CLEAN_BIN}/clang"
CLEAN_CXX="${CLEAN_BIN}/clang++"

# Installation folders
APP_DIR="/usr/local/lighttpd-${MODE}"
OPENSSL_DIR="/usr/local/openssl3-${MODE}"
PCRE_DIR="/usr/local/pcre-${MODE}"

# Conf
LIGHTTPD_CONF="${SRC_DIR}/lighttpd.conf"

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
    export AR=${LLVM_BIN}/llvm-ar
    export RANLIB=${LLVM_BIN}/llvm-ranlib
    export NM=${LLVM_BIN}/llvm-nm
    
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
            export CXXFLAGS="$LTO_FLAGS"
	    export CC=$CLEAN_CC
	    export CXX=$CLEAN_CXX
	    export AR=${CLEAN_BIN}/llvm-ar
	    export RANLIB=${CLEAN_BIN}/llvm-ranlib
	    export NM=${CLEAN_BIN}/llvm-nm
            ;;
        sideasan)
            export CFLAGS="$LTO_FLAGS $ASAN_FLAGS $SIDEASAN_FLAGS"
            export CXXFLAGS="$LTO_FLAGS"
            ;;
        *)
            echo "Invalid mode: $MODE"
            exit 1
            ;;
    esac

    export LDFLAGS='-fuse-ld=gold'
}

build_ssl() {
	mkdir -p $BUILD_DIR/lighttpd/${MODE}/openssl-3.0.7
	cd $BUILD_DIR/lighttpd/${MODE}/openssl-3.0.7
	make distclean
	rm *.typemap
	$SRC_DIR/openssl-3.0.7/config --prefix=$OPENSSL_DIR --openssldir=$OPENSSL_DIR shared
	make -j24
	make install -j24
}

build_pcre() {
	mkdir -p $BUILD_DIR/lighttpd/${MODE}/pcre-8.45
	cd $BUILD_DIR/lighttpd/${MODE}/pcre-8.45
	make distclean
	rm *.typemap
	$HTTPD_DIR/pcre-8.45/configure --prefix=$PCRE_DIR --disable-cpp
	make -j24
	make install -j24
}

build_app() {
	export LD_LIBRARY_PATH=$OPENSSL_DIR/lib64
	export LDFLAGS="-fuse-ld=gold -L$OPENSSL_DIR/lib64"

	mkdir -p $BUILD_DIR/lighttpd/${MODE}/lighttpd-1.4.76
	cd $BUILD_DIR/lighttpd/${MODE}/lighttpd-1.4.76
	make distclean
	rm *.typemap
	$SRC_DIR/lighttpd-1.4.76/configure --prefix=$APP_DIR \
		--with-openssl=$OPENSSL_DIR \
		--with-pcre=$PCRE_DIR --with-libev --without-zlib 
	make -j24
	make install -j24
}

# Function to run the server
run_server() {
    export LD_LIBRARY_PATH=$OPENSSL_DIR/lib64
    taskset -c 0 $APP_DIR/sbin/lighttpd -D -f $LIGHTTPD_CONF
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
    all)
        setup_env
	build_ssl
        build_pcre
        build_app
        #run_server
        ;;
    *)
        echo "Invalid action: $ACTION"
        exit 1
        ;;
esac

