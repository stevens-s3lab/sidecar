#!/bin/bash
GREPIT="grep -e '^Setting' -e '^Starting' -e 'Running.* 6' -e STACKSIZE= -e Notice: -e Disabling" 
function cmd_do() {
   echo "$cmd"
   echo "Press enter to do it, or press ^C to kill"
   read somevar
   eval "$cmd"
}

echo
echo Using fake output to see what config files would do
echo
echo stack_ok1: put preENV in the header section
cmd="runcpu --fake --config=stack_ok1 | $GREPIT | uniq"
cmd_do

echo
echo stack_ok2: put preENV in an 'fpspeed:' section
cmd="runcpu --fake --config=stack_ok2 | $GREPIT | uniq"
cmd_do

echo
echo stack_notok1: preENV does nothing if it is in the wrong section
cmd="runcpu --fake --config=stack_notok1 | $GREPIT | uniq"
cmd_do

echo 
echo "              -----------------------------------"
echo "stack_notok2: Warning, env_vars may surprise you."  
echo "              It may seem like it works:"
echo "              -----------------------------------"
cmd="runcpu --fake --config=stack_notok2 | $GREPIT | uniq"
cmd_do

echo
echo ...not ok2 continued: but in a reportable run, it will be disabled for base!
echo "                     (The following command may take a few minutes)"
cmd="runcpu --fake --config=stack_notok2 --reportable fpspeed | $GREPIT | uniq | less"
cmd_do
