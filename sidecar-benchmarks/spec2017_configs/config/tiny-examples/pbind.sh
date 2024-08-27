runcpu --config=pbind > /dev/null &  # put runcpu in background
sleep 15                             # let things get started
pbind -q > /tmp/pbind/bound          # query bindings
# see "Stupid Assumptions" in tiny-examples/contents.txt
