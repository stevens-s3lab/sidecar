runcpu --config=notes_submit 557.xz_r --tune=base | grep txt
runcpu --config=notes_submit 657.xz_s --tune=base | grep txt
runcpu --config=notes_submit 657.xz_s --tune=peak | grep txt

echo
grep -C1 "to prefer" /tmp/notes_submit/result/CPU2017.00*txt

cd /tmp/notes_submit/benchspec/CPU/557.xz_r/run/
  num_dirs=$(ls -d run* | wc -l)
 num_dobmk=$(ls run*/dobmk | wc -l)
echo
echo 557.xz_r has $num_dirs run dirs and $num_dobmk dobmk scripts

cd /tmp/notes_submit/benchspec/CPU/657.xz_s/run/
      num_dirs_base=$(ls -d run*base* | wc -l)
      num_dirs_peak=$(ls -d run*peak* | wc -l)
 num_dobmk_base=$(ls run*base*/dobmk | wc -l)
 num_dobmk_peak=$(ls run*peak*/dobmk 2>/dev/null | wc -l)
echo
echo "657.xz_s base has $num_dirs_base run dir(s) and $num_dobmk_base dobmk script(s)"
echo "657.xz_s peak has $num_dirs_peak run dir(s) and $num_dobmk_peak dobmk script(s)"
# see "Stupid Assumptions" in tiny-examples/contents.txt
