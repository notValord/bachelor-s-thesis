/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 15.04.2023
* Subject: Bachelor's thesis - 2st part
*/

#ifndef BAKALARKA_QBF_H
#define BAKALARKA_QBF_H

#include <cmath>

#include "tseitsen.h"
#include "auto_stats.h"

using uint = unsigned int;

void recurse_init_final(const std::string& expr, uint var_base, uint& result_base, uint iter, uint max);

/***
 * Class for input parameters for QBF reduction, inherits from automata_stats
 *  Variables indexed by rows for N=3 S=2:
 *      1  - T1a1    2  - T1a2    3  - T1a3        4  - T2a1    5  - T2a2    6  - T2a3
 *      7  - T3a1    8  - T3a2    9  - T3a3        10 - T1b1    11 - T1b2    12 - T1b3
 *      13 - T2b1    14 - T2b2    15 - T2b3        16 - T3b1    17 - T3a2    18 - T3b3
 *      19 - I1      20 - I2      21 - I3          22 - F1      23 - F2      24 - F3
 */
class qbf_stats : public automata_stats {
    /** size fo the binary vector representing the state*/
    uint state_bin;

public:
    /***
     * Constructor - sets the number of states, symbols and size of binary vector
     * @param states    number of states
     * @param symbols   number of symbols
     */
    qbf_stats(int states, int symbols);

    /***
     * Getter of state_bin
     * @return  the size of binary vector
     */
    [[nodiscard]] unsigned int get_bin() const;

    /***
     * Generates and prints clauses for setting the initial and final state for accepting the word
     * @param state_base    first variable of the first state
     * @param end_base      first variable of the last state
     */
    void init_final_clauses(uint state_base, uint end_base);
    /***
     * Generates and stores clauses for setting the initial and final state for rejecting the word
     * @param state_base        first variable of the first state
     * @param end_base          first variable of the last state
     * @param [out] result      vector where the generated clauses are stored for CNF transformation
     */
    void init_final_clauses_reject(uint state_base, uint end_base, std::vector <int>& result);

    /***
     * Prints the clauses for covering the invalid combinations of the binary vector for accepting a words
     *  that have to be rejected
     * @param start     first variable of the given state
     */
    void valid_combinations(uint start);
    /***
     * Stores the clauses for covering the invalid combinations of the binary vector for rejected a words
     *  that have to be accepted
     * @param start         first variable of the given state
     * @param [out] input   vector where the generated clauses are stored for CNF transformation
     */
    void valid_combinations_reject(uint start, std::vector <int>& input);

    /***
     *  Recursively generates the transition clauses for accepting a word
     * @param expr          partial output string
     * @param var_base      state variable of the binary vector
     * @param trans_base    transition variable
     * @param iter          levels of recursion, is equal to the size of the binary vector
     * @param cnt           counter of valid combinations of binary vector
     * @param end           flag for the second iteration of the second state in the transition,
     *                      when ends generates a clause
     */
    void recurse_accept(const std::string& expr, uint var_base, uint& trans_base, uint iter, uint& cnt, bool end=false);
    /***
     *  Generates the transition clauses for accepting a word, calls recurse_accept
     * @param var       first variable of the given state
     * @param trans     first transition variable in the symbols block
     */
    void accept_clauses(uint var, uint trans);


    /***
     * Recursively generates adn stores the transition clauses for rejecting a word
     * @param base          vector for partial storage
     * @param var_base      state variable of the binary vector
     * @param trans_base    transition variable
     * @param iter          levels of recursion, is equal to the size of the binary vector
     * @param cnt           counter of valid combinations of binary vector
     * @param [out] result  vector where the generated clauses are stored for CNF transformation
     * @param end           flag for the second iteration of the second state in the transition,
     *                      when ends generates a clause
     */
    void recurse_reject(const std::vector <int>& base, uint var_base, uint& trans_base, uint iter, uint& cnt,
                                   std::vector <int>& result, bool end=false);
    /***
     * Generates adn stores the transition clauses for rejecting a word, calls recurse_reject
     * @param var           first variable of the given state
     * @param trans         first transition variable in the symbols block
     * @param [out] result  vector where the generated clauses are stored for CNF transformation
     */
    void reject_clauses(uint var, uint trans, std::vector <int>& result);

    /***
     * Generates the clauses for example words that should be accepted or rejected by the automaton
     * @param tsei_start    index of the first variable that is free and
     *                      can be used for new variables for CNF transformation (includes the state variables)
     */
    void example_clauses(uint tsei_start);
};

#endif //BAKALARKA_QBF_H
