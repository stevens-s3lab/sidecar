ifeq ($(CC),)
CC=clang
endif

BIN=bin/
ASAN_FLAGS=-fsanitize=address -mno-omit-leaf-frame-pointer \
	   -fno-omit-frame-pointer -fno-optimize-sibling-calls \
	   -gline-tables-only -mllvm -asan-decouple

ifneq ($(SYSROOT),)
ASAN_FLAGS+=--sysroot=$(SYSROOT)
endif

ifneq ($(ISYSROOT),)
ASAN_FLAGS+=--isysroot=$(ISYSROOT)
endif

all: all_binaries

all_binaries:
	mkdir -p ${BIN}
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -mllvm -asan-instrument-dynamic-allocas alloca_big_alignment.cpp -o ${BIN}alloca_big_alignment
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 alloca_constant_size.cpp -o ${BIN}alloca_constant_size -fstack-protector
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -mllvm -asan-instrument-dynamic-allocas alloca_detect_custom_size_.cpp -o ${BIN}alloca_detect_custom_size_
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -mllvm -asan-instrument-dynamic-allocas alloca_instruments_all_paddings.cpp -o ${BIN}alloca_instruments_all_paddings
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -mllvm -asan-instrument-dynamic-allocas alloca_overflow_partial.cpp -o ${BIN}alloca_overflow_partial
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -mllvm -asan-instrument-dynamic-allocas alloca_overflow_right.cpp -o ${BIN}alloca_overflow_right
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -mllvm -asan-instrument-dynamic-allocas alloca_safe_access.cpp -o ${BIN}alloca_safe_access
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -mllvm -asan-instrument-dynamic-allocas alloca_underflow_left.cpp -o ${BIN}alloca_underflow_left
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -coverage -O0 asan_and_llvm_coverage_test.cpp -o ${BIN}asan_and_llvm_coverage_test
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 asan_options-help.cpp -o ${BIN}asan_options-help
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 atexit_stats.cpp -o ${BIN}atexit_stats
	${CC} ${ASAN_FLAGS} atoi_strict.c -o ${BIN}atoi_strict
	${CC} ${ASAN_FLAGS} atoll_strict.c -o ${BIN}atoll_strict
	${CC} ${ASAN_FLAGS} atol_strict.c -o ${BIN}atol_strict
	echo "fun:*brokenFunction*" > ${BIN}blacklist.tmpmp
	echo "global:*badGlobal*" >> ${BIN}blacklist.tmpmp
	echo "src:*blacklist-extra.cpp" >> ${BIN}blacklist.tmpmp
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -fsanitize-blacklist=${BIN}blacklist.tmpmp -O0 blacklist.cpp -o ${BIN}blacklist_o0  Helpers/blacklist-extra.cpp 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -fsanitize-blacklist=${BIN}blacklist.tmpmp -O1 blacklist.cpp -o ${BIN}blacklist_o1  Helpers/blacklist-extra.cpp
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -fsanitize-blacklist=${BIN}blacklist.tmpmp -O2 blacklist.cpp -o ${BIN}blacklist_o2  Helpers/blacklist-extra.cpp
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -fsanitize-blacklist=${BIN}blacklist.tmpmp -O3 blacklist.cpp -o ${BIN}blacklist_o3  Helpers/blacklist-extra.cpp
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -fexceptions -O contiguous_container.cpp -o ${BIN}contiguous_container 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O contiguous_container_crash.cpp -o ${BIN}contiguous_container_crash
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -fsanitize-coverage=func,trace-pc-guard coverage-disabled.cpp -o ${BIN}coverage-disabled
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -DTRACE_RT coverage-trace-pc.cpp -o ${BIN}coverage-trace-pc-rt.o -c
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -fsanitize-coverage=edge,trace-pc,indirect-calls coverage-trace-pc.cpp -o ${BIN}coverage-trace-pc ${BIN}coverage-trace-pc-rt.o
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 debug_double_free.cpp -o ${BIN}debug_double_free 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 debug_locate.cpp -o ${BIN}debug_locate 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O debug_mapping.cpp -o ${BIN}debug_mapping
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 debug_report.cpp -o ${BIN}debug_report 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 deep_tail_call.cpp -o ${BIN}deep_tail_call_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 deep_tail_call.cpp -o ${BIN}deep_tail_call_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 deep_tail_call.cpp -o ${BIN}deep_tail_call_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 deep_tail_call.cpp -o ${BIN}deep_tail_call_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 default_options.cpp -o ${BIN}default_options
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 describe_address.cpp -o ${BIN}describe_address 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 error_report_callback.cpp -o ${BIN}error_report_callback
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -g -std=c++11 -Wno-deprecated-declarations exitcode.cpp -o ${BIN}exitcode
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 force_inline_opt0.cpp -o ${BIN}force_inline_opt0_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 force_inline_opt0.cpp -o ${BIN}force_inline_opt0_o0 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 frexp_interceptor.cpp -o ${BIN}frexp_interceptor 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -o ${BIN}global-address global-address.cpp
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 global-demangle.cpp -o ${BIN}global-demangle 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 global-location.cpp -o ${BIN}global-location
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 global-overflow.cpp -o ${BIN}global-overflow_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 global-overflow.cpp -o ${BIN}global-overflow_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 global-overflow.cpp -o ${BIN}global-overflow_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 global-overflow.cpp -o ${BIN}global-overflow_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 global-underflow.cpp Helpers/underflow.cpp -o ${BIN}global-underflow_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 global-underflow.cpp Helpers/underflow.cpp -o ${BIN}global-underflow_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 global-underflow.cpp Helpers/underflow.cpp -o ${BIN}global-underflow_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 global-underflow.cpp Helpers/underflow.cpp -o ${BIN}global-underflow_o3
	${CC} ${ASAN_FLAGS} -fsanitize-recover=address halt_on_error-1.c -o ${BIN}halt_on_error-1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 handle_noreturn_bug.cpp -o ${BIN}handle_noreturn_bug 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 heap-overflow.cpp -o ${BIN}heap-overflow_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 heap-overflow.cpp -o ${BIN}heap-overflow_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 heap-overflow.cpp -o ${BIN}heap-overflow_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 heap-overflow.cpp -o ${BIN}heap-overflow_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 heap-overflow-large.cpp -o ${BIN}heap-overflow-large
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 heavy_uar_test.cpp -o ${BIN}heavy_uar_test_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 heavy_uar_test.cpp -o ${BIN}heavy_uar_test_o2 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} huge_negative_hea_oob.cpp -o ${BIN}huge_negative_hea_oob 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O huge_negative_hea_oob.cpp -o ${BIN}huge_negative_hea_oob_o 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 initialization-blacklist.cpp Helpers/initialization-blacklist-extra.cpp   Helpers/initialization-blacklist-extra2.cpp    -fsanitize-blacklist=Helpers/initialization-blacklist.txt -o ${BIN}initialization-blacklist_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 initialization-blacklist.cpp Helpers/initialization-blacklist-extra.cpp   Helpers/initialization-blacklist-extra2.cpp    -fsanitize-blacklist=Helpers/initialization-blacklist.txt -o ${BIN}initialization-blacklist_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 initialization-blacklist.cpp Helpers/initialization-blacklist-extra.cpp   Helpers/initialization-blacklist-extra2.cpp    -fsanitize-blacklist=Helpers/initialization-blacklist.txt -o ${BIN}initialization-blacklist_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS}  -O0 initialization-bug.cpp Helpers/initialization-bug-extra2.cpp -o ${BIN}initialization-bug-INIT-ORDER-EXE
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 initialization-constexpr.cpp Helpers/initialization-constexpr-extra.cpp --std=c++11 -o ${BIN}initialization-constexpr_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 initialization-constexpr.cpp Helpers/initialization-constexpr-extra.cpp --std=c++11 -o ${BIN}initialization-constexpr_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 initialization-constexpr.cpp Helpers/initialization-constexpr-extra.cpp --std=c++11 -o ${BIN}initialization-constexpr_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 initialization-constexpr.cpp Helpers/initialization-constexpr-extra.cpp --std=c++11 -o ${BIN}initialization-constexpr_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 initialization-nobug.cpp Helpers/initialization-nobug-extra.cpp -o ${BIN}initialization-nobug_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 initialization-nobug.cpp Helpers/initialization-nobug-extra.cpp -o ${BIN}initialization-nobug_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 initialization-nobug.cpp Helpers/initialization-nobug-extra.cpp -o ${BIN}initialization-nobug_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 initialization-nobug.cpp Helpers/initialization-nobug-extra.cpp -o ${BIN}initialization-nobug_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 init-order-atexit.cpp Helpers/init-order-atexit-extra.cpp -o ${BIN}init-order-atexit
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 inline.cpp -o ${BIN}inline 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 interception_failure_test.cpp -o ${BIN}interception_failure_test_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 interception_failure_test.cpp -o ${BIN}interception_failure_test_o1 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 interception_failure_test.cpp -o ${BIN}interception_failure_test_o2 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 interception_failure_test.cpp -o ${BIN}interception_failure_test_o3 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -fexceptions -O0 intercept-rethrow-exception.cpp -o ${BIN}intercept-rethrow-exception
	${CC} ${ASAN_FLAGS} interface_test.cpp -o ${BIN}interface_test 
	${CC} -x c ${ASAN_FLAGS} interface_test.cpp -o ${BIN}interface_test_c 
	${CC} --sysroot=$(SYSROOT) interface_test.cpp -pie -o ${BIN}interface_test_pie
	${CC} --sysroot=$(SYSROOT) -x c interface_test.cpp -pie -o ${BIN}interface_test_pie_c
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -fsanitize-address-field-padding=1  intra-object-overflow.cpp -o ${BIN}intra-object-overflow
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 invalid-pointer-pairs-compare-errors.cpp -o ${BIN}invalid-pointer-pairs-compare-errors -mllvm -asan-detect-invalid-pointer-pair
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 invalid-pointer-pairs-compare-null.cpp -o ${BIN}invalid-pointer-pairs-compare-null -mllvm -asan-detect-invalid-pointer-pair
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 invalid-pointer-pairs-compare-success.cpp -o ${BIN}invalid-pointer-pairs-compare-success -mllvm -asan-detect-invalid-pointer-pair
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 invalid-pointer-pairs.cpp -o ${BIN}invalid-pointer-pairs -mllvm -asan-detect-invalid-pointer-pair
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 invalid-pointer-pairs-subtract-errors.cpp -o ${BIN}invalid-pointer-pairs-subtract-errors -mllvm -asan-detect-invalid-pointer-pair
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 invalid-pointer-pairs-subtract-success.cpp -o ${BIN}invalid-pointer-pairs-subtract-success -mllvm -asan-detect-invalid-pointer-pair
	${CC} --driver-mode=g++ ${ASAN_FLAGS}  log-path_test.cpp -o ${BIN}log-path_test
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O longjmp.cpp -o ${BIN}longjmp 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 lsan_annotations.cpp -o ${BIN}lsan_annotations_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 lsan_annotations.cpp -o ${BIN}lsan_annotations_o3 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 lsan_crash.cpp -o ${BIN}lsan_crash 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 malloc_context_size.cpp -o ${BIN}malloc_context_size
	${CC} --driver-mode=g++ ${ASAN_FLAGS} malloc_fill.cpp -o ${BIN}malloc_fill
	# not used
	#${CC} ${ASAN_FLAGS} -Dtestfunc=mallinfo malloc-no-intercept.c -o ${BIN}malloc-no-intercept
	#${CC} ${ASAN_FLAGS} -Dtestfunc=mallopt  malloc-no-intercept.c -o ${BIN}malloc-no-intercept
	#${CC} ${ASAN_FLAGS} -Dtestfunc=memalign malloc-no-intercept.c -o ${BIN}malloc-no-intercept
	#${CC} ${ASAN_FLAGS} -Dtestfunc=pvalloc  malloc-no-intercept.c -o ${BIN}malloc-no-intercept
	#${CC} ${ASAN_FLAGS} -Dtestfunc=cfree    malloc-no-intercept.c -o ${BIN}malloc-no-intercept
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 max_redzone.cpp -o ${BIN}max_redzone_o0 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 max_redzone.cpp -o ${BIN}max_redzone_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 memcmp_strict_test.cpp -o ${BIN}memcmp_strict_test 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 memcmp_test.cpp -o ${BIN}memcmp_test_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 memcmp_test.cpp -o ${BIN}memcmp_test_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 memcmp_test.cpp -o ${BIN}memcmp_test_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 memcmp_test.cpp -o ${BIN}memcmp_test_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -DTEST_MEMSET memset_test.cpp -o ${BIN}memset_test_memset_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -DTEST_MEMSET memset_test.cpp -o ${BIN}memset_test_memset_o1 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -DTEST_MEMSET memset_test.cpp -o ${BIN}memset_test_memset_o2 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 -DTEST_MEMSET memset_test.cpp -o ${BIN}memset_test_memset_o3 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -DTEST_MEMCPY memset_test.cpp -o ${BIN}memset_test_memcpy_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -DTEST_MEMCPY memset_test.cpp -o ${BIN}memset_test_memcpy_o1 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -DTEST_MEMCPY memset_test.cpp -o ${BIN}memset_test_memcpy_o2 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 -DTEST_MEMCPY memset_test.cpp -o ${BIN}memset_test_memcpy_o3 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -DTEST_MEMMOVE memset_test.cpp -o ${BIN}memset_test_memmove_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -DTEST_MEMMOVE memset_test.cpp -o ${BIN}memset_test_memmove_o1 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -DTEST_MEMMOVE memset_test.cpp -o ${BIN}memset_test_memmove_o2 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 -DTEST_MEMMOVE memset_test.cpp -o ${BIN}memset_test_memmove_o3 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -DTEST_MEMCPY_SIZE_OVERFLOW memset_test.cpp -o ${BIN}memset_test_size_overflow
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 null_deref.cpp -o ${BIN}null_deref_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 null_deref.cpp -o ${BIN}null_deref_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 null_deref.cpp -o ${BIN}null_deref_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 null_deref.cpp -o ${BIN}null_deref_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 on_error_callback.cpp -o ${BIN}on_error_callback
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 partial_right.cpp -o ${BIN}partial_right_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 partial_right.cpp -o ${BIN}partial_right_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 partial_right.cpp -o ${BIN}partial_right_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 partial_right.cpp -o ${BIN}partial_right_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 pass-object-byval.cpp -o ${BIN}pass-object-byval
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 pass-struct-byval.cpp -o ${BIN}pass-struct-byval
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 poison_partial.cpp -o ${BIN}poison_partial
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -std=c++11 pr33372.cpp -o ${BIN}pr33372_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -std=c++11 pr33372.cpp -o ${BIN}pr33372_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -std=c++11 pr33372.cpp -o ${BIN}pr33372_o2
	${CC} ${ASAN_FLAGS} -O2 printf-1.c -o ${BIN}printf-1
	${CC} ${ASAN_FLAGS} -O2 printf-2.c -o ${BIN}printf-2
	${CC} ${ASAN_FLAGS} -O2 printf-3.c -o ${BIN}printf-3
	${CC} ${ASAN_FLAGS} -O2 printf-4.c -o ${BIN}printf-4
	${CC} ${ASAN_FLAGS} -O2 printf-5.c -o ${BIN}printf-5
	${CC} ${ASAN_FLAGS} -O2 printf-m.c -o ${BIN}printf-m
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 print_summary.cpp -o ${BIN}print_summary
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 realloc.cpp -o ${BIN}realloc
	#${CC} ${ASAN_FLAGS} -O0 set_shadow_test.c -o ${BIN}set_shadow_test
	${CC} ${ASAN_FLAGS} -O2 sleep_after_init.c -o ${BIN}sleep_after_init
	${CC} ${ASAN_FLAGS} -O2 sleep_before_dying.c -o ${BIN}sleep_before_dying
	${CC} --driver-mode=g++ ${ASAN_FLAGS} small_memcpy_test.cpp -o ${BIN}small_memcpy_test
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 speculative_load2.cpp -o ${BIN}speculative_load2_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 speculative_load2.cpp -o ${BIN}speculative_load2_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 speculative_load2.cpp -o ${BIN}speculative_load2_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 speculative_load2.cpp -o ${BIN}speculative_load2_o3
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 speculative_load.cpp -o ${BIN}speculative_load_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 speculative_load.cpp -o ${BIN}speculative_load_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 speculative_load.cpp -o ${BIN}speculative_load_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 speculative_load.cpp -o ${BIN}speculative_load_o3 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 stack-buffer-overflow.cpp -o ${BIN}stack-buffer-overflow_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 stack-buffer-overflow.cpp -o ${BIN}stack-buffer-overflow_o1 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 stack-buffer-overflow.cpp -o ${BIN}stack-buffer-overflow_o2 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 stack-buffer-overflow.cpp -o ${BIN}stack-buffer-overflow_o3 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 stack-buffer-overflow-with-position.cpp -o ${BIN}stack-buffer-overflow-with-position
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 stack-frame-demangle.cpp -o ${BIN}stack-frame-demangle 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 stack-oob-frames.cpp -o ${BIN}stack-oob-frames
	${CC} ${ASAN_FLAGS} strcasestr-1.c -o ${BIN}strcasestr-1 
	${CC} ${ASAN_FLAGS} strcasestr-2.c -o ${BIN}strcasestr-2 
	${CC} ${ASAN_FLAGS} strcasestr_strict.c -o ${BIN}strcasestr_strict 
	${CC} ${ASAN_FLAGS} strcat_strict.c -o ${BIN}strcat_strict
	${CC} ${ASAN_FLAGS} strchr_strict.c -o ${BIN}strchr_strict 
	${CC} ${ASAN_FLAGS} strcmp.c -o ${BIN}strcmp
	${CC} ${ASAN_FLAGS} strcmp_strict.c -o ${BIN}strcmp_strict 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -fno-builtin strcpy-overlap.cpp -o ${BIN}strcpy-overlap_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fno-builtin strcpy-overlap.cpp -o ${BIN}strcpy-overlap_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -fno-builtin strcpy-overlap.cpp -o ${BIN}strcpy-overlap_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 -fno-builtin strcpy-overlap.cpp -o ${BIN}strcpy-overlap_o3
	${CC} ${ASAN_FLAGS} strcspn-1.c -o ${BIN}strcspn-1 
	${CC} ${ASAN_FLAGS} strcspn-2.c -o ${BIN}strcspn-2 
	${CC} ${ASAN_FLAGS} strcspn_strict.c -o ${BIN}strcspn_strict 
	${CC} ${ASAN_FLAGS} -O2 strip_path_prefix.c -o ${BIN}strip_path_prefix
	${CC} ${ASAN_FLAGS} strncasecmp_strict.c -o ${BIN}strncasecmp_strict
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -fno-builtin strncat-overlap.cpp -o ${BIN}strncat-overlap_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fno-builtin strncat-overlap.cpp -o ${BIN}strncat-overlap_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -fno-builtin strncat-overlap.cpp -o ${BIN}strncat-overlap_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 -fno-builtin strncat-overlap.cpp -o ${BIN}strncat-overlap_o3
	${CC} ${ASAN_FLAGS} strncat_strict.c -o ${BIN}strncat_strict
	${CC} ${ASAN_FLAGS} strncmp_strict.c -o ${BIN}strncmp_strict
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -fno-builtin strncpy-overlap.cpp -o ${BIN}strncpy-overlap_o0
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fno-builtin strncpy-overlap.cpp -o ${BIN}strncpy-overlap_o1
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -fno-builtin strncpy-overlap.cpp -o ${BIN}strncpy-overlap_o2
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 -fno-builtin strncpy-overlap.cpp -o ${BIN}strncpy-overlap_o3
	${CC} ${ASAN_FLAGS} strpbrk-1.c -o ${BIN}strpbrk-1 
	${CC} ${ASAN_FLAGS} strpbrk-2.c -o ${BIN}strpbrk-2 
	${CC} ${ASAN_FLAGS} strpbrk_strict.c -o ${BIN}strpbrk_strict 
	${CC} ${ASAN_FLAGS} strspn-1.c -o ${BIN}strspn-1 
	${CC} ${ASAN_FLAGS} strspn-2.c -o ${BIN}strspn-2 
	${CC} ${ASAN_FLAGS} strspn_strict.c -o ${BIN}strspn_strict 
	${CC} ${ASAN_FLAGS} strstr-1.c -o ${BIN}strstr-1 
	${CC} ${ASAN_FLAGS} strstr-2.c -o ${BIN}strstr-2 
	${CC} ${ASAN_FLAGS} strstr_strict.c -o ${BIN}strstr_strict 
	${CC} ${ASAN_FLAGS} strtok.c -o ${BIN}strtok
	${CC} ${ASAN_FLAGS} strtoll_strict.c -o ${BIN}strtoll_strict
	${CC} ${ASAN_FLAGS} -D_CRT_SECURE_NO_WARNINGS -DTEST1 strtol_strict.c -o ${BIN}strtol_strict
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 suppressions-exec-relative-location.cpp -o ${BIN}suppressions-exec-relative-location
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 suppressions-function.cpp -o ${BIN}suppressions-function_o0 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O3 suppressions-function.cpp -o ${BIN}suppressions-function_o3 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 suppressions-interceptor.cpp -o ${BIN}suppressions-interceptor
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -DSHARED_LIB suppressions-library.cpp -fPIC -shared -o ${BIN}libsuppressions-library.cpp.dynamic.so
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 suppressions-library.cpp -o ${BIN}suppressions-library -Wl,-rpath,bin/cross-compilation/ -L${BIN} -lsuppressions-library.cpp.dynamic
	${CC} --driver-mode=g++ ${ASAN_FLAGS} throw_call_test.cpp -o ${BIN}throw_call_test 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O throw_catch.cpp -o ${BIN}throw_catch 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} throw_invoke_test.cpp -o ${BIN}throw_invoke_test 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} throw_invoke_test.cpp -o ${BIN}throw_invoke_test -stdlib=libstdc++ -static-libstdc++ 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 time_interceptor.cpp -o ${BIN}time_interceptor 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 uar_and_exceptions.cpp -o ${BIN}uar_and_exceptions
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 unaligned_loads_and_stores.cpp -o ${BIN}unaligned_loads_and_stores
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 use-after-poison.cpp -o ${BIN}use-after-poison 
	#${CC} --driver-mode=g++ ${ASAN_FLAGS} -std=c++11 -O1 -fsanitize-address-use-after-scope use-after-scope-capture.cpp -o ${BIN}use-after-scope-capture 
	#${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -fsanitize-address-use-after-scope use-after-scope-conversion.cpp -o ${BIN}use-after-scope-conversion
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fsanitize-address-use-after-scope use-after-scope.cpp -o ${BIN}use-after-scope 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 use-after-scope.cpp -o ${BIN}use-after-scope 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fsanitize-address-use-after-scope use-after-scope-dtor-order.cpp -o ${BIN}use-after-scope-dtor-order 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -fsanitize-address-use-after-scope use-after-scope-goto.cpp -o ${BIN}use-after-scope-goto 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fsanitize-address-use-after-scope use-after-scope-if.cpp -o ${BIN}use-after-scope-if 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O2 -fsanitize-address-use-after-scope use-after-scope-inlined.cpp -o ${BIN}use-after-scope-inlined 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fsanitize-address-use-after-scope use-after-scope-loop-bug.cpp -o ${BIN}use-after-scope-loop-bug 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fsanitize-address-use-after-scope use-after-scope-loop.cpp -o ${BIN}use-after-scope-loop 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fsanitize-address-use-after-scope use-after-scope-loop-removed.cpp -o ${BIN}use-after-scope-loop-removed 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O1 -fsanitize-address-use-after-scope use-after-scope-nobug.cpp -o ${BIN}use-after-scope-nobug 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -std=c++11 -O1 -fsanitize-address-use-after-scope use-after-scope-temp2.cpp -o ${BIN}use-after-scope-temp2 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -std=c++11 -O1 -fsanitize-address-use-after-scope use-after-scope-temp.cpp -o ${BIN}use-after-scope-temp 
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -std=c++11 -O0 -fsanitize-address-use-after-scope use-after-scope-types.cpp -o ${BIN}use-after-scope-types
	${CC} --driver-mode=g++ ${ASAN_FLAGS} -O0 -mllvm -asan-instrument-dynamic-allocas vla_chrome_testcase.cpp -o ${BIN}vla_chrome_testcase
	${CC} --driver-mode=g++ ${ASAN_FLAGS} zero_page_pc.cpp -o ${BIN}zero_page_pc 
clean:
	rm -rf ${BIN}
