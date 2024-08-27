runcpu --config=continued --label=backslash | grep copies
runcpu --config=continued --label=fieldN    | grep copies
runcpu --config=continued --label=heredoc   | grep copies
#
cd /tmp/submit/benchspec/CPU/505.mcf_r/run
ls *backslash*/dobmk
ls *fieldN*/dobmk
ls *heredoc*/dobmk
#
for file in *0007/dobmk ; do
   echo ==== $file =====
   cat $file
done
# see "Stupid Assumptions" in tiny-examples/contents.txt
