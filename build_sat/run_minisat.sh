#!/bin/bash
N=0
ALPHA=0
AUTO_VTF="../input_automata/old/armcNFA_inclTest_1.vtf"
if [ $# -eq 1 ]
  then
    AUTO_VTF=$1
elif [ $# -gt 1 ]
  then
    echo "Wrong arguments. Expected single argument of path to a vtf file with automaton for reduction"
    echo "If no arguments are passed, the base path is used set in the variable."
fi
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

echo "Got info about the automaton:"
echo "      Reduction of states to: $N, size of the alphabet: $ALPHA"
echo -n "      Accepted words:"
delim=""
for item in "${EX[@]}"; do
  printf "%s" "$delim$item"
  delim=","
done
echo

echo -n "      Rejected words:"
delim=""
for item in "${CONEX[@]}"; do
  printf "%s" "$delim$item"
  delim=","
done
echo

touch sat.cnf
./sat_red --init -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > sat.cnf

echo "--[AUTO LOG]--              INITIAL GENERATION DONE"
out=$(./MiniSat.14_linux sat.cnf result.txt)

while [ $FOUND -eq 0 ]
do
  if [[ "$out" == *"UNSATISFIABLE"* ]]; then
    N=$((N + 1))
    echo "--[SOLVER LOG]--            COMPUTATION DONE - UNSATISFIABLE, INCREASING NUMBER OF STATES - "$N

    truncate -s 0 sat.cnf
    ./sat_red --init -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > sat.cnf

    echo "--[AUTO LOG]--              INITIAL GENERATION DONE"
    out=$(./MiniSat.14_linux sat.cnf result.txt)

    continue
    fi

    echo "--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE"
    out=$(./sat_red --compare -N $N -S $ALPHA "$AUTO_VTF")

      if [[ "$out" == *"PASS"* ]]; then
        FOUND=1
        echo "--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE EQUAL"

        else
          echo "--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE *NOT* EQUAL"

          IFS=';' read -ra lines <<< "$out"
          IFS=',' read -ra PRO <<< "${lines[0]}"
          IFS=',' read -ra CON <<< "${lines[1]}"

          echo -n "--[AUTO LOG]--              ADDING NEW EXAMPLES, ACCEPTING:"
          delim=""
          for item in "${PRO[@]}"; do
            printf "%s" "$delim$item"
            delim=","
          done
          echo -n "; REJECTING:"
          delim=""
          for item in "${CON[@]}"; do
            printf "%s" "$delim$item"
            delim=","
          done
          echo

          MAX=$(grep -Eo '[0-9]+' "sat.cnf" | sort -rn | head -n 1)

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

          echo "--[AUTO LOG]--              ADDITIONAL GENERATION DONE"
          out=$(./MiniSat.14_linux sat.cnf result.txt)
          fi
      sleep 0.5
  done

