add_compiler_rt_component(dcfi)

if(OS_NAME MATCHES "Linux" OR OS_NAME MATCHES "FreeBSD" OR OS_NAME MATCHES "NetBSD")
  set(DCFI_SOURCES dcfi.cpp)

  include_directories(..)

  set(DCFI_CFLAGS ${SANITIZER_COMMON_CFLAGS})

  foreach(arch ${DCFI_SUPPORTED_ARCH})
    add_compiler_rt_runtime(clang_rt.dcfi
      STATIC
      ARCHS ${arch}
      SOURCES ${DCFI_SOURCES}
      CFLAGS ${DCFI_CFLAGS}
      PARENT_TARGET dcfi)
  endforeach()
endif()
