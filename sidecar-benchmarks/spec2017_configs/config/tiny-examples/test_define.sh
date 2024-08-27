configpp --config=test_define -S a=1 -S b=1 -S c=1 | grep OPT 
echo
configpp --config=test_define -S a=0 -S b=0 -S c=0 | grep OPT
# see "Stupid Assumptions" in tiny-examples/contents.txt
