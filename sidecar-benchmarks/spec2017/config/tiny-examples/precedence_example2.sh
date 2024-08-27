runcpu --config=precedence_example2 --tune=base | grep txt
runcpu --config=precedence_example2 --tune=peak | grep txt
cd /tmp/ptest2/result
grep 'O[0-9]' *txt
# see "Stupid Assumptions" in tiny-examples/contents.txt
