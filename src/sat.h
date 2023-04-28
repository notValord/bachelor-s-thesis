/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 16.04.2023
* Subject: Bachelor's thesis - 2st part
*/

#ifndef BAKALARKA_SAT_H
#define BAKALARKA_SAT_H

#include "auto_stats.h"
#include "tseitsen.h"

/***
 * Class for input parameters for SAT reduction, inherits from automata_stats
 *  Variables indexed by rows for N=3 S=2:
 *      1  - T1a1    2  - T1a2    3  - T1a3        4  - T2a1    5  - T2a2    6  - T2a3
 *      7  - T3a1    8  - T3a2    9  - T3a3        10 - T1b1    11 - T1b2    12 - T1b3
 *      13 - T2b1    14 - T2b2    15 - T2b3        16 - T3b1    17 - T3a2    18 - T3b3
 *      19 - F1      20 - F2      21 - F3
 */
class sat_stats : public automata_stats{
    public:
    /***
     * Construction - sets the number of states and symbols, sets of words are empty
     * @param state     number of states
     * @param symbols   number of symbols
     */
    sat_stats(int state, int symbols);

    /***
     * Generates the clauses for determinism of the automaton
     *  -1 or -2    and     -1 or -3    and     -2 or -3
     */
    void determine_clauses() const;

    /***
     * Generates the clauses for completeness of the automaton
     *  1 or 2 or 3
     */
    void complete_clauses() const;

    /***
     * Generates the clauses for accepting and rejecting words, calls recurse_tseitsen and recurse_tseitsen_reject
     * @param max_index     first free variable that can be used for CNF transformation
     */
    void example_clauses(unsigned int max_index);

    /***
     * Recursively generates and stores the clauses for accepting a word in DNF
     * @param base          partial vector for storage for the result
     * @param state         state where the previous transition ended
     * @param word          word that is being accepted
     * @param [out] result  output vector with the formula for CNF transformation
     */
    void recurse_tseitsen(const std::vector<int>& base, unsigned int state, std::queue <unsigned int> word,
                          std::vector<int>& result);

    /***
     * Recursively generates and prints the clauses on stdin for rejecting a word
     * @param base          partial string for storage fo the output
     * @param state         state where the previous transition ended
     * @param word          word that is being rejected
     */
    void recurse_tseitsen_reject(const std::string& base, unsigned int state, std::queue <unsigned int> word);

    void recurse_nfa_tseitsen(const std::vector<int>& base, unsigned int state, std::queue <unsigned int> word,
                          std::vector<int>& result);
    void recurse_nfa_tseitsen_reject(const std::string& base, unsigned int state, std::queue <unsigned int> word);
    void example_nfa_clauses(unsigned int max_index);
};
#endif //BAKALARKA_SAT_H
