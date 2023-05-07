/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#ifndef BAKALARKA_REZIDUAL_AUTO_H
#define BAKALARKA_REZIDUAL_AUTO_H

#include "det_auto.h"

/***
 * Function for reduction by transformation into canonical RSA
 * @param nfa           the original NFA
 * @return              a created instance of CRSA
 */
std::shared_ptr <det_auto> rezidual_auto(const std::shared_ptr <det_auto>& nfa);

/***
 * Class representing the RSA, inheriting from the deterministic automaton adding methods required for finding CRSA
 */
class rezid_auto : public det_auto{
public:
    /***
     * Constructor  - creates an empty instance of automaton
     */
    rezid_auto();
    /***
     * Convert constructor from the det_auto class
     * @param base          the deterministic automaton from which the converting is going
     */
    rezid_auto(det_auto& base);      //convert constructor

    /***
     * Removes the covered state by the covering vector from the automaton
     * @param state_value           the state that is being removed
     * @param covering              vector of states covering the given state
     */
    void remove_rezidual_state(const std::string& state_value, std::vector <int>& covering);
    /***
     * Recursively find all states of the RSA by searching the automaton
     * @param base                  the set of states to be searched
     * @param rezid                 the newly created residual automaton
     * @param base_value            the value of the searched state
     * @param all_states            set fo all states that have been created
     */
    void create_rezidual_state(const ptr_state_vector & base, const std::shared_ptr <rezid_auto>& rezid,
                               int base_value, std::vector <ptr_state_vector>& all_states);

    /***
     * Recursively checks for coverable states in the covering vector
     * @param check_index           index of the state that is being checked
     * @param states                vector of all original state of the automaton states
     * @param start                 index from which the search for covering states starts
     * @param covering              set of states that could cover the given state
     * @param checked_out           help table with the flags of state which have been already checked
     * @param rezid                 the result RSA
     * @return                      true if the given state is covered, else returns false
     */
    bool is_recurse_coverable(int check_index, const std::vector <ptr_state_vector>& states, int start,
                              const std::vector <int>& covering, std::vector <bool>& checked_out,
                              const std::shared_ptr <rezid_auto>& rezid);

    /***
     * Checks whether the states given by the check_index is covered in the rezid automaton, if so can be removed
     * @param check_index           index of the state that is being checked
     * @param states                vector of all original state of the automaton states
     * @param start                 index from which the search for covering states starts
     * @param checked_out           help table with the flags of state which have been already checked
     * @param rezid                 the result RSA
     */
    void is_coverable(int check_index, const std::vector <ptr_state_vector>& states, int start,
                      std::vector <bool>& checked_out, const std::shared_ptr<rezid_auto>& rezid);

    /***
     * Method for creating a CRSA of this automaton
     * @return          a newly creates instance of CRSA
     */
    std::shared_ptr <det_auto> rezidual();
};


#endif //BAKALARKA_REZIDUAL_AUTO_H
