/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#ifndef BAKALARKA_SIMULATION_AUTO_H
#define BAKALARKA_SIMULATION_AUTO_H

#include "det_auto.h"

class simul_auto;

/***
 * Creates the symmetric fragment of the given matrix
 * @param omega_matrix              the matrix that will be modified to be symmetric
 */
void symetric_fragment(std::vector<std::vector<bool>>& omega_matrix);

/***
 * Modify the automaton through relation of simulation to reduce it
 * @param nfa                       the given automaton to be reduced, this automaton will be modified in the process
 */
void simulate_min(const std::shared_ptr <simul_auto>& nfa);

/***
 * Class for the automaton reduced by simulation relation, inherits from the deterministic automaton and add
 *  method needed for the relation fo simulation
 */
class simul_auto: public det_auto{
public:
    /***
     * Convert constructor from the base deterministic automaton
     * @param base          the base automaton for conversion
     */
    simul_auto(det_auto& base);      //convert constructor

    /***
     * Modify the automaton based on the relation of simulation to cause reduction by merging states that are being simulates
     * @param omega_matrix          relation of simulation
     */
    void minimal_sim(std::vector<std::vector<bool>>& omega_matrix);
    /***
     * The algorithm for finding the relation of simulation for the automaton
     * @param omega_matrix          the result relation of simulation
     */
    void create_simulate_matrix(std::vector<std::vector<bool>>& omega_matrix);
    /***
     *  Initialize card tables with the number of transitions for every symbol of the alphabet
     * @param card_tables
     */
    void init_card_tables(std::vector <std::vector <std::vector<int>>>& card_tables);
    /***
     * Initialize the omega matrix with the final and non-final states, and state with transition and no transition
     */
    void init_omega_matrix(std::vector<std::vector<bool>>&, std::vector <std::pair <int, int>>&);
};

#endif //BAKALARKA_SIMULATION_AUTO_H
