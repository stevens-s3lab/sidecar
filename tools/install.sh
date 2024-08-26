#!/bin/bash -e

# Determine the root, build, and install directories
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
ROOT_DIR="$(cd -- "${SCRIPT_DIR}/.." &> /dev/null && pwd)"
LLVM_DIR="${ROOT_DIR}/sidecar-llvm"
BUILD_DIR="${ROOT_DIR}/build"
INSTALL_DIR="${ROOT_DIR}/install"

# Usage information
USAGE="Usage: ${0} [ all | llvm-sidecar | llvm-original ]"

build_sidecar_llvm() {
    local build_dir="${BUILD_DIR}/llvm-sidecar"

    mkdir -p "${build_dir}"
    pushd "${build_dir}"

    cmake -G "Unix Makefiles" "${LLVM_DIR}/llvm" \
        -DLLVM_ENABLE_PROJECTS='clang;compiler-rt' \
        -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_TARGETS_TO_BUILD='X86' \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}/llvm-sidecar" \
        -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
        -DLLVM_USE_LINKER=gold \
        -DLLVM_LINK_LLVM_DYLIB=true \
        -DCMAKE_EXE_LINKER_FLAGS='-Wl,-no-keep-memory -fuse-ld=gold' \
        -DLLVM_BINUTILS_INCDIR=/usr/include \
        -DCOMPILER_RT_HAS_GLINE_TABLES_ONLY_FLAG=OFF \
        -DCOMPILER_RT_HAS_FNO_SANITIZE_SAFE_STACK_FLAG=OFF \
        -DCOMPILER_RT_HAS_WGLOBAL_CONSTRUCTORS_FLAG=OFF \
        -DLLVM_BUILD_LLVM_DYLIB=ON 
    make -j$(nproc)
    make install

    popd
}

build_original_llvm() {
    local build_dir="${BUILD_DIR}/llvm-orig"

    mkdir -p "${build_dir}"
    pushd "${build_dir}"

    cmake -G "Unix Makefiles" "${LLVM_DIR}/llvm" \
        -DLLVM_ENABLE_PROJECTS='clang;compiler-rt' \
        -DCMAKE_BUILD_TYPE=Release \
        -DLLVM_TARGETS_TO_BUILD='X86' \
        -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}/llvm-orig" \
        -DCOMPILER_RT_BUILD_LIBFUZZER=OFF \
        -DLLVM_USE_LINKER=gold \
        -DLLVM_LINK_LLVM_DYLIB=true \
        -DCMAKE_EXE_LINKER_FLAGS='-Wl,-no-keep-memory -fuse-ld=gold' \
        -DLLVM_BINUTILS_INCDIR=/usr/include \
        -DCOMPILER_RT_HAS_GLINE_TABLES_ONLY_FLAG=OFF \
        -DCOMPILER_RT_HAS_FNO_SANITIZE_SAFE_STACK_FLAG=OFF \
        -DCOMPILER_RT_HAS_WGLOBAL_CONSTRUCTORS_FLAG=OFF \
        -DLLVM_BUILD_LLVM_DYLIB=ON \
	-DASAN_DECOUPLE=OFF
    make -j$(nproc)
    make install

    popd
}

main() {
    if [[ $# -eq 0 ]]; then
        echo "${USAGE}" 1>&2
        exit 1
    fi

    while [[ $# -gt 0 ]]; do
        case "${1}" in
	    "all")
				  build_sidecar_llvm
		    		  build_original_llvm
		    							;;
            "llvm-sidecar"	) build_sidecar_llvm                    ;;
            "llvm-original"   	) build_original_llvm                   ;;
            *        		) echo "${USAGE}" 1>&2; exit 1          ;;
        esac
        shift
    done
}

main "$@"

