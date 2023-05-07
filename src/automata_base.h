/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#ifndef BAKALARKA_AUTOMATA_BASE_H
#define BAKALARKA_AUTOMATA_BASE_H

#include <vector>
#include <set>
#include <memory>
#include <fstream>
#include <filesystem>

#include "auto_dictionary.h"

// constant for "dead" / sink state in the automaton
#define DEAD "D"
// constant for the notation of epsilon transitions
#define EPS "eps"

class automata;
class auto_state;

using ptr_state_vector = std::vector<std::shared_ptr<auto_state>>;

/***
 * Checks whether the two vectors of automata_state pointers have a common state
 * @param first     First vector of state pointers
 * @param second    Second vector of state pointers
 * @return          True if vectors have an intersection, else False is returned
 */
bool has_intersect(ptr_state_vector& first, ptr_state_vector& second);

/***
 *  Finds the state with the smallest index from the given vector
 * @param states    Vector of automata states that is going to be searched
 * @return          Pointer to the state with the smallest index
 */
std::shared_ptr <auto_state> get_smallest_state(const ptr_state_vector& states);

/***
 * Class for representing the state of the automaton
 */
class auto_state {
protected:
    /** Index of the state in the automaton */
    int index;
    /** Vector with transitions for each symbol as a vector of pointers to the states the transition is leading to*/
    std::vector<std::shared_ptr<ptr_state_vector>> transitions;

public:
    /***
     * Constructor - sets the value of the state, transitions are left empty
     * @param value         value of the state
     */
    explicit auto_state(int value){
        this->index = value;
    }

    /***
     * Debug function to print the value of the state as well as all transitions from this state
     */
    void print();
    /***
     * Checks whether the state has a transition through the given index of the symbol
     * @param value         index of the symbol of the alphabet
     * @return              true if there exists a transition leading from this state through the given symbol
     */
    bool has_trans(int value);

    /***
     * Getter of the index of the state
     * @return              the index of the state
     */
    [[nodiscard]] int get_value() const;
    /***
     * Setter of index of the state
     * @param new_val       the new value fo the index of the state
     */
    void set_value(int new_val);

    /***
     * Getter of the row of the transition vector through the given symbol
     * @param symbol        the symbol over the transitions
     * @return              gets the vector of transitions leading from this state through the given symbol,
     *                          if the are no such transitions nullptr is returned
     */
    std::shared_ptr<ptr_state_vector> get_trans_row(int symbol);
    /***
     * Getter of the whole transition vector
     * @return              the transition vector of this state
     */
    std::vector <std::shared_ptr<ptr_state_vector>>& get_trans();

    /***
     * Adds a new transition going from this state through the given symbol
     * @warning             cannot add duplicates of transitions
     * @param symbol        the symbol of the new transition
     * @param trans_to      the state where the transition is going to
     */
    void add_transition(int symbol, const std::shared_ptr <auto_state>& trans_to);
    /***
     * Adds a multiple transitions over the same given symbol, each leading to one of the states of the new_row
     * @param symbol        the symbol of the new transitions
     * @param new_row       the vector of states to each of which a new transition si created over the given symbol
     */
    void add_transition_row(int symbol, const ptr_state_vector& new_row);

    /***
     * Gets the first transition from the vector over the given symbol and returns the state it is leading to
     * @param symbol        the symbol of the searched transition
     * @return              the next state we can get with a transition over the given symbol
     * @warning             takes only the first transition
     */
    std::shared_ptr <auto_state> get_next(int symbol);

    /***
     * For every transition for this state create its reversed version in the automaton reverse
     * @param reverse       pointer to a reversed automaton
     */
    void reverse_trans(const std::shared_ptr <automata>& reverse);
    /***
     * Deletes every transition from this state
     */
    void clear_trans();
    /***
     * Creates a copy of all transitions from this state to the automaton copy_auto
     * @param copy_auto         copy of the automaton where the copy of transitions are creating
     */
    void copy_state(const std::shared_ptr<automata>& copy_auto);

    //used in det_auto.cpp
    /***
     * Removes all epsilon transitions of this state and adds all transition of states in replace vector
     * @param replace           vector of states (epsilon closure kinda) from which all of the transitions
     *                              are copied to current state
     * @param eps_index         index of the epsilon transition
     */
    void replace_eps_trans(const ptr_state_vector& replace, int eps_index);

