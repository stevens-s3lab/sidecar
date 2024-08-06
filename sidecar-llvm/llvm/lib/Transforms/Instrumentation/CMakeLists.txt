add_llvm_component_library(LLVMInstrumentation
  AddressSanitizer.cpp
  BoundsChecking.cpp
  CGProfile.cpp
  ControlHeightReduction.cpp
  DataFlowSanitizer.cpp
  GCOVProfiling.cpp
  MemProfiler.cpp
  MemorySanitizer.cpp
  IndirectCallPromotion.cpp
  Instrumentation.cpp
  InstrOrderFile.cpp
  InstrProfiling.cpp
  PGOInstrumentation.cpp
  PGOMemOPSizeOpt.cpp
  PoisonChecking.cpp
  SanitizerCoverage.cpp
  ValueProfileCollector.cpp
  ThreadSanitizer.cpp
  HWAddressSanitizer.cpp
  SideStack.cpp #SideStack support

  ADDITIONAL_HEADER_DIRS
  ${LLVM_MAIN_INCLUDE_DIR}/llvm/Transforms

  DEPENDS
  intrinsics_gen

  LINK_COMPONENTS
  Analysis
  Core
  MC
  Support
  TransformUtils
  ProfileData
  )


# S3LAB
option(ASAN_PERF_ANALYZE "Enable statistics collection to analyze ASan performance." OFF)
option(ASAN_DECOUPLE "Decouple ASan and run checks in a separate process." ON)
option(USE_PTWRITE "Use PTWRITE instruction for sending messages (only for x86_64)." ON)
option(NOMESSAGES "Do not send any messages when decoupling" OFF)
if (ASAN_PERF_ANALYZE)
  add_definitions(-DASAN_PERF_ANALYZE)
endif()
if (ASAN_DECOUPLE)
  add_definitions(-DASAN_DECOUPLE)
  if (USE_PTWRITE)
    add_definitions(-DUSE_PTWRITE)
  endif()
endif()
if (NOMESSAGES)
  add_definitions(-DNOMESSAGES)
endif()
