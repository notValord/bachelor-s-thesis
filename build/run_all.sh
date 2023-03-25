#!/bin/bash
rm -f output.txt
touch output.txt
echo "Name; Original states; Det_min states; Time; Equal; Time; Sim states; Time; Equal; Time"
for f in ../input_automata/3/*; do
    #echo "$f"
	#file=$(basename ${f})
    #echo "$file"
    ./reduction -t min_det "$f"
    # remember to quote it or spaces may misbehave
done