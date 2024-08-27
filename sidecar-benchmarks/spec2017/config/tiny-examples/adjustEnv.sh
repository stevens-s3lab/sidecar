export LD_LIBRARY_PATH=my:old:path
runcpu  -c adjustEnv                     | grep -e 'Setting LD_LIBRARY_PATH' -e 'gcc version'
runcpu  -c adjustEnv --define wantGccV8  | grep -e 'Setting LD_LIBRARY_PATH' -e 'gcc version'
# see "Stupid Assumptions" in tiny-examples/contents.txt
