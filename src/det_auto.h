/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#ifndef BAKALARKA_DET_AUTO_H
#define BAKALARKA_DET_AUTO_H

#include "automata_base.h"
#include "auto_stats.h"

class det_auto;

/***
 * Function for determinization of the given automaton
 * @return          an equivalent DFA to the given automaton
 */
std::shared_ptr <det_auto> determine_nfa(const std::shared_ptr <det_auto>&);

/***
 * Class inheriting from the base automaton adding the determinization, reverse, complement, and copy operation
 */
class det_auto : public automata{
public:
    /***
     * Constructor  - initializes all of the attributed of the automaton
     * @param states        states of the automaton
     * @param alphabet      alphabet of the automaton
     * @param trans         transitions of the automaton in format from_state : symbol : to_state
     * @param init_states   initial state of the automaton
     * @param fin_states    final states of the automaton
     */
    det_auto(const std::set <std::string>& states, const std::set <std::string>& alphabet,
             const std::vector <std::array<std::string, 3>>& trans,
             const std::set <std::string>& init_states, const std::set <std::string>& fin_states);

    /***
     * Constructor  - creates an automaton with a single state and with the alphabet
     * @param state             single state of the automaton
     * @param alphabet          alphabet of the automaton
     */
    det_auto(const std::string& state, const std::vector <std::string>& alphabet);
    /***
     * Constructor  - creates an automaton with alphabet and the dictionary of the automaton
     * @param alphabet      alphabet fo the automaton
     * @param dict          initialized dictionary of the automaton
     */
    det_auto(int alphabet, auto_dictionary& dict);
    /***
     * Constructor  - creates only an empty automaton
     */
    det_auto();

    /***
     * Copy function of the automaton
     * @return      a newly created equal instance of the automaton
     */
    std::shared_ptr <det_auto> copy();

    /***
     * Reverses the structure of the automaton
     * @return      a newly created instance of a reversed automaton
     */
    std::shared_ptr <det_auto> reverse();

    /***
     * Function for reversing the accept and non-accept states
     */
    void reverse_accept_states();
    /***
     * Creates a complement of the automaton
     * @return      a newly created instance of a complement automaton
     */
    std::shared_ptr <det_auto> complement();


    /***
     * Combines the values of the states in the vector into a single value by concatenation from the smallest to highest
     * @param states        vector of states whose values will be combined
     * @return              a string as a new value created from concatenation
     */
    std::string combine_states(ptr_state_vector states);

    /***
     *  Iteratively finds set of states that needs to be merged and represented as one for determinization
     * @param old               old nondeterministic automaton
     * @param set_queue         queue of sets of states that needs to be searched for new states for the DFA
     * @param current_set       set of states that will form the new state, from those states the iteration
     *                              proceeds further
     * @param current_state     value of the newly created state
     */
    void determine_state(det_auto* old, std::vector <ptr_state_vector>& set_queue,
                         const ptr_state_vector& current_set, int current_state);
    /***
     *  recursively find the whole epsilon closure of the state
     * @param search_group      set of states to search for epsilon transitions
     * @param eps_index         index of the epsilon transition
     * @param ret               the final epsilon closure that is returned
     */
    void recurse_find_all_eps(const ptr_state_vector& search_group, int eps_index, ptr_state_vector& ret);
    /***
     *  Removes the epsilon transitions in the automaton by finding the epsilon closure of each state and replace
     *      them with transition with symbols
     */
    void remove_eps_transitions();
    /***
     * Creates a new deterministic instance of the automaton
     * @return          a newly created instance of a DFA equal to this automaton
     */
    std::shared_ptr <det_auto> determine();

    /***
     * Traverses and finds example of words that the automaton accepts and rejects, finds only states
     *  up to the length of the shortest accepting word
     * @param stats     structure holding the found words
     */
    void find_examples(const std::shared_ptr<automata_stats>& stats);
};
#endif //BAKALARKA_DET_AUTO_H
