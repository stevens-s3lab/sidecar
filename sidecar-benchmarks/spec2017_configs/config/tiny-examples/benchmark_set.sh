runcpu --fakereportable --config=benchmark_set.correct | grep speed.txt
runcpu --fakereportable --config=benchmark_set.wrong   | grep speed.txt
cd /tmp/benchmark_set/result
grep STACKSIZE *txt
# see "Stupid Assumptions" in tiny-examples/contents.txt
