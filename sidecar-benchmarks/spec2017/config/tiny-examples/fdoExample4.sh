runcpu --config=fdoExample4 --fake \
   | grep -e readline.f90          \
          -e Train                 \
          -e '^rm'                 \
          -e '^specmake.*clean'    \
          -e '^specmake.*build'    \
   | cat -n
# see "Stupid Assumptions" in tiny-examples/contents.txt
