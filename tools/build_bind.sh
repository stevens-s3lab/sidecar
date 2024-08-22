#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <lto | cfi | sidecfi | sidexcfi> <envsetup | ssl | libuv | app | run | all>"
    exit 1
fi

MODE=$1
ACTION=$2

# Determine the root, build, and install directories
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
ROOT_DIR="$(cd -- "${SCRIPT_DIR}/.." &> /dev/null && pwd)"
LLVM_DIR="${ROOT_DIR}/sidecar-llvm"
BUILD_DIR="${ROOT_DIR}/build"
INSTALL_DIR="${ROOT_DIR}/install"
SRC_DIR="${ROOT_DIR}/sidecar-benchmarks/bind"
HTTPD_DIR="${ROOT_DIR}/sidecar-benchmarks/apache"
LLVM_BIN="${INSTALL_DIR}/bin"
CC="${LLVM_BIN}/clang"
CXX="${LLVM_BIN}/clang++"

# Installation folders
APP_DIR="/usr/local/bind-${MODE}"
OPENSSL_DIR="/usr/local/openssl-${MODE}"
LIBUV_DIR="/usr/local/libuv-${MODE}"

# Conf
BIND_CONF="${SRC_DIR}/bind9/named.conf"

# Flags
LTO_FLAGS="-fvisibility=default -flto"
CFI_FLAGS="-fsanitize=cfi-vcall,cfi-icall -fsanitize-cfi-cross-dso"
SIDE_FLAGS="-fsanitize-cfi-decouple"
SIDEX_FLAGS="-fsanitize-cfi-slowpath-decouple"

# Function for setting up environment variables
setup_env() {
    export CC=$CC
    export CXX=$CCX
    
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
            export CFLAGS="$LTO_FLAGS $CFI_FLAGS $SIDE_FLAGS"
            export CXXFLAGS="$LTO_FLAGS $CFI_FLAGS $SIDE_FLAGS"
            ;;
        sidexcfi)
            export CFLAGS="$LTO_FLAGS $CFI_FLAGS $SIDEX_FLAGS"
            export CXXFLAGS="$LTO_FLAGS $CFI_FLAGS $SIDEX_FLAGS"
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
	mkdir -p "$BUILD_DIR/bind/${MODE}/openssl-1.1.1"
	cd "$BUILD_DIR/bind/${MODE}/openssl-1.1.1"
	make distclean
	rm *.typemap
	$HTTPD_DIR/openssl-1.1.1/config --prefix=$OPENSSL_DIR --openssldir=$OPENSSL_DIR shared no-async
	make -j24
	make install -j24
}

build_libuv() {
	mkdir -p "$BUILD_DIR/bind/${MODE}/libuv"
	cd "$BUILD_DIR/bind/${MODE}/libuv"
	make distclean
	#./autogen.sh
	$SRC_DIR/libuv/configure --prefix=$LIBUV_DIR
	make -j24
	make install -j24
}

build_app() {
	export PKG_CONFIG_PATH=$LIBUV_DIR/lib:$OPENSSL_DIR/lib:$PKG_CONFIG_PATH
	export LDFLAGS="-fuse-ld=gold -L${OPENSSL_DIR}/lib/ -L${LIBUV_DIR}/lib/"

	mkdir -p "$BUILD_DIR/bind/${MODE}/bind9"
	cd "$BUILD_DIR/bind9/${MODE}/bind9"
	make distclean
	#autoreconf -fi
	$SRC_DIR/bind9/configure --prefix=$APP_DIR --with-openssl=$OPENSSL_DIR/lib --disable-doh
	make -j24
	make install -j24

	mkdir -p "/var/cache/bind"
}

run_server() {
	export LD_LIBRARY_PATH="$OPENSSL_DIR/lib"

	taskset -c 0 $APP_DIR/sbin/named -c $BIND_CONF -n 1 -g
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
    libuv)
        setup_env
	build_libuv
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
	#build_ssl
        build_libuv
        build_app
        #run_server
        ;;
    *)
        echo "Invalid action: $ACTION"
        exit 1
        ;;
esac

