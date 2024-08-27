runcpu --fake --config=precedence_example5 --label=wall 519.lbm          | grep lbm.c
runcpu --fake --config=precedence_example5 --label=wall -T peak  519.lbm | grep lbm.c
runcpu --fake --config=precedence_example5 --label=wall -T peak  619.lbm | grep lbm.c
# see "Stupid Assumptions" in tiny-examples/contents.txt
