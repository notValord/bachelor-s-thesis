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
#include <cstring>

#include "auto_dictionary.h"


class auto_state;
class automata;
class power_element;

using ptr_state_vector = std::vector<std::shared_ptr<auto_state>>;


/// Checks whether two sets of pointers to states have an intersection
bool has_intersect(ptr_state_vector& first, ptr_state_vector& second);

/// Creates the minimal deterministic automata
std::shared_ptr <automata> det_n_min(const std::shared_ptr <automata>& nfa);

/// Replaces all epsilon transitions within the automata, determines it and returns a new one
std::shared_ptr <automata> determine_nfa(const std::shared_ptr <automata>&);

/// Checks whether two vectors of pointers to power element are equal
bool is_eq(std::vector <std::shared_ptr <power_element>>&, std::vector <std::shared_ptr <power_element>>&);

/// Creates minimal equivalent of deterministic automata using power states
void minimal_dfa(const std::shared_ptr <automata>&);


void symetric_fragment(std::vector<std::vector<bool>>& omega_matrix);

void simulate_min(const std::shared_ptr <automata>& nfa);

std::shared_ptr<automata> rezidual_auto(const std::shared_ptr <automata>& nfa);

/// Checks whether two given automatas have intersection in their languages
bool language_intersect(const std::shared_ptr <automata>&, const std::shared_ptr <automata>&);

/// Checks whether two given automata have equal languages through determination and complements
bool language_equal(const std::shared_ptr <automata>&, const std::shared_ptr <automata>&);

void insert_pow_set(const std::shared_ptr <power_element>&, std::vector <std::shared_ptr<power_element>>&);

void init_power_hash(std::vector <ptr_state_vector>&, const std::vector <std::shared_ptr<power_element>>&);

std::shared_ptr <auto_state> get_smallest_state(const ptr_state_vector& states);

class power_element{
private:
    ptr_state_vector same_set;      //todo pointre/referencie
    std::vector <ptr_state_vector> transition;

public:
    friend bool operator==(const power_element& first, const power_element& second){
        if (first.same_set == second.same_set){
            return true;
        }
        return false;
    }

    friend bool operator!=(const power_element& first, const power_element& second){
        if (first.same_set != second.same_set){
            return true;
        }
        return false;
    }

    power_element(const std::shared_ptr <auto_state>& state, std::vector<ptr_state_vector>& trans);

    explicit power_element(ptr_state_vector set);

    power_element();

    ptr_state_vector& get_set();
    std::vector<ptr_state_vector>& get_trans();

    void add_state(const std::shared_ptr <auto_state>& state);
    void set_trans(const std::vector<ptr_state_vector>& new_trans);
};


// Class for state of automata - consists of its unique value and a hashtable of transitions to others states,
//                               hashtable maps the symbol of transitions to the vector of all reachable states
//                               through such symbol
class auto_state{
    private:
        int index;
        std::vector <std::shared_ptr<ptr_state_vector>> transitions;

    public:
        // Constructor of tha state needing its value
        explicit auto_state(int value){
            this->index = value;
        }

    // Method print - prints the value and all transitions of the state
        void print();

    // Method has_eps - checks whether the state has any transition through epsilon symbol,
//                  if so returns true
        bool has_trans(int value);

    // Method get_value - returns the value of the state
        int get_value() const;
        void set_value(int new_val);

    // Method get_trans_row - takes a symbol and returns a pointer of the set of states
//                          reachable through such symbol
        std::shared_ptr<ptr_state_vector> get_trans_row(int symbol);
        std::vector <std::shared_ptr<ptr_state_vector>>& get_trans();

        void get_pow_trans(std::vector<ptr_state_vector>& new_trans, std::vector<ptr_state_vector>& helper);

    // Method add_transition - takes a symbol and pointer to another state, creates a new transition
//                       - shows and error message if adding a duplicate transition
        void add_transition(int symbol, const std::shared_ptr <auto_state>& trans_to);
    // Method add_transition_row - adds a multiple transitions through the same symbol
        void add_transition_row(int symbol, const ptr_state_vector& new_row);

    // Method get_eps_transitions - returns epsilon transitions of the current state,
//                              if there are none returns and empty set
        //state_set get_eps_transitions();
    // Method replace_eps_trans - deletes all epsilon transitions and adds all of the transitions form the
//                              set replace passed to the method
        void replace_eps_trans(const ptr_state_vector& replace, int eps_index);

        std::shared_ptr <auto_state> get_next(const int symbol);

        void reverse_trans(const std::shared_ptr <automata>& reverse);
        void clear_trans();
        void copy_state(const std::shared_ptr<automata>& copy_auto);

