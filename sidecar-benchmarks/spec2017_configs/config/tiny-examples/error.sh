configpp --config=error > /tmp/out
echo runcpu exit code: $?
grep -C2 ERROR /tmp/out
# see "Stupid Assumptions" in tiny-examples/contents.txt
