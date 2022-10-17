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

std::string combine_states(state_set);

bool has_intersect(const state_set&, const state_set&);

void determine_state(const std::shared_ptr <automata>&, std::vector <state_set>*,
                        const state_set&, const std::string&);

std::shared_ptr <automata> determine_nfa(const std::shared_ptr <automata>&);
std::shared_ptr <automata> det_n_min(const std::shared_ptr <automata>& nfa);

bool language_equal(const std::shared_ptr <automata>&, const std::shared_ptr <automata>&);


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

        void print();

        bool has_eps();

        std::string get_value();
        state_set get_trans_row(const std::string& symbol);
        std::unordered_map <std::string, state_set> get_trans();

        void get_pow_trans(std::list <power_element>& previous,
                           std::unordered_map <std::string, state_set>& new_trans,
                           std::unordered_map <std::shared_ptr <auto_state>, state_set>& helper);

        void add_transition(const std::string& symbol, const std::shared_ptr <auto_state>& trans_to);
        void add_transition_row(const std::string& symbol,
                                const state_set& new_trans);

        state_set get_eps_transitions();
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

        state_set recurse_find_all_eps(const state_set& search_group);

    public:
        automata(const std::set <std::string>& states, const std::set <std::string>& alphabet,
                 const std::vector <std::array<std::string, 3>>& trans,
                 const std::set <std::string>& init_states, const std::set <std::string>& fin_states);

        automata (const std::string& state, const std::set <std::string>& alphabet);
        automata ();

        void min_power(const std::list <power_element>& power_set);
        // Copy constructor TODO je to zle
        std::shared_ptr <automata> copy();

        bool add_state(const std::string& state_value);
        void add_alphabet(const std::string& new_symbol);
        void add_transition(const std::string& symbol, const std::string& from, const std::string& to);

        void add_init_state(const std::string& init_state);
        void add_init_state_force(const std::string& init_state);
        void add_accept_state(const std::string& fin_state);
        void add_accept_state(const std::shared_ptr <auto_state>& fin_state);
        void add_accept_state_force(const std::string& fin_state);


        std::set <std::string> get_alphabet();
        std::string get_init();
        unsigned long get_state_number();

        bool is_final(const std::string& state);

        std::string get_next_state(const std::string& symbol, const std::string& state);

        void remove_eps_transitions();

        std::shared_ptr <automata> determine();
        std::shared_ptr <automata> reverse();

        void init_power_set(std::list <power_element>& pow_set){
        power_element final = (power_element)this->accept_states;
        power_element non_final = power_element();
        for (const auto& element: this->state_table){
            if (this->accept_states.find(element.second) == this->accept_states.end()){
                non_final.add_state(element.second);
            }
        }
        pow_set.push_back(final);
        pow_set.push_back(non_final);
        }

        static void split_power_state(std::list <power_element>& previous, std::list <power_element>& next);

        void reverse_accept_states();
        std::shared_ptr <automata> complement();


        void print();
};

#endif //BAKALARKA_AUTOMATA_H
