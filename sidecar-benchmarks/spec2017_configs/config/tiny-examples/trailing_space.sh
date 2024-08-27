runcpu -c space --fake 527.cam4 | grep without/ | head -1 | cut -b 1-80
runcpu -c space --fake 527.cam4 | grep trailing | head -1 | cut -b 1-80
# see "Stupid Assumptions" in tiny-examples/contents.txt
