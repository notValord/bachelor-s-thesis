/*
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 3.10 2022
* Subject: Bachelor's thesis
*/

#ifndef BAKALARKA_AUTOMATA_H
#define BAKALARKA_AUTOMATA_H

// Defining epsilon transition and dead state as constants
#define EPS "eps"
#define DEAD "D"

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <memory>
#include <set>
#include <list>


class auto_state;
class automata;
class power_element;

using state_set = std::set <std::shared_ptr <auto_state>>;

/// Connects two given strings in alphabetical order
std::string combine_string(const std::string&, const std::string&);

/**
 * Takes values of passed states and combine them into one in alphabetical order
 * @param states Set of pointers to automata states to be merged together
 * @return Value of single state
 */
std::string combine_states(state_set);



/// Checks whether two sets of pointer to states have an intersection
bool has_intersect(const state_set&, const state_set&);



/**
 * Determines the state by taking its transitions for each symbol, combining them into one state
 * and adding a single transition
 * @param dfa Pointer to the automata which is being created
 * @param set_queue Pointer to vector of states waiting to be processed
 * @param current_set The current set of states being processed
 * @param current_state Value of newly created state
 */
void determine_state(const std::shared_ptr<automata>&, std::vector <state_set>*,
                     const state_set&, const std::string&);

/// Replaces all epsilon transitions within the automata, determines it and returns a new one
std::shared_ptr <automata> determine_nfa(const std::shared_ptr <automata>&);

//TODO
void minimal_dfa(const std::shared_ptr <automata>&);

/// Creates the minimal deterministic automata
std::shared_ptr <automata> det_n_min(const std::shared_ptr <automata>& nfa);



/// Checks whether two given automatas have intersection in their languages
bool language_intersect(const std::shared_ptr <automata>&, const std::shared_ptr <automata>&);

/// Checks whether two given automata have equal languages through determination and complements
bool language_equal(const std::shared_ptr <automata>&, const std::shared_ptr <automata>&);


//todo
state_set find_power_closure(std::vector <power_element>&, const std::shared_ptr <auto_state>&);
//todo
void insert_pow_set(power_element, std::vector <power_element>&);
//todo
std::shared_ptr <auto_state> get_smallest_state(const state_set&);

class power_element{
private:
    state_set same_set;
    std::unordered_map <std::string, state_set> transition;

public:
    friend bool operator==(const power_element& first, const power_element& second){
        if (first.same_set == second.same_set){
            return true;
        }
        return false;
    }

    power_element(const std::shared_ptr <auto_state>& state, std::unordered_map <std::string, state_set>& trans);

    explicit power_element(state_set set);

    power_element();

    state_set get_set();
    std::unordered_map <std::string, state_set> get_trans();

    void add_state(const std::shared_ptr <auto_state>& state);
    void set_trans(const std::unordered_map <std::string, state_set>& new_trans);
};


// Class for state of automata - consists of its unique value and a hashtable of transitions to others states,
//                               hashtable maps the symbol of transitions to the vector of all reachable states
//                               through such symbol
class auto_state{
    private:
        std::string value;
        std::unordered_map <std::string, state_set> transitions;

    public:
        // Constructor of tha state needing its value
        explicit auto_state(const std::string &value){
            this->value = value;
        }

    // Method print - prints the value and all transitions of the state
        void print();

    // Method has_eps - checks whether the state has any transition through epsilon symbol,
//                  if so returns true
        bool has_eps();

    // Method get_value - returns the value of the state
        std::string get_value();
    // Method get_trans_row - takes a symbol and returns a pointer of the set of states
//                          reachable through such symbol
        state_set get_trans_row(const std::string& symbol);
        std::unordered_map <std::string, state_set> get_trans();

        void get_pow_trans(std::vector <power_element>& previous,
                           std::unordered_map <std::string, state_set>& new_trans,
                           std::unordered_map <std::shared_ptr <auto_state>, state_set>& helper);

