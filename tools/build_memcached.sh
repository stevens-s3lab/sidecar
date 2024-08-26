#!/bin/bash

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <lto | cfi | sidecfi | safestack | sidestack | asan | sideasan> <envsetup | ssl | levent | app | run | all>"
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
SRC_DIR="${ROOT_DIR}/sidecar-benchmarks/memcached"
HTTPD_DIR="${ROOT_DIR}/sidecar-benchmarks/apache"
LLVM_BIN="${INSTALL_DIR}/bin"
CC="${LLVM_BIN}/clang"
CXX="${LLVM_BIN}/clang++"
CLEAN_DIR="${ROOT_DIR}/install/llvm-orig"
CLEAN_BIN="${CLEAN_DIR}/bin"
CLEAN_CC="${CLEAN_BIN}/clang"
CLEAN_CXX="${CLEAN_BIN}/clang++"

# Installation folders
APP_DIR="/usr/local/memcached-${MODE}"
OPENSSL_DIR="/usr/local/openssl-${MODE}"
LIBEVENT_DIR="/usr/local/libevent-${MODE}"

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
            export CXXFLAGS="$LTO_FLAGS $ASAN_FLAGS"
	    export CC=$CLEAN_CC
	    export CXX=$CLEAN_CXX
	    export AR=${CLEAN_BIN}/llvm-ar
	    export RANLIB=${CLEAN_BIN}/llvm-ranlib
	    export NM=${CLEAN_BIN}/llvm-nm
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

    export LDFLAGS='-fuse-ld=gold'
}

build_ssl() {
	mkdir -p $BUILD_DIR/memcached/${MODE}/openssl-1.1.1
	cd $BUILD_DIR/memcached/${MODE}/openssl-1.1.1
	make distclean
	rm *.typemap
	$HTTPD_DIR/openssl-1.1.1/config --prefix=$OPENSSL_DIR --openssldir=$OPENSSL_DIR shared no-async
	make -j24
	make install -j24
}

build_libevent() {
	export LD_LIBRARY_PATH=$OPENSLL_DIR/lib

	mkdir -p $BUILD_DIR/memcached/${MODE}/libevent
	cd $BUILD_DIR/memcached/${MODE}/libevent
	rm -rf *

	cmake $SRC_DIR/libevent -DCMAKE_INSTALL_PREFIX=$LIBEVENT_DIR \
		-DOPENSSL_CRYPTO_LIBRARY=$OPENSSL_DIR/lib/libcrypto.so \
		-DOPENSSL_INCLUDE_DIR=$OPENSSL_DIR/include \
		-DOPENSSL_SSL_LIBRARY=$OPENSSL_DIR/lib/libssl.so
	make -j24
	make install -j24
}

build_app() {
	export LD_LIBRARY_PATH=$OPENSLL_DIR/lib

	mkdir -p $BUILD_DIR/memcached/${MODE}/memcached-1.6.9
	cd $BUILD_DIR/memcached/${MODE}/memcached-1.6.9
	make distclean
	$SRC_DIR/memcached-1.6.9/configure --with-libevent=$LIBEVENT_DIR
	make -j24
}

run_server() {
	export PKG_CONFIG_PATH=$BUILD_DIR/memcached/${MODE}/memcached-1.6.9/lib/pkgconfig:$PKG_CONFIG_PATH

	cd $BUILD_DIR/memcached/${MODE}/memcached-1.6.9
	taskset -c 0 ./memcached -p 11211 -t 1 -u kleftog
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
    levent)
        setup_env
	build_libevent
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
        build_libevent
        build_app
        #run_server
        ;;
    *)
        echo "Invalid action: $ACTION"
        exit 1
        ;;
esac

