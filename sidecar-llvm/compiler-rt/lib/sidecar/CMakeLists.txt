add_compiler_rt_component(sidecar)

set(SIDECAR_SOURCES sidecar_dev.cpp)

set(SIDECAR_HEADERS sidecar_dev.h)

include_directories(..)

set(SIDECAR_CFLAGS ${SANITIZER_COMMON_CFLAGS})

foreach(arch ${SIDECAR_SUPPORTED_ARCH})
	add_compiler_rt_runtime(clang_rt.sidecar
		STATIC
		ARCHS ${arch}
		SOURCES ${SIDECAR_SOURCES}
		CFLAGS ${SIDECAR_CFLAGS}
    OBJECT_LIBS RTInterception
                RTSanitizerCommon
                RTSanitizerCommonLibc
		PARENT_TARGET sidecar)
endforeach()
