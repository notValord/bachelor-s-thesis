#!/bin/bash
TIMEFORMAT='%3R'
ITER=0
TIME_SAT=0
TIME_GEN=0
TIME_COMP=0
TIME_INFO=0
N=0
ALPHA=0
AUTO_VTF="../input_automata/test_auto/armcNFA_inclTest_5.vtf"
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
TMP=$( { time ./sat_red --get_info "$AUTO_VTF"; } 2>&1 )
info=${TMP:: -5}
TIME_INFO=${TMP: -5}

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

touch nfa_sat.cnf

{ time ./sat_red --nfa -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}"; } > nfa_sat.cnf 2>&1
TIME_GEN=$( tail -n 1 nfa_sat.cnf )
sed -i '$ d' nfa_sat.cnf

echo "--[AUTO LOG]--              INITIAL GENERATION DONE"
out=$( { time ./MiniSat.14_linux nfa_sat.cnf nfa_result.txt; } 2>&1 )
TIME_SAT=${out: -5}

while [ $FOUND -eq 0 ]
do
  ITER=$((ITER+1))
  if [[ "$out" == *"UNSATISFIABLE"* ]]; then
    N=$((N + 1))
    echo "--[SOLVER LOG]--            COMPUTATION DONE - UNSATISFIABLE, INCREASING NUMBER OF STATES - "$N

    truncate -s 0 nfa_sat.cnf
    { time ./sat_red --nfa -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}"; } > nfa_sat.cnf 2>&1
    TIME_GEN=$( bc <<<"($TIME_GEN + $( tail -n 1 nfa_sat.cnf ))" )
    sed -i '$ d' nfa_sat.cnf

    echo "--[AUTO LOG]--              INITIAL GENERATION DONE"
    out=$( { time ./MiniSat.14_linux nfa_sat.cnf nfa_result.txt; } 2>&1)
    TIME_SAT=$( bc <<<"($TIME_SAT + ${out: -5})" )

    continue
    fi

    echo "--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE"
    TMP=$( { time ./sat_red --nfa_compare -N $N -S $ALPHA "$AUTO_VTF"; } 2>&1)
    out=${TMP:: -5}
    TIME_COMP=$( bc <<<"($TIME_COMP + ${TMP: -5})" )

      if [[ "$out" == *"PASS"* ]]; then
        FOUND=1
        MAX=$(grep -Eo '[0-9]+' "nfa_sat.cnf" | sort -rn | head -n 1)
        echo "--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE EQUAL"
        echo
        echo "Iterations:                   "$ITER
        echo "Accept words:                 "${#EX[@]}
        echo "Reject words:                 "${#CONEX[@]}
        echo "Variables:                    "$MAX
        echo "Clauses:                      ""$(wc -l < nfa_sat.cnf)"
        echo "Get info time:                "$TIME_INFO
        echo "Generate clauses time:        "$TIME_GEN
        echo "Compare time:                 "$TIME_COMP
        echo "Solver time:                  "$TIME_SAT

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
            { time ./sat_red --nfa_add -N $N -S $ALPHA -M $MAX -A "${PRO[@]}" -R "${CON[@]}"; } >> nfa_sat.cnf 2>&1
          elif [ ! -z "${PRO[*]}" ]; then
            EX=("${EX[@]}" "${PRO[@]}")
            { time ./sat_red --nfa_add -N $N -S $ALPHA -M $MAX -A "${PRO[@]}"; } >> nfa_sat.cnf 2>&1
          else [ ! -z "${CON[*]}" ]
            CONEX=("${CONEX[@]}" "${CON[@]}")
            { time ./sat_red --nfa_add -N $N -S $ALPHA -M $MAX -R "${CON[@]}"; } >> nfa_sat.cnf 2>&1
          fi
          TIME_GEN=$( bc <<<"($TIME_GEN + $( tail -n 1 nfa_sat.cnf ))" )
          sed -i '$ d' nfa_sat.cnf


          echo "--[AUTO LOG]--              ADDITIONAL GENERATION DONE"
          out=$( { time ./MiniSat.14_linux nfa_sat.cnf nfa_result.txt; } 2>&1)
          TIME_SAT=$( bc <<<"($TIME_SAT + ${out: -5})" )

          fi
      sleep 0.5
  done