        void set_power_state(std::vector <ptr_state_vector>& new_trans);
        void check_simul_trans(const std::vector <ptr_state_vector>& help_table);

        int get_trans_card(const int symbol);
        bool not_under_simulate(const std::shared_ptr <auto_state>& second);

        void change_coverable(ptr_state_vector& covering_ptr, const std::shared_ptr<auto_state>& change);
};

// Class automata - represents a finite automata
//                - consists of hashtable state_table mapping pointers to states to their unique value,
//                  alphabet, initial and accept states of the automata
class automata{
    private:

    std::vector <std::shared_ptr <auto_state>> state_table;
    int alphabet = 0;
    std::vector <int> init_states;
    std::vector <int> accept_states;
    auto_dictionary dict;

    // Method recurse_find_all_eps - recursive function used to get epsilon closure of the passed group,
//                               returns the found closure
    void recurse_find_all_eps(const ptr_state_vector& search_group, int eps_index, ptr_state_vector& ret);

    public:
    // Constructor of the automata - takes a set of states, alphabet, initial and accept states
//                               and a vector of transitions
        automata(const std::set <std::string>& states, const std::set <std::string>& alphabet,
                 const std::vector <std::array<std::string, 3>>& trans,
                 const std::set <std::string>& init_states, const std::set <std::string>& fin_states);

    // Constructor of the automata - takes only the alphabet and one state
        automata (const std::string& state, const std::vector <std::string>& alphabet);
        automata (int alphabet, auto_dictionary& dict);
        automata ();
        ~automata();

        void min_power(const std::vector <std::shared_ptr<power_element>>& power_set);
        // Copy constructor TODO
        std::shared_ptr <automata> copy();

    // Method add_state - adds a new state to the automata, if the state is already present returns false
//                  - in case of adding a dead state the infinite loop is also added and
//                    always returns false
        bool add_state(const std::string& state_value);

        void add_alphabet(const std::string& new_symbol);
    // Method add_transition - adds a new transitions to the automata
//                       - if some of the states doesn't exist in the automata an error message is shown
        void add_transition(const std::string& symbol, const std::string& from, const std::string& to);
        void add_transition(int symbol, int from, int to);
        void create_transition(const int symbol, const int from, const int to);

        void add_init_state(int init_state);
        void add_init_state_force(const std::string& init_state);
    // Method add_accept_state - adds accept state to the automata
//                         - if the state doesn't exist in the automata an error message is shown
        void add_accept_state(int fin_state);
        void add_accept_state(const std::shared_ptr <auto_state>& fin_state);
        void add_accept_state_force(const std::string& fin_state);


        int get_alphabet() const;
        int get_init();
        unsigned long get_state_number();

        bool is_final(const int& state);

        int get_next_state(const int& symbol, const int& state);

    // Method remove_eps_transitions - find epsilon closures for all states and using them replaces all
//                                 epsilon transitions in the automata
        void remove_eps_transitions();

        std::string combine_states(ptr_state_vector states);
        std::string combine_states(std::set<std::shared_ptr<auto_state>> states);
        void determine_state(automata* old, std::vector <ptr_state_vector>& set_queue,
                              const ptr_state_vector& current_set, int current_state);
    // Method determine - creates and returns a new deterministic automata representing the same language
        std::shared_ptr <automata> determine();
        std::shared_ptr <automata> reverse();

        void init_power_set(std::vector <std::shared_ptr<power_element>>& pow_set);

        void split_power_state(std::vector <std::shared_ptr <power_element>>& previous,
                                      std::vector <std::shared_ptr <power_element>>& next);

        void reverse_accept_states();
        std::shared_ptr <automata> complement();

        void minimal_sim(std::vector<std::vector<bool>>& omega_matrix);
        void create_simulate_matrix(std::vector<std::vector<bool>>& omega_matrix);
        void init_card_tables(std::vector <std::vector <std::vector<int>>>& card_tables);
        void init_omega_matrix(std::vector<std::vector<bool>>&, std::vector <std::pair <int, int>>&);

        bool same_alphabets(const std::shared_ptr <automata>& second);

        void remove_rezidual_state(const std::string& state_value, std::vector <int>& covering);
        void create_rezidual_state(std::set<std::shared_ptr<auto_state>>& base, const std::shared_ptr <automata>& rezid,
                                         int base_value, std::vector <std::set<std::shared_ptr<auto_state>>>& all_states);
        std::shared_ptr <automata> rezidual();
    // Method print - prints the automata
        void print();
};

#endif //BAKALARKA_AUTOMATA_H
