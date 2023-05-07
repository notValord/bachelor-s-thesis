#!/bin/bash
echo "Name; Original states; Hopcr states; Time; Equal; Time; Brz states; Time; Equal; Time; Sim states; Time; Equal; Time; Rezid states; Time; Equal; Time"

for f in "../input_automata/1/"*; do
    >&2 echo "$f"
    ./fsa_red -t all -f "$f" --debug
done
>&2 echo "-------------DONE---------------"