    //used in min_det.cpp
    /***
     * Gets the transition of power state in regards to to old vector stored in helper matrix
     * @param new_trans         variable storing the found transitions
     * @param helper            matrix with the closures based on the index of the state
     */
    void get_pow_trans(std::vector<ptr_state_vector>& new_trans, std::vector<ptr_state_vector>& helper);
    /***
     * Changes the transition from the state over the symbol to only the smallest one
     * @param new_trans         vector of states from which only teh smallest one is found
     */
    void set_power_state(std::vector <ptr_state_vector>& new_trans);

    //used in simul.cpp
    /***
     * Change the transition to lead to the smallest state from the merged state based on the help table
     * @param help_table            table with every state and a new set of states that it is in
     */
    void check_simul_trans(const std::vector <ptr_state_vector>& help_table);
    /***
     * Checks whether the state is simulating the state second aka if it has a transition over a symbol
     *      while the other states doesn't
     * @param second                second state to check the relation of simulation to
     * @return                      true if there is no relation of simulation, else false is returned
     */
    bool not_under_simulate(const std::shared_ptr <auto_state>& second);

    /***
     * Gets the number of transitions for the given symbol
     * @param symbol                symbol of the transitions
     * @return                      number of transitions leading from this state through the given symbol
     */
    int get_trans_card(const int symbol);

    //used in rezidual.cpp
    /***
     * Changes transitions leading to a coverable state in the RSA, the transition leading to this state will be
     * redirected to the covering states
     * @param covering_ptr          the set of states covering the removed state
     * @param change                the covered state that is being removed from the automaton
     */
    void change_coverable(ptr_state_vector& covering_ptr, const std::shared_ptr<auto_state>& change);
};


/***
 * Class for representing the finite state automaton
 */
class automata{
protected:
    /** size of the alphabet of the automaton*/
    int alphabet = 0;
    /** vector with indexes of initial states*/
    std::vector <int> init_states;
    /** vector with indexes of final states*/
    std::vector <int> accept_states;
    /** automaton dictionary to cenvert name to index and vice versa*/
    auto_dictionary dict;

    /** table with all states of the automaton*/
    std::vector <std::shared_ptr <auto_state>> state_table;
public:
    /***
     * Constructor  - sets all of the attributes of the class
     * @param states        the states of the automaton
     * @param alphabet      the alphabet of the automaton
     * @param trans         the transitions in format from_state : symbol : to_state
     * @param init_states   the initial states
     * @param fin_states    the final states
     */
    automata(const std::set <std::string>& states, const std::set <std::string>& alphabet,
             const std::vector <std::array<std::string, 3>>& trans,
             const std::set <std::string>& init_states, const std::set <std::string>& fin_states);

    /***
     * Constructor  - creates an automaton with a single state and an alphabet
     * @param state         single state of the automaton
     * @param alphabet      the alphabet fo the automaton
     */
    automata (const std::string& state, const std::vector <std::string>& alphabet);
    /***
     * Construction - creates an automaton based on the dictionary and an size of the alphabet,
     *  doesn't generate state table
     * @param alphabet      size of the alphabet
     * @param dict          the created dictionary of the automaton
     */
    automata (int alphabet, auto_dictionary& dict);
    /***
     * Constructor of the empty automaton
     */
    automata ();
    /***
     * Destructor of the automaton
     */
    ~automata();

    /***
     * Adds a new state in the automaton, if the state already exists no state is added,
     *  if a sink state is added transitions looping in this state are also generated
     * @param state_value       value of the state to add
     * @return                  true if the state was added into the automaton, if a sink state is added or
     *                              the state was already in the automaton returns false
     */
    bool add_state(const std::string& state_value);
    /***
     * Adds new symbol of the alphabet
     * @param new_symbol        value of the symbol to be added
     */
    void add_alphabet(const std::string& new_symbol);

    /***
     * Adds a new transition into the automaton
     * @param symbol            symbol of the transition to be created
     * @param from              state from where the transition is going
     * @param to                state to where the transition is going
     * @warning                 states and symbol must already exist in the automaton, cannot add duplicate transitions
     */
    void add_transition(const std::string& symbol, const std::string& from, const std::string& to);
    /***
     * Adds a transition with the indices of the states and the symbol of the alphabet
     * @param symbol            index of the symbol of the transition to be created
     * @param from              index of the state from where the transition is going
     * @param to                index of the state to where the transition is going
     * @warning                 states and symbol must already exist in the automaton, cannot add duplicate transitions
     */
    void add_transition(int symbol, int from, int to);
    /***
     * Adds a transition with the indices of the states and the symbol of the alphabet
     * @param symbol            index of the symbol of the transition to be created
     * @param from              index of the state from where the transition is going
     * @param to                index of the state to where the transition is going
     * @attention               if the states are not present in the automaton they will be created,
     *                              the symbol must be in the automaton
     */
    void create_transition(const int symbol, const int from, const int to);

