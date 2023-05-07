#!/bin/bash
N=0
ALPHA=0
AUTO_VTF="../input_automata/test_auto/armcNFA_inclTest_6.vtf"
if [ $# -eq 1 ]
  then
    AUTO_VTF=$1
elif [ $# -gt 1 ]
  then
    echo "Wrong arguments. Expected single argument of path to a vtf file with automaton for reduction"
    echo "If no arguments are passed, the base path is used set in the variable."
    exit
fi
EX=()
CONEX=()
FOUND=0
MAX=1
info=$(../build/fsa_red --get_info "$AUTO_VTF")

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

if [ -z "${EX[*]}" ] && [ -z "${CONEX[*]}" ]; then
  echo "Couldn't open the given file"
  exit
fi

touch nfa_sat.cnf

../build/fsa_red --sat_nfa_init -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > nfa_sat.cnf

echo "--[AUTO LOG]--              INITIAL GENERATION DONE"
out=$( ./MiniSat.14_linux nfa_sat.cnf nfa_result.txt )

while [ $FOUND -eq 0 ]
do
  if [[ "$out" == *"UNSATISFIABLE"* ]]; then
    N=$((N + 1))
    echo "--[SOLVER LOG]--            COMPUTATION DONE - UNSATISFIABLE, INCREASING NUMBER OF STATES - "$N

    truncate -s 0 nfa_sat.cnf
    ../build/fsa_red --sat_nfa_init -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}" > nfa_sat.cnf

    echo "--[AUTO LOG]--              INITIAL GENERATION DONE"
    out=$( ./MiniSat.14_linux nfa_sat.cnf nfa_result.txt )

    continue
    fi

    echo "--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE"
    out=$( ../build/fsa_red --sat_nfa_compare -N $N -S $ALPHA "$AUTO_VTF" )

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

          MAX=$(grep -Eo '[0-9]+' "nfa_sat.cnf" | sort -rn | head -n 1)

          if [ ! -z "${PRO[*]}" ] && [ ! -z "${CON[*]}" ]; then
            EX=("${EX[@]}" "${PRO[@]}")
            CONEX=("${CONEX[@]}" "${CON[@]}")
            ../build/fsa_red --sat_nfa_add_clauses -N $N -S $ALPHA -M $MAX -A "${PRO[@]}" -R "${CON[@]}" >> nfa_sat.cnf
          elif [ ! -z "${PRO[*]}" ]; then
            EX=("${EX[@]}" "${PRO[@]}")
            ../build/fsa_red --sat_nfa_add_clauses -N $N -S $ALPHA -M $MAX -A "${PRO[@]}" >> nfa_sat.cnf
          else [ ! -z "${CON[*]}" ]
            CONEX=("${CONEX[@]}" "${CON[@]}")
            ../build/fsa_red --sat_nfa_add_clauses -N $N -S $ALPHA -M $MAX -R "${CON[@]}" >> nfa_sat.cnf
          fi

          echo "--[AUTO LOG]--              ADDITIONAL GENERATION DONE"
          out=$( ./MiniSat.14_linux nfa_sat.cnf nfa_result.txt )
          fi
      sleep 0.5
  done