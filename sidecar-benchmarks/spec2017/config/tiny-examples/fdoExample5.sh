runcpu --fake --config=fdoExample5 \
  | grep -e '^mkdir'               \
         -e FullBoard.cpp          \
         -e Train                  \
         -e this.prof              \
         -e ^mv                    \
         -e '^postopt'             \
  | cat -n
# see "Stupid Assumptions" in tiny-examples/contents.txt
