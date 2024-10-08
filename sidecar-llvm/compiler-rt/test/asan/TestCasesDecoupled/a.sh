#!/bin/bash

# Loop through all files in the current directory
for file in *; do
	# Check if it's a file
	if [ -f "$file" ]; then
		sed -i 's/%run %t \([^ ]*\)/%run taskset -c 0 %t \1 \& \/home\/kleftog\/sidecar-ae\/sidecar\/sidecar-monitors\/sideasan\/x86-64\/monitor/g' "$file"
		sed -i 's/%run %t/%run taskset -c 0 %t \& \/home\/kleftog\/sidecar-ae\/sidecar\/sidecar-monitors\/sideasan\/x86-64\/monitor/g' "$file"
				        
		# Print the name of the file processed
		echo "Processed $file"
	fi
done
