#!/bin/bash
N=0
ALPHA=0
#todo brat auto vtf z argumentu skriptu
AUTO_VTF="../input_automata/old/armcNFA_inclTest_0.vtf"
EX=()
CONEX=()
FOUND=0
MAX=1
info=$(./sat_red --get_info "$AUTO_VTF")

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

touch sat.cnf
./sat_red --init -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > sat.cnf
MAX=$(grep -Eo '[0-9]+' "sat.cnf" | sort -rn | head -n 1)
IFS=' ' read -ra lines <<< "$(wc -l sat.cnf)"
sed -i "1s/^/p cnf $MAX ${lines[0]} \n/" sat.cnf

echo "DONE"
./kissat -q sat.cnf > result.txt
out=$(head -n 1 result.txt)

while [ $FOUND -eq 0 ]
do
  if [[ "$out" == *"UNSATISFIABLE"* ]]; then
    N=$((N + 1))
    echo "UNSATISFIABLE, increasing number of states: "$N

    truncate -s 0 sat.cnf
    ./sat_red --init -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > sat.cnf
    MAX=$(grep -Eo '[0-9]+' "sat.cnf" | sort -rn | head -n 1)
    IFS=' ' read -ra lines <<< "$(wc -l sat.cnf)"
    sed -i "1s/^/p cnf $MAX ${lines[0]} \n/" sat.cnf

    echo "DONE"
    ./kissat -q sat.cnf > result.txt
    out=$(head -n 1 result.txt)

    continue
    fi

    echo "SATISFIABLE"
    out=$(./sat_red --compare -N $N -S $ALPHA "$AUTO_VTF")

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

          if [ ! -z "${PRO[*]}" ] && [ ! -z "${CON[*]}" ]; then
            EX=("${EX[@]}" "${PRO[@]}")
            CONEX=("${CONEX[@]}" "${CON[@]}")
            ./sat_red --add -N $N -S $ALPHA -M $MAX -A "${PRO[@]}" -R "${CON[@]}" >> sat.cnf
          elif [ ! -z "${PRO[*]}" ]; then
            EX=("${EX[@]}" "${PRO[@]}")
            ./sat_red --add -N $N -S $ALPHA -M $MAX -A "${PRO[@]}" >> sat.cnf
          else [ ! -z "${CON[*]}" ]
            CONEX=("${CONEX[@]}" "${CON[@]}")
            ./sat_red --add -N $N -S $ALPHA -M $MAX -R "${CON[@]}" >> sat.cnf
          fi

          sed -i "1d" sat.cnf
          MAX=$(grep -Eo '[0-9]+' "sat.cnf" | sort -rn | head -n 1)
          IFS=' ' read -ra lines <<< "$(wc -l sat.cnf)"
          sed -i "1s/^/p cnf $MAX ${lines[0]} \n/" sat.cnf

          echo "DONE"
          ./kissat -q sat.cnf > result.txt
          out=$(head -n 1 result.txt)

          fi
      sleep 0.5
  done

