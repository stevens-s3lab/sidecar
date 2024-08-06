add_compiler_rt_component(sidestack)

set(SIDESTACK_SOURCES sidestack.cpp)

include_directories(..)

set(SIDESTACK_CFLAGS ${SANITIZER_COMMON_CFLAGS})

foreach(arch ${SIDESTACK_SUPPORTED_ARCH})
  add_compiler_rt_runtime(clang_rt.sidestack
    STATIC
    ARCHS ${arch}
    SOURCES ${SIDESTACK_SOURCES}
    CFLAGS ${SIDESTACK_CFLAGS}
    PARENT_TARGET sidestack)
endforeach()
