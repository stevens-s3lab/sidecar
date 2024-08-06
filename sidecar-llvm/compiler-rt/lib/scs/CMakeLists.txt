add_compiler_rt_component(scs)

set(SCS_SOURCES scs.cpp)

include_directories(..)

set(SCS_CFLAGS ${SANITIZER_COMMON_CFLAGS})

foreach(arch ${SCS_SUPPORTED_ARCH})
  add_compiler_rt_runtime(clang_rt.scs
    STATIC
    ARCHS ${arch}
    SOURCES ${SCS_SOURCES}
    CFLAGS ${SCS_CFLAGS}
    OBJECT_LIBS 
                RTSanitizerCommon
                RTSanitizerCommonLibc
    PARENT_TARGET scs)
endforeach()
