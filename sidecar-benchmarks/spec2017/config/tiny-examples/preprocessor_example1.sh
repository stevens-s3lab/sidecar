configpp -c preprocessor_example1.cfg --define chip=sparc | grep -e perlbench -e PORT
configpp -c preprocessor_example1.cfg --define chip=x86 | grep -e perlbench -e PORT
configpp -c preprocessor_example1.cfg -S chip=x86 -S bits=32 | grep -e perlbench -e PORT
# see "Stupid Assumptions" in tiny-examples/contents.txt