    // Method add_transition - takes a symbol and pointer to another state, creates a new transition
//                       - shows and error message if adding a duplicate transition
        void add_transition(const std::string& symbol, const std::shared_ptr <auto_state>& trans_to);
    // Method add_transition_row - adds a multiple transitions through the same symbol
        void add_transition_row(const std::string& symbol,
                                const state_set& new_trans);

    // Method get_eps_transitions - returns epsilon transitions of the current state,
//                              if there are none returns and empty set
        state_set get_eps_transitions();
    // Method replace_eps_trans - deletes all epsilon transitions and adds all of the transitions form the
//                              set replace passed to the method
        void replace_eps_trans(const state_set& replace);

        std::shared_ptr <auto_state> get_next(const std::string& symbol);

        void reverse_trans(const std::shared_ptr <automata>& reverse);
        void clear_trans();
        void copy_state(const std::shared_ptr <automata>& comple);

        void set_power_state(const std::string& new_val, std::unordered_map <std::string, state_set>& new_trans);
};

// Class automata - represents a finite automata
//                - consists of hashtable state_table mapping pointers to states to their unique value,
//                  alphabet, initial and accept states of the automata
class automata{
    private:
        std::unordered_map <std::string, std::shared_ptr <auto_state>> state_table;
        std::set <std::string> alphabet;
        state_set init_states;
        state_set accept_states;

    // Method recurse_find_all_eps - recursive function used to get epsilon closure of the passed group,
//                               returns the found closure
        state_set recurse_find_all_eps(const state_set& search_group);

    public:
    // Constructor of the automata - takes a set of states, alphabet, initial and accept states
//                               and a vector of transitions
        automata(const std::set <std::string>& states, const std::set <std::string>& alphabet,
                 const std::vector <std::array<std::string, 3>>& trans,
                 const std::set <std::string>& init_states, const std::set <std::string>& fin_states);

    // Constructor of the automata - takes only the alphabet and one state
        automata (const std::string& state, const std::set <std::string>& alphabet);
        automata ();
        ~automata();

        void min_power(const std::vector <power_element>& power_set);
        // Copy constructor TODO je to zle
        std::shared_ptr <automata> copy();

    // Method add_state - adds a new state to the automata, if the state is already present returns false
//                  - in case of adding a dead state the infinite loop is also added and
//                    always returns false
        bool add_state(const std::string& state_value);
        void add_alphabet(const std::string& new_symbol);
    // Method add_transition - adds a new transitions to the automata
//                       - if some of the states doesn't exist in the automata an error message is shown
        void add_transition(const std::string& symbol, const std::string& from, const std::string& to);

        void add_init_state(const std::string& init_state);
        void add_init_state_force(const std::string& init_state);
    // Method add_accept_state - adds accept state to the automata
//                         - if the state doesn't exist in the automata an error message is shown
        void add_accept_state(const std::string& fin_state);
        void add_accept_state(const std::shared_ptr <auto_state>& fin_state);
        void add_accept_state_force(const std::string& fin_state);


        std::set <std::string> get_alphabet();
        std::string get_init();
        unsigned long get_state_number();

        bool is_final(const std::string& state);

        std::string get_next_state(const std::string& symbol, const std::string& state);

    // Method remove_eps_transitions - find epsilon closures for all states and using them replaces all
//                                 epsilon transitions in the automata
        void remove_eps_transitions();

    // Method determine - creates and returns a new deterministic automata representing the same language
        std::shared_ptr <automata> determine();
        std::shared_ptr <automata> reverse();

        void init_power_set(std::vector <power_element>& pow_set);

        static void split_power_state(std::vector <power_element>&, std::vector <power_element>&);

        void reverse_accept_states();
        std::shared_ptr <automata> complement();


    // Method print - prints the automata
        void print();
};

#endif //BAKALARKA_AUTOMATA_H
