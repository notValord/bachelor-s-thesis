//
// Created by vmvev on 3/19/2023.
//

#ifndef BAKALARKA_AUTOMATA_BASE_H
#define BAKALARKA_AUTOMATA_BASE_H

#include <vector>
#include <set>
#include <memory>
#include <fstream>
#include <filesystem>

#include "auto_dictionary.h"

#define DEAD "D"
#define EPS "eps"

class automata;
class auto_state;

using ptr_state_vector = std::vector<std::shared_ptr<auto_state>>;


bool has_intersect(ptr_state_vector& first, ptr_state_vector& second);
std::shared_ptr <auto_state> get_smallest_state(const ptr_state_vector& states);

class auto_state {
protected:
    int index;
    std::vector<std::shared_ptr<ptr_state_vector>> transitions;

public:
    explicit auto_state(int value){
        this->index = value;
    }

    void print();
    bool has_trans(int value);

    [[nodiscard]] int get_value() const;
    void set_value(int new_val);

    std::shared_ptr<ptr_state_vector> get_trans_row(int symbol);
    std::vector <std::shared_ptr<ptr_state_vector>>& get_trans();

    void add_transition(int symbol, const std::shared_ptr <auto_state>& trans_to);
    void add_transition_row(int symbol, const ptr_state_vector& new_row);

    std::shared_ptr <auto_state> get_next(int symbol);

    void reverse_trans(const std::shared_ptr <automata>& reverse);
    void clear_trans();
    void copy_state(const std::shared_ptr<automata>& copy_auto);

    //det_auto
    void replace_eps_trans(const ptr_state_vector& replace, int eps_index);

    //min_det
    void get_pow_trans(std::vector<ptr_state_vector>& new_trans, std::vector<ptr_state_vector>& helper);
    void set_power_state(std::vector <ptr_state_vector>& new_trans);

    //simul
    void check_simul_trans(const std::vector <ptr_state_vector>& help_table);
    bool not_under_simulate(const std::shared_ptr <auto_state>& second);

    //rezidual
    int get_trans_card(const int symbol);
    void change_coverable(ptr_state_vector& covering_ptr, const std::shared_ptr<auto_state>& change);
};

class automata{
protected:
    int alphabet = 0;
    std::vector <int> init_states;
    std::vector <int> accept_states;
    auto_dictionary dict;

    std::vector <std::shared_ptr <auto_state>> state_table;
public:
    automata(const std::set <std::string>& states, const std::set <std::string>& alphabet,
             const std::vector <std::array<std::string, 3>>& trans,
             const std::set <std::string>& init_states, const std::set <std::string>& fin_states);

    // Constructor of the automata - takes only the alphabet and one state
    automata (const std::string& state, const std::vector <std::string>& alphabet);
    automata (int alphabet, auto_dictionary& dict);
    automata ();
    ~automata();

    bool add_state(const std::string& state_value);
    void add_alphabet(const std::string& new_symbol);

    void add_transition(const std::string& symbol, const std::string& from, const std::string& to);
    void add_transition(int symbol, int from, int to);
    void create_transition(const int symbol, const int from, const int to);

    void add_init_state(int init_state);
    void add_init_state_force(const std::string& init_state);

    void add_accept_state(int fin_state);
    void add_accept_state_force(const std::string& fin_state);

    int get_alphabet() const;

    int get_init();
    std::shared_ptr <auto_state> get_state(int index);
    std::vector <int>& get_init_vec();
    const std::vector <int>& get_acc_vec();
    const auto_dictionary& get_dict();
    const std::vector <std::shared_ptr <auto_state>>& get_state_table();

    unsigned long get_state_number();

    bool is_final(const int& state);
    bool is_product_final(const std::set <int>& check);
    bool same_alphabets(const std::shared_ptr <automata>& second);

    int get_next_state(const int& symbol, const int& state);
    std::vector <int> get_next_vec(const int& symbol, const int& state);

    void print();
    void save_to_file(const std::string& filename, const std::string& type);
};

#endif //BAKALARKA_AUTOMATA_BASE_H
