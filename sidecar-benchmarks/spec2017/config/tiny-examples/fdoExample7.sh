echo build with --feedback
runcpu --fake --config=fdoExample6 --feedback | grep -e Building -e '^/opt'
echo
echo build with --nofeedback
runcpu --fake --config=fdoExample6 --nofeedback | grep -e Building -e '^/opt'
# see "Stupid Assumptions" in tiny-examples/contents.txt
