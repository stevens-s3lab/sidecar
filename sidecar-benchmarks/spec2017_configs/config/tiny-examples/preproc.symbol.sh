JUST_DEFINE="configpp --config=preproc.symbol.just_define.cfg"
 TEST_FIRST="configpp --config=preproc.symbol.test_first.cfg"

GREP_IT='grep -e WARNING -e makeflags'

echo original:
$JUST_DEFINE --define build_ncpus=8 | $GREP_IT
$JUST_DEFINE                           | $GREP_IT
echo
echo test1st: 
$TEST_FIRST  --define build_ncpus=8 | $GREP_IT
$TEST_FIRST                            | $GREP_IT
# see "Stupid Assumptions" in tiny-examples/contents.txt