    /***
     * Adds a new initial state
     * @param init_state        index of the state that should be added to initial states
     * @warning                 the state must already exist in the automaton and cannot add duplicate initial states,
     *                              else an error occurs
     */
    void add_init_state(int init_state);
    /***
     * Adds a new initial state
     * @param init_state        index of the state that should be added to initial states
     * @attention               if the state is not present in the automaton it will be created, can cause duplicates
     */
    void add_init_state_force(const std::string& init_state);

    /***
     * Adds a new final state
     * @param fin_state         index of the state that should be added to final states
     * @warning                 the state must already exist in the automaton and cannot add duplicate initial states,
     *                              else an error occurs
     */
    void add_accept_state(int fin_state);
    /***
     * Adds a new final state
     * @param fin_state         index of the state that should be added to final states
     * @attention               if the state is not present in the automaton it will be created, can cause duplicates
     */
    void add_accept_state_force(const std::string& fin_state);

    /***
     * Getter of the size of the alphabet
     * @return                  the size of alphabet of the automaton
     */
    int get_alphabet() const;

    /***
     * Getter of the initial state
     * @return                  the first element in the initial states vector
     * @warning                 return only the first element (DFA), if empty the use of this function may
     *                              behave unpredictably
     */
    int get_init();
    /***
     * Getter of a pointer to a state based on the index
     * @param index             index of the searched state
     * @return                  pointer to the given state or nullptr if indexing out of range
     */
    std::shared_ptr <auto_state> get_state(int index);
    /***
     * Getter of the initial states vector
     * @return                  the whole vector of the initial states
     */
    std::vector <int>& get_init_vec();
    /***
     * Getter of the final states vector
     * @return                  the whole vector of the final states
     */
    const std::vector <int>& get_acc_vec();
    /***
     * Getter of the dictionary of the automaton
     * @return                  the dictionary of the automaton
     */
    const auto_dictionary& get_dict();
    /***
     * Getter of the state table of the automaton
     * @return                  the whole state table fo the automaton
     */
    const std::vector <std::shared_ptr <auto_state>>& get_state_table();

    /***
     * Gets the number of state of the automaton as the size of the state table, does not have to correspond
     *  to the number of states as some cells may be empty in vector is not smoothed out
     * @return                  the size of the state table
     */
    unsigned long get_state_number();

    /***
     * Checks whether the state given by the index is final or not
     * @param state             index of the state to check
     * @return                  true if the state is final, else false is returned
     */
    bool is_final(const int& state);
    /***
     * Checks whether the product state is final, meaning that whether at least one of the state in check set is final
     * @param check             product to check as a set of states of the automaton
     * @return                  true if at least one element from the set is a final state, else returns false
     */
    bool is_product_final(const std::set <int>& check);
    /***
     * Checks whether the automaton has the same alphabet as this one
     * @param second            the automaton to compare to
     * @return                  true if the alphabets are equal, else false is returned
     */
    bool same_alphabets(const std::shared_ptr <automata>& second);

    /***
     * Gets the next state of the given state with transition over the given symbol, uses get_next for auto_state
     * @param symbol            the symbol of the transition
     * @param state             the symbol from which the transition should be leading
     * @return                  the index of the next state from the given state through the transition with the given symbol,
     *                              if there is no such state, returns -1
     * @warning                 if the state doesn't exist in the automaton error occurs and programs exits
     */
    int get_next_state(const int& symbol, const int& state);
    /***
     * Gets the next vector of indices of state where a transitions are leading through the given symbol from the given state
     * @param symbol            symbol of the transition
     * @param state             state from which the transition should be leading
     * @return                  the vector of indices of states reachable from the given state through the given symbol,
     *                              if there are no such states empty vector is returned
     */
    std::vector <int> get_next_vec(const int& symbol, const int& state);

    /***
     * Debug function to print all of the contents of the automaton
     */
    void print();
    /***
     * Saves the instance of the automaton into a file in .vtf format
     * @param filename              name of the file where the automaton will be stored
     * @param type                  comment on the top of the file
     */
    void save_to_file(const std::string& filename, const std::string& type);
};

#endif //BAKALARKA_AUTOMATA_BASE_H
