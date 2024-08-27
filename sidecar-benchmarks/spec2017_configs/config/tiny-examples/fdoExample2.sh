runcpu --config=fdoExample2 --fake \
   | grep -e readline.f90 -e Train -e '^/opt'
# see "Stupid Assumptions" in tiny-examples/contents.txt
