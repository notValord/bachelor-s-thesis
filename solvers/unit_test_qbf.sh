#!/bin/bash
TIMEFORMAT='%3R'

if [ $# -lt 2 ]
  then
    echo "Wrong arguments. Expected the number of states N followed by the size of the input {XS | S | M | L}"
    echo "./unit_test_sat.sh N size"
    exit
fi

N=$1
ALPHA=2
EXTRA_E=("0 1")
EXTRA_C=("1 1")
SMALL_E=("0" "0 1")
SMALL_C=("1" "1 1")
MED_E=("0" "0 1" "1 0 1" "1 1 0 1" "1 1 0 1 1")
MED_C=("1" "1 1" "0 0 0" "0 1 0 1" "1 0 1 0 1")
LARGE_E=("0" "0 1" "1 0 1" "1 1 0 1" "1 1 0 1 1" "1 0 0 0 0 1" "1 1 1 0 1 1 1")
LARGE_C=("1" "1 1" "0 0 0" "0 1 0 1" "1 0 1 0 1" "0 0 1 1 0 0" "0 1 1 1 1 1 0")
if [[ $2 == "XS" ]]; then
  EX=( "${EXTRA_E[@]}" )
  CONEX=( "${EXTRA_C[@]}" )
elif [[ $2 == "S" ]]; then
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

{ time ../build/fsa_red --qbf_clauses -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}"; } > qbf.qdimacs 2>&1
echo "Generate clauses time nfa with header:        $( tail -n 1 qbf.qdimacs )"
sed -i '$ d' qbf.qdimacs

echo "Variables:                                    $(tail -n +4 qbf.qdimacs | grep -Eo '[0-9]+' | sort -rn | head -n 1)"
echo "Clauses:                                      $(($(wc -l < qbf.qdimacs)-5))"

echo "-----------------------------------------------------------------------"

{ time ./caqe --qdo qbf.qdimacs; } > qbf_result.txt 2>&1
echo "CAQE time:                                    $( tail -n 1 qbf_result.txt )"
echo
echo "-----------------------------------------------------------------------"

{ time ./depqbf-version-6.03/depqbf --qdo --no-dynamic-nenofex qbf.qdimacs; } > qbf_result.txt 2>&1
echo "DepQBF time:                                  $( tail -n 1 qbf_result.txt )"
echo
echo "-----------------------------------------------------------------------"

{ time ../build/fsa_red --qbf_clauses_qute -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}"; } > qbf.qdimacs 2>&1
echo "Generate clauses time nfa for qute:           $( tail -n 1 qbf.qdimacs )"
sed -i '$ d' qbf.qdimacs

{ time ./qute/qute --partial-certificate qbf.qdimacs; } > qbf_result.txt 2>&1
echo "QUTE time:                                    $( tail -n 1 qbf_result.txt )"
echo
echo "-----------------------------------------------------------------------"


