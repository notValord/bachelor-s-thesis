#!/bin/bash
N=0
ALPHA=0
#todo brat auto vtf z argumentu skriptu
AUTO_VTF="../input_automata/old/armcNFA_inclTest_1.vtf"
EX=()
CONEX=()
FOUND=0
info=$(./qbf_red --get_info "$AUTO_VTF")

IFS=';' read -ra lines <<< "$info"
N=${lines[0]}
ALPHA=${lines[1]}

IFS=',' read -ra EX <<< "${lines[2]}"
IFS=',' read -ra CONEX <<< "${lines[3]}"

echo "states: $N, alpha: $ALPHA"

delim=""
for item in "${EX[@]}"; do
  printf "%s" "$delim$item"
  delim=","
done
echo

delim=""
for item in "${CONEX[@]}"; do
  printf "%s" "$delim$item"
  delim=","
done
echo

touch qbf.qdimacs
./qbf_red --clauses -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > qbf.qdimacs

echo "DONE"
./caqe --qdo qbf.qdimacs > result.txt
out=$(tail -1 result.txt)

while [ $FOUND -eq 0 ]
do
  if [[ "$out" == *"Unsatisfiable"* ]]; then
    N=$((N + 1))
    echo "UNSATISFIABLE, increasing number of states: "$N

    ./qbf_red --clauses -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > qbf.qdimacs
    echo "DONE"
    ./caqe --qdo qbf.qdimacs > result.txt
    echo "CAQE DONE"
    out=$(tail -1 result.txt)

    continue
    fi

    echo "SATISFIABLE"
    out=$(./qbf_red --compare -N $N -S $ALPHA "$AUTO_VTF")

      if [[ "$out" == *"PASS"* ]]; then
        FOUND=1
        echo "EQUAL"

        else
          echo "NOT EQUAL"
          echo "$out"

          IFS=';' read -ra lines <<< "$out"
          IFS=',' read -ra PRO <<< "${lines[0]}"
          IFS=',' read -ra CON <<< "${lines[1]}"

          echo "Adding new words, CON: ${CON[*]}; PRO: ${PRO[*]}"

          if [ ! -z "${PRO[*]}" ]; then
            EX=("${EX[@]}" "${PRO[@]}")
          fi

          if [ ! -z "${CON[*]}" ]; then
            CONEX=("${CONEX[@]}" "${CON[@]}")
          fi

          ./qbf_red --clauses -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > qbf.qdimacs

          echo "DONE"
          ./caqe --qdo qbf.qdimacs > result.txt
          echo "CAQE DONE"
          out=$(tail -1 result.txt)

          fi
      sleep 0.5
  done

