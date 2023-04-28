#!/bin/bash
TIMEFORMAT='%3R'
N=20
ALPHA=2
SMALL_E=("0" "0 1")
SMALL_C=("1" "1 1")
MED_E=("0" "0 1" "1 0 1" "1 1 0 1" "1 1 0 1 1")
MED_C=("1" "1 1" "0 0 0" "0 1 0 1" "1 0 1 0 1")
LARGE_E=("0" "0 1" "1 0 1" "1 1 0 1" "1 1 0 1 1" "1 0 0 0 0 1" "1 1 1 0 1 1 1")
LARGE_C=("1" "1 1" "0 0 0" "0 1 0 1" "1 0 1 0 1" "0 0 1 1 0 0" "0 1 1 1 1 1 0")
EX=( "${LARGE_E[@]}" )
CONEX=( "${LARGE_C[@]}" )

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
{ time ./qbf_red --clauses -N $N -S $ALPHA -A "${EX[@]}" -R "${CONEX[@]}"; } > qbf.qdimacs 2>&1
echo "Generate clauses time dfa no header:          $( tail -n 1 qbf.qdimacs )"
sed -i '$ d' qbf.qdimacs

echo "Variables:                                    $(tail -n +4 qbf.qdimacs | grep -Eo '[0-9]+' | sort -rn | head -n 1)"
echo "Clauses:                                      $(($(wc -l < qbf.qdimacs)-5))"

echo "-----------------------------------------------------------------------"
exit
{ time ./caqe --qdo qbf.qdimacs; } > result.txt 2>&1
echo "CAQE time:                                    $( tail -n 1 result.txt )"
echo
echo "-----------------------------------------------------------------------"
exit

{ time ./depqbf-version-6.03/depqbf --qdo qbf.qdimacs; } > result.txt 2>&1
echo "depqbf time:                                  $( tail -n 1 result.txt )"
echo
echo "-----------------------------------------------------------------------"
exit

{ time ./qute/qute --partial-certificate qbf.qdimacs; } > result.txt 2>&1
echo "QUTE time:                                    $( tail -n 1 result.txt )"
echo
echo "-----------------------------------------------------------------------"


