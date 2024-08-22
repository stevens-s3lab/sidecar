# Install script for directory: /mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/home/kleftog/software/libevent/nolto")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_core.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_core-2.2.so.1.0.0"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_core-2.2.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_core-2.2.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_core-2.2.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_core.so")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/libevent_core.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_extra.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_extra-2.2.so.1.0.0"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_extra-2.2.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_extra-2.2.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so"
         OLD_RPATH "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_extra-2.2.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_extra.so")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/libevent_extra.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_openssl.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_openssl-2.2.so.1.0.0"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_openssl-2.2.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_openssl-2.2.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so"
         OLD_RPATH "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_openssl-2.2.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_openssl.so")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/libevent_openssl.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_pthreads.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_pthreads-2.2.so.1.0.0"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_pthreads-2.2.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHANGE
           FILE "${file}"
           OLD_RPATH "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib:"
           NEW_RPATH "")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_pthreads-2.2.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so")
    file(RPATH_CHANGE
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so"
         OLD_RPATH "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib:"
         NEW_RPATH "")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent_pthreads-2.2.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent_pthreads.so")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/libevent_pthreads.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent-2.2.so.1.0.0"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent-2.2.so.1"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so.1.0.0"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so.1"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so")
    file(RPATH_CHECK
         FILE "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so"
         RPATH "")
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent-2.2.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/usr/bin/strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libevent-2.2.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xlibx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/lib/libevent.so")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/libevent.pc")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/evdns.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/evrpc.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/evhttp.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/evutil.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/event2" TYPE FILE FILES
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/buffer.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/bufferevent.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/bufferevent_compat.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/bufferevent_struct.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/buffer_compat.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/dns.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/dns_compat.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/dns_struct.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/event.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/event_compat.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/event_struct.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/watch.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/http.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/http_compat.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/http_struct.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/keyvalq_struct.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/listener.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/rpc.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/rpc_compat.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/rpc_struct.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/tag.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/tag_compat.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/thread.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/util.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/ws.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/visibility.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/include/event2/event-config.h"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/include/event2/bufferevent_ssl.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent" TYPE FILE FILES
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/LibeventConfig.cmake"
    "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/LibeventConfigVersion.cmake"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent/LibeventTargets-static.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent/LibeventTargets-static.cmake"
         "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles/Export/lib/cmake/libevent/LibeventTargets-static.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent/LibeventTargets-static-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent/LibeventTargets-static.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles/Export/lib/cmake/libevent/LibeventTargets-static.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles/Export/lib/cmake/libevent/LibeventTargets-static-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xdevx" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent/LibeventTargets-shared.cmake")
    file(DIFFERENT EXPORT_FILE_CHANGED FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent/LibeventTargets-shared.cmake"
         "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles/Export/lib/cmake/libevent/LibeventTargets-shared.cmake")
    if(EXPORT_FILE_CHANGED)
      file(GLOB OLD_CONFIG_FILES "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent/LibeventTargets-shared-*.cmake")
      if(OLD_CONFIG_FILES)
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent/LibeventTargets-shared.cmake\" will be replaced.  Removing files [${OLD_CONFIG_FILES}].")
        file(REMOVE ${OLD_CONFIG_FILES})
      endif()
    endif()
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles/Export/lib/cmake/libevent/LibeventTargets-shared.cmake")
  if("${CMAKE_INSTALL_CONFIG_NAME}" MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/libevent" TYPE FILE FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/CMakeFiles/Export/lib/cmake/libevent/LibeventTargets-shared-release.cmake")
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xruntimex" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/event_rpcgen.py")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/mnt/ubuntu/home/kleftog/benchmarks/memcached-1.6.9/libevent/nolto_build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
