#!/bin/bash
TIMEFORMAT='%3R'

if [ $# -lt 2 ]
  then
    echo "Wrong arguments. Expected the number of states N followed by the size of the input {S | M | L}"
    echo "./unit_test_sat.sh N size"
    exit
fi

N=$1
ALPHA=2
SMALL_E=("0" "0 1")
SMALL_C=("1" "1 1")
MED_E=("0" "0 1" "1 0 1" "1 1 0 1" "1 1 0 1 1")
MED_C=("1" "1 1" "0 0 0" "0 1 0 1" "1 0 1 0 1")
LARGE_E=("0" "0 1" "1 0 1" "1 1 0 1" "1 1 0 1 1" "1 0 0 0 0 1" "1 1 1 0 1 1 1")
LARGE_C=("1" "1 1" "0 0 0" "0 1 0 1" "1 0 1 0 1" "0 0 1 1 0 0" "0 1 1 1 1 1 0")
if [[ $2 == "S" ]]; then
  EX=( "${SMALL_E[@]}" )
  CONEX=( "${SMALL_C[@]}" )
elif [[ $2 == "M" ]]; then
  EX=( "${MED_E[@]}" )
  CONEX=( "${MED_C[@]}" )
else [[ $2 == "L" ]]
  EX=( "${LARGE_E[@]}" )
  CONEX=( "${LARGE_C[@]}" )
fi

echo -n "States: $N; Symbols: $ALPHA; "
echo -n "Accepted words:  "
delim=""
for item in "${EX[@]}"; do
  printf "%s" "$delim$item"
  delim=","
done
echo -n "; "

echo -n "Rejected words:  "
delim=""
for item in "${CONEX[@]}"; do
  printf "%s" "$delim$item"
  delim=","
done
echo
echo "======================================================================"

touch sat.cnf
{ time ../build/fsa_red --sat_init -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}"; } > sat.cnf 2>&1
echo "Generate clauses time dfa no header:          $( tail -n 1 sat.cnf )"
sed -i '$ d' sat.cnf

echo "Variables:                                    $(grep -Eo '[0-9]+' "sat.cnf" | sort -rn | head -n 1)"
echo "Clauses:                                      $(wc -l < sat.cnf)"

out=$( { time ./MiniSat.14_linux sat.cnf sat_result.txt; } 2>&1)
echo "MiniSat time:                                 ${out: -5}"

echo

{ time ../build/fsa_red --sat_init_header -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}"; } > sat.cnf 2>&1
echo "Generate clauses time dfa with header:        $( tail -n 1 sat.cnf )"
sed -i '$ d' sat.cnf

echo "Clauses:                                      $(wc -l < sat.cnf)"

{ time ./kissat -q sat.cnf; } > sat_result.txt 2>&1
echo "Kissat time:                                  $( tail -n 1 sat_result.txt )"
echo
echo "-----------------------------------------------------------------------"
exit

{ time ../build/fsa_red --sat_nfa_init -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}"; } > nfa_sat.cnf 2>&1
echo "Generate clauses time nfa:                    $( tail -n 1 nfa_sat.cnf )"
sed -i '$ d' nfa_sat.cnf

echo "Variables:                                    $(grep -Eo '[0-9]+' "nfa_sat.cnf" | sort -rn | head -n 1)"
echo "Clauses:                                      $(wc -l < nfa_sat.cnf)"

out=$( { time ./MiniSat.14_linux nfa_sat.cnf nfa_result.txt; } 2>&1 )
echo "Minisat time:                                 ${out: -5}"
echo
echo "-----------------------------------------------------------------------"


