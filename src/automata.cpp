/*
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 3.10 2022
* Subject: Bachelor's thesis
*/


/*TODO pouzivat viac std::move kde sa da, pozret ako sa prekladaju veci na https://godbolt.org/
 * */

#include <iostream>
#include "automata.h"

/// Delimeter for combining states
#define DELIM "."


bool has_intersect(ptr_state_vector& first, ptr_state_vector& second){
    std::sort(first.begin(), first.end());
    std::sort(second.begin(), second.end());

    std::vector <std::shared_ptr <auto_state>> tmp(first.size());
    std::vector <std::shared_ptr <auto_state>>::iterator last;

    last = std::set_intersection(first.begin(), first.end(), second.begin(), second.end(), tmp.begin());

    if (last - tmp.begin()){
        return true;
    }
    return false;
}

std::shared_ptr <automata> brzozowski(const std::shared_ptr <automata>& nfa){
    auto rev = nfa->reverse();
    auto dfa = determine_nfa(rev);
    std::cout << dfa->get_state_number() << "; ";
    rev = dfa->reverse();
    dfa = determine_nfa(rev);
    return dfa;
}


std::shared_ptr <automata> det_n_min(const std::shared_ptr <automata>& nfa){
    auto dfa = determine_nfa(nfa);
    //std::cout << "After det " << dfa->get_state_number() << std::endl;
    minimal_dfa(dfa);
    return dfa;
}

std::shared_ptr <automata> determine_nfa(const std::shared_ptr <automata>& nfa){
    nfa->remove_eps_transitions();
    auto dfa = nfa->determine();
    return dfa;
}

bool is_eq(std::vector <std::shared_ptr <power_element>>& first,
           std::vector <std::shared_ptr <power_element>>& second){
    if (first.size() != second.size()){
        return false;
    }

    for(int i = 0; i < first.size(); i++){
        if (*(first[i]) != *(second[i])){
            return false;
        }
    }
    return true;
}


void minimal_dfa(const std::shared_ptr <automata>& dnf){
    std::vector <std::shared_ptr <power_element>> pow_vec_old;
    std::vector <std::shared_ptr <power_element>> pow_vec_new;
    dnf->init_power_set(pow_vec_new);

    do {
        pow_vec_old = std::move(pow_vec_new);
        pow_vec_new.clear();

        dnf->split_power_state(pow_vec_old, pow_vec_new);
    } while (not is_eq(pow_vec_new, pow_vec_old));

    dnf->min_power(pow_vec_new);            // create new automata from the power states
}

void simulate_min(const std::shared_ptr <automata>& nfa){
    std::vector<std::vector <bool>> simulate_matrix;
    nfa->create_simulate_matrix(simulate_matrix);
    nfa->minimal_sim(simulate_matrix);
}

void symetric_fragment(std::vector<std::vector<bool>>& omega_matrix){
    for (int i = 0; i < omega_matrix.size(); i++){
        if (omega_matrix[i][i] == 0){
            std::cerr << "Omega matrix is not reflexive" << std::endl;
            exit(-1);
        }

        for (int j = 0; j < omega_matrix[i].size(); j++){
            if (not omega_matrix[i][j]){
                omega_matrix[j][i] = false;
            }
        }
    }
}

std::shared_ptr<automata> rezidual_auto(const std::shared_ptr <automata>& nfa){
    auto reverse_nfa = nfa->reverse();
    auto rev_dfa = determine_nfa(reverse_nfa);
    std::cout << "After rev " << rev_dfa->get_state_number() << std::endl;
    auto rez_prepare = rev_dfa->reverse();
    return rez_prepare->rezidual();
}


bool language_intersect(const std::shared_ptr <automata>& first, const std::shared_ptr <automata>& second){
    //std::vector <int> word_stack;
    std::vector <std::pair <int, int>> state_stack;
    std::set <std::pair <int, int>> gone_through;

    if (not first->same_alphabets(second)){
        return true;
    }

    auto start = std::make_pair(first->get_init(), second->get_init());
    state_stack.push_back(start);
    //word_stack.emplace_back("");
    gone_through.insert(start);

    while(not state_stack.empty()){
        //std::string current_word = word_stack.back();
        std::pair <int, int> current_pair = state_stack.back();
        //word_stack.pop_back();
        state_stack.pop_back();

        if (first->is_final(current_pair.first) and second->is_final(current_pair.second)){
            std::cout << "FOUND WORD "<< std::endl;
            return true;
        }

        for (int i = 0; i < first->get_alphabet(); i++){
            auto new_pair = std::make_pair(first->get_next_state(i, current_pair.first),
                                              second->get_next_state(i, current_pair.second));

            if (gone_through.find(new_pair) == gone_through.end()){
                state_stack.push_back(new_pair);
                //word_stack.push_back(current_word + symbol);
                gone_through.insert(new_pair);
            }
        }
    }
    return false;
}

bool language_equal(const std::shared_ptr <automata>& first, const std::shared_ptr <automata>& second){
    auto dfa_first = determine_nfa(first);
    auto dfa_second = determine_nfa(second);
    auto first_comple = dfa_first->complement();
    auto second_comple = dfa_second->complement();

    if (language_intersect(dfa_first, second_comple) or
        language_intersect(first_comple, dfa_second)){
        return false;
    }
    return true;
}



void insert_pow_set(const std::shared_ptr <power_element>& adding,
                    std::vector <std::shared_ptr<power_element>>& power_set){
    for (auto& element: power_set){
        if (element->get_trans() == adding->get_trans()){         // if exists a closure with the same transitions
            element->add_state(*(adding->get_set().begin()));      // add new state to it, single state
            return;
        }
    }
    power_set.push_back(adding);                // create new closure
}

void init_power_hash(std::vector <ptr_state_vector>& help_table,
                     const std::vector <std::shared_ptr<power_element>>& previous){
    for (const auto& closure: previous){
        auto old_set = closure->get_set();
        for (const auto& element: old_set){
            help_table[element->get_value()] = old_set;
        }
    }
}

// TODO teoreticky sa to da vylepsit ako usporiadanie vectoru a vratenie prveho prvku
std::shared_ptr <auto_state> get_smallest_state(const ptr_state_vector& states){
    int value = (*states.begin())->get_value();
    std::shared_ptr <auto_state> found = nullptr;
    for (const auto& element: states){
        int tmp = element->get_value();
        if (tmp <= value){
            value = tmp;
            found = element;
        }
    }
    return found;
}


power_element::power_element(const std::shared_ptr <auto_state>& state, std::vector<ptr_state_vector>& trans){
    this->same_set.push_back(state);
    this->transition = std::move(trans);
}

power_element::power_element(ptr_state_vector set){
    this->same_set = std::move(set);
}

power_element::power_element(){
    ;
}

ptr_state_vector& power_element::get_set(){
    return this->same_set;
}

std::vector<ptr_state_vector>& power_element::get_trans(){
    return this->transition;
}

void power_element::add_state(const std::shared_ptr <auto_state>& state){
    this->same_set.push_back(state);
}

void power_element::set_trans(const std::vector<ptr_state_vector>& new_trans){
    this->transition = new_trans;
}



void auto_state::print(){
    std::cout << "State index" << std::endl;
    std::cout << this->index << std::endl;

    std::cout << "Transitions:" << std::endl;

    for (int i = 0; i < this->transitions.size(); i++){
        std::cout << i << " -> ";
        if (this->transitions[i] != nullptr){
            for (int j = 0; j < this->transitions[i]->size(); j++){
                std:: cout << (*this->transitions[i])[j]->index << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}


bool auto_state::has_trans(int value){
    if (value >= this->transitions.size()){
        return false;
    }
    else{
        if (this->transitions[value] == nullptr){
            return false;
        }
        return true;
    }
}


int auto_state::get_value() const{
    return this->index;
}

void auto_state::set_value(int new_val){
    this->index = new_val;
}


std::shared_ptr<ptr_state_vector> auto_state::get_trans_row(int symbol){
    if (this->transitions.size() > symbol){
        return this->transitions[symbol];
    }
    else{
        return nullptr;
    }
}

std::vector <std::shared_ptr<ptr_state_vector>>& auto_state::get_trans(){
    return this->transitions;
}

void auto_state::get_pow_trans(std::vector<ptr_state_vector>& new_trans, std::vector<ptr_state_vector>& helper){
    for (auto& transition : this->transitions){            // has transition for every symbol
        auto search = *(transition->begin());               // deterministic automata, trans to single state

        if (search->get_value() >= helper.size()){
            std::cerr << "Indexing out of bounds\n" << std::endl;
            exit(-1);
        }
        new_trans.push_back(helper[search->get_value()]);       // pushing in order of the symbols
    }
}


void auto_state::add_transition(int symbol, const std::shared_ptr <auto_state>& trans_to){
    if (symbol >= this->transitions.size()){
        this->transitions.resize(symbol+1, nullptr);
        std::shared_ptr<ptr_state_vector> new_trans = std::make_shared<ptr_state_vector>();
        new_trans->push_back(trans_to);

        this->transitions[symbol] = new_trans;
    }
    else if (this->transitions[symbol] == nullptr){
        std::shared_ptr<ptr_state_vector> new_trans = std::make_shared<ptr_state_vector>();
        new_trans->push_back(trans_to);

        this->transitions[symbol] = new_trans;
    }
    else{
        if (std::count(this->transitions[symbol]->begin(), this->transitions[symbol]->end(), trans_to)){
            std::cerr << "Trying to add a duplicate transition in add_transition" << std::endl;
            return;
        }

        this->transitions[symbol]->push_back(trans_to);
    }
}


void auto_state::add_transition_row(int symbol, const ptr_state_vector& new_row){
    if (symbol >= this->transitions.size()){
        this->transitions.resize(symbol+1, nullptr);
        std::shared_ptr<ptr_state_vector> new_trans = std::make_shared<ptr_state_vector>();
        *new_trans = new_row;

        this->transitions[symbol] = new_trans;
    }
    else if (this->transitions[symbol] == nullptr){
        std::shared_ptr<ptr_state_vector> new_trans = std::make_shared<ptr_state_vector>();
        *new_trans = new_row;

        this->transitions[symbol] = new_trans;
    }
    else {      //TODO zmenit na pridavanie do setu a ten dat do vectoru
        for (const auto& elem: new_row){
            if (std::count(this->transitions[symbol]->begin(), this->transitions[symbol]->end(), elem) == 0){
                this->transitions[symbol]->push_back(elem);
            }
        }
    }
}

void auto_state::replace_eps_trans(const ptr_state_vector& replace, int eps_index){
    if (eps_index == this->transitions.size()-1){
        this->transitions.pop_back();
    }
    else{
        this->transitions[eps_index] = this->transitions.back();
        this->transitions.pop_back();
    }

    for (const auto& state: replace){
        for (int i = 0; i < state->transitions.size(); i++){
            if (i == eps_index or state->transitions[i] == nullptr){
                continue;
            }
            this->add_transition_row(i, *state->transitions[i]);
        }
    }
}

std::shared_ptr <auto_state> auto_state::get_next(const int symbol){
    if (symbol < this->transitions.size()){
        if (this->transitions[symbol] != nullptr){
            return *this->transitions[symbol]->begin();   // gets the first element
        }
    }
    return nullptr;
}

void auto_state::reverse_trans(const std::shared_ptr <automata>& reverse){
    for (int i = 0; i < this->transitions.size(); i++){
        if (this->transitions[i] == nullptr) {
            continue;
        }

        for (const auto& state: *this->transitions[i]){
            reverse->create_transition(i, state->index, this->index);
        }
    }
}

void auto_state::clear_trans(){
    for(auto & transition : this->transitions){
        if (transition != nullptr){
            transition->clear();
        }
    }
    this->transitions.clear();
}

void auto_state::set_power_state(std::vector <ptr_state_vector>& new_trans){
    for (int i = 0; i < new_trans.size(); i++){
        auto smallest_state = get_smallest_state(new_trans[i]);
        this->transitions[i]->clear();
        this->transitions[i]->push_back(smallest_state);
    }
}

void auto_state::check_simul_trans(const std::vector <ptr_state_vector>& help_table){
    for (const auto& row: this->transitions){
        if (row == nullptr){
            continue;
        }
        ptr_state_vector new_trans;
        for (const auto& state: *row){
            if (help_table.size() < state->get_value()){
                std::cerr << "Nieco je blbo\n";
                exit(-1);
            }
            if (help_table[state->get_value()].size() == 0){
                std::cerr << "Heh?\n";
                exit(-1);
            }
            new_trans.push_back(*help_table[state->get_value()].begin());
        }
        row->clear();
        row->insert(row->begin(), new_trans.begin(), new_trans.end());
    }
}

int auto_state::get_trans_card(const int symbol){
    if (this->transitions.size() <= symbol){
        return 0;
    }
    if (this->transitions[symbol] == nullptr){
        return 0;
    }
    else{
        return static_cast <int> (this->transitions[symbol]->size());
    }
}


bool auto_state::not_under_simulate(const std::shared_ptr <auto_state>& second){
    if (this->transitions.size() < second->transitions.size()){
        return true;
    }

    for (int i = 0; i < this->transitions.size(); i++){
        if (i >= second->transitions.size()){
            break;
        }

        if (this->transitions[i] == nullptr && second->transitions[i] != nullptr){
            return true;
        }
    }
    return false;
}


void auto_state::copy_state(const std::shared_ptr<automata>& copy_auto){
    for (int i = 0; i < this->transitions.size(); i++){
        if (this->transitions[i] == nullptr) {
            continue;
        }

        for (const auto& state: *this->transitions[i]){
            copy_auto->create_transition(i, this->index, state->index);
        }
    }
}

void auto_state::change_coverable(ptr_state_vector& covering_ptr, const std::shared_ptr<auto_state>& change){
    for (int i = 0; i < this->transitions.size(); i++){
        if (transitions[i] == nullptr){
            continue;
        }
        auto search = std::find((*transitions[i]).begin(), (*transitions[i]).end(), change);
        if (search == transitions[i]->end()){
            continue;
        }
        else{
            (*transitions[i])[search-transitions[i]->begin()] = (*transitions[i])[transitions[i]->size()-1];
            transitions[i]->pop_back();

            this->add_transition_row(i, covering_ptr);
        }
    }
}


void automata::recurse_find_all_eps(const ptr_state_vector& search_group, int eps_index, ptr_state_vector& ret){
    for (const auto& state: search_group){
        auto new_add = state->get_trans_row(eps_index);
        if (new_add == nullptr){
            return;
        }

        bool end = true;                // no new addition
        ptr_state_vector next_search;
        for (const auto& new_state: (*new_add)){
            if (std::count(ret.begin(), ret.end(), new_state) == 0){
                ret.push_back(new_state);
                next_search.push_back(new_state);
                end = false;
            }
        }

        if (not end){
            this->recurse_find_all_eps(next_search, eps_index, ret);
        }
    }
}




automata::automata(const std::set <std::string>& states, const std::set <std::string>& alphabet,
         const std::vector <std::array<std::string, 3>>& trans,
         const std::set <std::string>& init_states, const std::set <std::string>& fin_states)
{
    this->state_table.resize(states.size());
    for (const std::string& state_name: states){
        auto index = this->dict.add_state(state_name);
        std::shared_ptr <auto_state> new_state(std::make_shared <auto_state> (index));
        this->state_table[index] = new_state;
    }

    for (const std::string& symbol: alphabet){
        this->dict.add_alpha(symbol, this->alphabet);
        this->alphabet++;
    }

    for (const std::array <std::string, 3>& triple: trans){
        if (triple.size() < 3){
            std::cerr << "Wrong notation of transitions in automata constructor" << std::endl;
            continue;
        }

        auto index_from = this->dict.get_state_index(triple[0]);
        if (index_from >= this->state_table.size()){
            std::cerr << "Nonexistent state for transition in automata constructor" << std::endl;
            continue;
        }

        auto index_to = this->dict.get_state_index(triple[2]);
        if (index_to >= this->state_table.size()){
            std::cerr << "Nonexistent state for transition in automata constructor" << std::endl;
            continue;
        }

        auto index_symb = this->dict.get_alpha_index(triple[1]);
        if (index_symb >= this->alphabet){
            std::cerr << "Nonexistent symbol for transition in automata constructor" << std::endl;
            continue;
        }

        this->state_table[index_from]->add_transition(static_cast<int>(index_symb), this->state_table[index_to]);
    }

    for (const std::string& in_state: init_states){
        auto index = this->dict.get_state_index(in_state);
        this->init_states.push_back(static_cast<int>(index));
    }

    for (const std::string& fin_state: fin_states){
        auto index = this->dict.get_state_index(fin_state);
        this->accept_states.push_back(static_cast<int>(index));
    }
}


automata::automata (const std::string& state, const std::vector <std::string>& alphabet){
    auto index = this->dict.add_state(state);
    std::shared_ptr <auto_state> new_state(std::make_shared <auto_state> (index));
    this->state_table.push_back(new_state);

    for (const std::string& symbol: alphabet){
        this->dict.add_alpha(symbol, this->alphabet);
        this->alphabet++;
    }
}

automata::automata (int alphabet, auto_dictionary& dict){
    this->alphabet = alphabet;
    this->dict = dict;
}

automata::automata() {
    ;
}

automata::~automata() {
    for(const auto& element: this->state_table){
        element->clear_trans();
    }

    this->state_table.clear();
    this->init_states.clear();
    this->accept_states.clear();
}


std::shared_ptr <automata> automata::copy(){
    std::shared_ptr <automata> copy = std::make_shared <automata> (this->alphabet, this->dict); //huh

    copy->state_table.resize(this->state_table.size(), nullptr);
    for (int i = 0; i < this->state_table.size(); i++){
        if (copy->state_table[i] == nullptr){
            std::shared_ptr <auto_state> new_state(std::make_shared <auto_state> (i));
            copy->state_table[i] = new_state;
        }
        this->state_table[i]->copy_state(copy);
    }

    for (const auto& state: this->init_states){
        copy->add_init_state(state);
    }

    for (const auto& state: this->accept_states){
        copy->add_accept_state(state);
    }
    return copy;
}

void automata::min_power(const std::vector <std::shared_ptr<power_element>>& power_set){
    for (const auto& element: power_set){
        auto states_set = element->get_set();
        auto trans = element->get_trans();
        std::string new_value = combine_states(states_set);

        std::shared_ptr <auto_state> stays = get_smallest_state(states_set);        // change value in dict
        this->dict.change_state_name(stays->get_value(), new_value);

        stays->set_power_state(trans);

        for (const auto& state: states_set){
            if (state != stays){
                this->dict.remove_state(state->get_value());        // remove state from dict and automata
                state->clear_trans();
                this->state_table[state->get_value()] = nullptr;


                auto search = std::find(this->accept_states.begin(), this->accept_states.end(), state->get_value());
                if (search != this->accept_states.end()){
                    this->accept_states.erase(search);
                }

                search = std::find(this->init_states.begin(), this->init_states.end(), state->get_value());
                if (search != this->init_states.end()) {
                    this->accept_states.erase(search);          //could be expensice
                }
            }
        }
    }

    unsigned int old_id, new_id;
    while (this->dict.smooth_vector_state(old_id, new_id)){
        this->state_table[new_id] = this->state_table[old_id];
        this->state_table[new_id]->set_value(static_cast<int> (new_id));

        auto search = std::find(this->accept_states.begin(), this->accept_states.end(), old_id);
        if (search != this->accept_states.end()){
            *search = static_cast<int> (new_id);
        }
        search = std::find(this->init_states.begin(), this->init_states.end(), old_id);
        if (search != this->init_states.end()) {
            *search = static_cast <int> (new_id);          //could be expensice
        }
    }

    this->state_table.resize(this->dict.get_state_size());
}

bool automata::add_state(const std::string& state_value){
    if (this->dict.state_exists(state_value)){
        return false;
    }

    auto index = this->dict.add_state(state_value);
    std::shared_ptr <auto_state> new_state(std::make_shared <auto_state> (index));

    if (index >= this->state_table.size()){
        this->state_table.resize(index+1);
    }
    this->state_table[index] = new_state;


    if (state_value == DEAD){
        for (int i = 0 ; i < this->alphabet; i++){
            new_state->add_transition(i, new_state);
        }
        return false;
    }
    else{
        return true;
    }
}


void automata::add_alphabet(const std::string& new_symbol){
    if (not this->dict.alpha_exists(new_symbol)){
        this->dict.add_alpha(new_symbol, this->alphabet);
        this->alphabet++;
    }
}


void automata::add_transition(const std::string& symbol, const std::string& from, const std::string& to){
    auto index_from = this->dict.get_state_index(from);
    if (index_from >= this->state_table.size()){
        std::cerr << "Nonexistent state for transition in automata constructor" << std::endl;
        return;
    }

    auto index_to = this->dict.get_state_index(to);
    if (index_to >= this->state_table.size()){
        std::cerr << "Nonexistent state for transition in automata constructor" << std::endl;
        return;
    }

    auto index_symb = this->dict.get_alpha_index(symbol);
    if (index_symb >= this->alphabet){
        std::cerr << "Nonexistent symbol for transition in automata constructor" << std::endl;
        return;
    }

    this->state_table[index_from]->add_transition(static_cast<int>(index_symb), this->state_table[index_to]);
}

void automata::add_transition(int symbol, int from, int to){
    if (to >= this->state_table.size() or this->state_table[to] == nullptr){
        std::cerr << "Nonexistent symbol for transition in automata constructor" << std::endl;
        return;
    }

    if (from >= this->state_table.size() or this->state_table[from] == nullptr){
        std::shared_ptr <auto_state> new_state(std::make_shared <auto_state> (from));
        this->state_table[from] = new_state;
    }

    if (symbol >= this->alphabet){
        std::cerr << "Nonexistent symbol for transition in automata constructor" << std::endl;
        return;
    }

    this->state_table[from]->add_transition(symbol, this->state_table[to]);
}

void automata::create_transition(const int symbol, const int from, const int to){
    if (this->state_table[to] == nullptr){
        std::shared_ptr <auto_state> new_state(std::make_shared <auto_state> (to));
        this->state_table[to] = new_state;
    }

    if (this->state_table[from] == nullptr){
        std::shared_ptr <auto_state> new_state(std::make_shared <auto_state> (from));
        this->state_table[from] = new_state;
    }

    this->state_table[from]->add_transition(symbol, this->state_table[to]);
}

void automata::add_init_state(int init_state){
    if (not this->dict.state_exists(init_state)){
        std::cerr << "Trying to add nonexistent state for init\n";
        exit(-1);
    }

    if (std::count(this->init_states.begin(), this->init_states.end(), init_state) == 0){
        this->init_states.push_back(init_state);
    }
    else{
        std::cerr << "Trying to add duplicate state in add_init_state\n";
    }
}

void automata::add_init_state_force(const std::string& init_state){
    this->add_state(init_state);
    auto index = this->dict.get_state_index(init_state);
    this->init_states.push_back(static_cast<int>(index));
}


void automata::add_accept_state(int fin_state){
    if (not this->dict.state_exists(fin_state)){
        std::cerr << "Indexing accept state out of range\n";
        exit(-1);
    }
    if (std::count(this->accept_states.begin(), this->accept_states.end(), fin_state) == 0){
        this->accept_states.push_back(fin_state);
    }
    else{
        std::cerr << "Trying to add duplicate state in add_accept_state\n";
    }
}

void automata::add_accept_state_force(const std::string& fin_state){
    this->add_state(fin_state);
    auto index = this->dict.get_state_index(fin_state);
    this->accept_states.push_back(static_cast<int>(index));
}


int automata::get_alphabet() const{
    return this->alphabet;
}

int automata::get_init(){
    return (*this->init_states.begin());
}

unsigned long automata::get_state_number(){
    return this->state_table.size();
}

bool automata::is_final(const int& index){
    if (std::count(this->accept_states.begin(), this->accept_states.end(), index)){
        return true;
    }
    return false;
}


int automata::get_next_state(const int& symbol, const int& state){
    if (state < this->state_table.size()){
        auto next = this->state_table[state]->get_next(symbol);

        if (next == nullptr){
            //std::cerr << "There isn't any next state in get_next_state" << std::endl;
            return -1;
        }
        return next->get_value();
    }
    else{
        std::cerr << "Searching nonexistent state in get_next_state" << std::endl;
        exit(-1);
    }
}


void automata::remove_eps_transitions(){
    if (not this->dict.alpha_exists(EPS)){
        return;
    }
    int eps_index = static_cast<int>(this->dict.get_alpha_index(EPS));

    ptr_state_vector accept_vect;
    for (auto index: this->accept_states){
        accept_vect.push_back(this->state_table[index]);
    }

    for (const auto& state: this->state_table){
        if (not state->has_trans(eps_index)){
            continue;
        }

        ptr_state_vector eps_closure;
        ptr_state_vector tmp_set = {state};
        this->recurse_find_all_eps(tmp_set, eps_index, eps_closure);

        if (has_intersect(eps_closure, accept_vect)){
            this->add_accept_state(state->get_value());
        }

        state->replace_eps_trans(eps_closure, eps_index);

    }

    this->dict.remove_alpha(EPS);
    this->alphabet--;
}

std::string automata::combine_states(ptr_state_vector states){
    if (states.size() == 1){
        return this->dict.get_state_name((*states.begin())->get_value());
    }

    std::sort(states.begin(), states.end());
    auto first_id = states.back();
    std::string first = this->dict.get_state_name(first_id->get_value());
    states.pop_back();

    while(not states.empty()){
        auto second_id = states.back();
        states.pop_back();

        first += (DELIM + this->dict.get_state_name(second_id->get_value()));
    }

    return first;
}

std::string automata::combine_states(std::set<std::shared_ptr<auto_state>> states){
    if (states.size() == 1){
        return this->dict.get_state_name((*states.begin())->get_value());
    }

    auto first_id = *states.rbegin();
    std::string first = this->dict.get_state_name(first_id->get_value());
    states.erase(std::prev(states.end()));

    while(not states.empty()){
        auto second_id = *states.rbegin();
        states.erase(std::prev(states.end()));

        first += (DELIM + this->dict.get_state_name(second_id->get_value()));
    }

    return first;
}


void automata::determine_state(automata* old, std::vector <ptr_state_vector>& set_queue,
                               const ptr_state_vector& current_set, int current_state){
    ptr_state_vector new_state_vect;
    std::set <std::shared_ptr<auto_state>> tmp_set;
    std::string new_value;

    for (int i = 0; i < old->alphabet; i++){
        new_state_vect.clear();
        tmp_set.clear();

        // gets the new  set of states
        for (const auto& state: current_set){
            auto search = state->get_trans_row(i);
            if (search == nullptr){
                continue;
            }

            /*for (auto elem: *search){
                if (not old->dict.state_exists(elem->get_value())){
                    std::cout << state->get_value() << std::endl;
                    std::cout << "AHHHH " << elem->get_value() << std::endl;
                }
            }*/

            tmp_set.insert(search->begin(), search->end());         // using set for removing duplicates
        }
        new_state_vect.assign(tmp_set.begin(), tmp_set.end());

        if (new_state_vect.empty()){
            new_value = DEAD;
        }
        else{
            new_value = old->combine_states(new_state_vect);
        }

        if (this->add_state(new_value)){
            set_queue.push_back(new_state_vect);
        }

        this->add_transition(i, current_state, static_cast<int> (this->dict.get_state_index(new_value)));
    }
}

std::shared_ptr <automata> automata::determine(){
    //for (int i= 0; i < this->alphabet; i++){
     /*   auto tmp = this->state_table[137]->get_trans_row(7);
        if (tmp != nullptr){
            for (auto elem: *tmp){
                std::cout << "To " << elem->get_value() << std::endl;
                if (not this->dict.state_exists(elem->get_value())){
                    std::cout << "WTF " << std::endl;
                }
            }
        }*/
    //}

    std::vector <ptr_state_vector> set_queue;

    ptr_state_vector init_vect;             // get initial states to combine them into one
    for (auto index: this->init_states){
        init_vect.push_back(this->state_table[index]);
    }
    set_queue.push_back(init_vect);

    std::string new_value = this->combine_states(init_vect);        // new value for the state
    init_vect.clear();

    for (auto index: this->accept_states){                      // get accept states for checking
        init_vect.push_back(this->state_table[index]);
    }

    std::vector <std::string> alpha(this->alphabet);        // get alpha vector for new automata
    for (int i = 0; i < this->alphabet; i++){
        alpha[i] = this->dict.get_alpha_name(i);
    }

    std::shared_ptr <automata> dfa(std::make_shared <automata> (new_value, alpha));
    dfa->add_init_state(static_cast <int> (dfa->dict.get_state_index(new_value)));

    while (not set_queue.empty()){
        auto state_set = set_queue.back();      //get last new state set
        set_queue.pop_back();

        new_value = this->combine_states(state_set);
        int new_index = static_cast <int> (dfa->dict.get_state_index(new_value));

        if (has_intersect(init_vect, state_set)){           // check for accept states
            dfa->add_accept_state(new_index);
        }

        dfa->determine_state(this, set_queue, state_set, new_index);
    }

    return dfa;
}

std::shared_ptr <automata> automata::reverse(){
    std::shared_ptr <automata> reverse = std::make_shared <automata> (this->alphabet, this->dict); //huh

    reverse->state_table.resize(this->state_table.size(), nullptr);
    for (int i = 0; i < this->state_table.size(); i++){
        if (reverse->state_table[i] == nullptr){
            std::shared_ptr <auto_state> new_state(std::make_shared <auto_state> (i));
            reverse->state_table[i] = new_state;
        }
        this->state_table[i]->reverse_trans(reverse);
    }

    for (const auto& state: this->init_states){
        reverse->add_accept_state(state);
    }

    for (const auto& state: this->accept_states){
        reverse->add_init_state(state);
    }

    return reverse;
}


void automata::init_power_set(std::vector <std::shared_ptr<power_element>>& pow_set){
    ptr_state_vector accept_vect;
    for (auto index: this->accept_states){
        accept_vect.push_back(this->state_table[index]);
    }

    auto final = std::make_shared<power_element>(accept_vect);
    auto non_final =  std::make_shared<power_element>();
    for (int i = 0; i < this->state_table.size(); i++){
        if (std::find(this->accept_states.begin(), this->accept_states.end(), i) == this->accept_states.end()){
            non_final->add_state(this->state_table[i]);
        }
    }
    pow_set.push_back(final);
    pow_set.push_back(non_final);
}


void automata::split_power_state(std::vector <std::shared_ptr <power_element>>& previous,
                                 std::vector <std::shared_ptr <power_element>>& next){
    std::vector <ptr_state_vector> help_table(this->state_table.size());
    init_power_hash(help_table, previous);

    for (const auto& closure: previous){
        auto old_set = closure->get_set();
        if (old_set.size() == 1){                           // no need to divide, it is single element
            std::vector <ptr_state_vector> trans;           // update transition for the single closure
            std::shared_ptr <auto_state> only_element = *old_set.begin();
            only_element->get_pow_trans(trans, help_table);         // get it's transition

            closure->set_trans(trans);          // update
            next.push_back(closure);            // push to the next vector
            continue;
        }

        std::vector <std::shared_ptr<power_element>> new_part;
        for (const auto& element: old_set){
            std::vector<ptr_state_vector> trans;

            element->get_pow_trans(trans, help_table);
            auto shared_add = std::make_shared<power_element>(element, trans);

            insert_pow_set(shared_add, new_part);
        }

        next.insert(next.end(), new_part.begin(), new_part.end());
    }
}

void automata::minimal_sim(std::vector<std::vector<bool>>& omega_matrix){
    std::vector <ptr_state_vector> tmp_states;
    std::vector <ptr_state_vector> help_table(omega_matrix.size());

    ptr_state_vector state_vec;
    for (int i = 0; i < omega_matrix.size(); i++){
        state_vec.clear();
        for (int j = i; j < omega_matrix[i].size(); j++){
            if (omega_matrix[i][j]){
                if (i != j){
                    std::fill(omega_matrix[j].begin(), omega_matrix[j].end(), false);
                }
                state_vec.push_back(this->state_table[j]);
            }
        }
        if (not state_vec.empty()){
            tmp_states.push_back(state_vec);
        }
        for (const auto& element: state_vec){
            help_table[element->get_value()] = state_vec;
        }
    }

    for (auto& closure: tmp_states){
        if (closure.size() == 1){
            (*closure.begin())->check_simul_trans(help_table);
        }
        else{
            //najsi najmensi stav aka prvy stav zkombinuj hodnoty nastav oprav
            std::string new_value = combine_states(closure);

            std::shared_ptr <auto_state> stays = *closure.begin();        // change value in dict
            this->dict.change_state_name(stays->get_value(), new_value);

            stays->check_simul_trans(help_table);

            for (const auto& state: closure) {
                if (state != stays) {
                    this->dict.remove_state(state->get_value());        // remove state from dict and automata
                    state->clear_trans();
                    this->state_table[state->get_value()] = nullptr;


                    auto search = std::find(this->accept_states.begin(), this->accept_states.end(), state->get_value());
                    if (search != this->accept_states.end()) {
                        this->accept_states.erase(search);
                        if (std::count(this->accept_states.begin(), this->accept_states.end(), stays->get_value()) ==
                            0) {
                            this->accept_states.push_back(stays->get_value());
                        }
                    }

                    search = std::find(this->init_states.begin(), this->init_states.end(), state->get_value());
                    if (search != this->init_states.end()) {
                        this->init_states.erase(search);          //could be expensice
                        if (std::count(this->init_states.begin(), this->init_states.end(), stays->get_value()) == 0) {
                            this->init_states.push_back(stays->get_value());
                        }
                    }
                }
            }
        }
    }

    unsigned int old_id, new_id;
    while (this->dict.smooth_vector_state(old_id, new_id)){
        this->state_table[new_id] = this->state_table[old_id];
        this->state_table[new_id]->set_value(static_cast<int> (new_id));

        auto search = std::find(this->accept_states.begin(), this->accept_states.end(), old_id);
        if (search != this->accept_states.end()){
            *search = static_cast<int> (new_id);
        }
        search = std::find(this->init_states.begin(), this->init_states.end(), old_id);
        if (search != this->init_states.end()) {
            *search = static_cast <int> (new_id);          //could be expensice
        }
    }

    this->state_table.resize(this->dict.get_state_size());
}


void automata::reverse_accept_states(){
    std::vector <int> new_accept;
    for (int i = 0; i < this->state_table.size(); i++){
        if (std::find(this->accept_states.begin(), this->accept_states.end(), i) == this->accept_states.end()){
            new_accept.push_back(i);
        }
    }
    this->accept_states.clear();
    this->accept_states = std::move(new_accept);
}


std::shared_ptr <automata> automata::complement(){
    auto comple = this->copy();

    comple->reverse_accept_states();
    return comple;
}


void automata::create_simulate_matrix(std::vector<std::vector<bool>>& ret_matrix){
    unsigned long state_num = this->state_table.size();
    std::vector<std::vector<std::vector<int>>> card_tables(this->alphabet);
    this->init_card_tables(card_tables);

    auto rev_auto = this->reverse();

    std::vector<bool> row(state_num, true);
    std::vector<std::vector<bool>>omega_matrix(state_num, row);
    std::vector <std::pair <int, int>> gone_stack;  // mozno lepsie ako set
    this->init_omega_matrix(omega_matrix, gone_stack);

    while (not gone_stack.empty()){
        auto curr_pair = gone_stack.back();
        gone_stack.pop_back();
        //std::cout << "I= " << curr_pair.first << " J= " << curr_pair.second << std::endl;

        for (int symbol = 0; symbol < this->alphabet; symbol++){
            //std::cout << "Sym= " << symbol << std::endl;

            auto trans_rev_k = rev_auto->state_table[curr_pair.second]->get_trans_row(symbol);
            if (trans_rev_k == nullptr){
                continue;
            }
            std::vector<std::vector<int>>& curr_card_table = card_tables[symbol];

            for (const auto& k: *trans_rev_k){
                int k_index = k->get_value();
                //std::cout << "K= " << k_index << std::endl;
                curr_card_table[k_index][curr_pair.first]--;

                if (curr_card_table[k_index][curr_pair.first] == 0){
                    auto trans_remove = rev_auto->state_table[curr_pair.first]->get_trans_row(symbol);
                    if (trans_remove == nullptr){
                        continue;
                    }

                    for (const auto& remove_state: *trans_remove){
                        int remove_index = remove_state->get_value();

                        if (omega_matrix[remove_index][k_index]){
                            //std::cout << "Removing m= " << remove_index << " - k=" << k_index  <<std::endl;
                            omega_matrix[remove_index][k_index] = false;
                            gone_stack.emplace_back(std::make_pair(remove_index, k_index));
                        }
                    }
                }
            }
        }
    }

    symetric_fragment(omega_matrix);
    ret_matrix = std::move(omega_matrix);
}


void automata::init_card_tables(std::vector <std::vector <std::vector<int>>>& card_tables){
    int state_num = static_cast <int> (this->state_table.size());

    for (int i = 0; i < this->alphabet; i++) {
        std::vector <std::vector <int>> column(state_num);

        for (int j = 0; j < state_num; j++){
            int card = this->state_table[j]->get_trans_card(i);

            std::vector <int> row(state_num);
            std::fill(row.begin(), row.end(), card);

            column[j] = std::move(row);
        }
        card_tables[i] = std::move(column);
    }
}


void automata::init_omega_matrix(std::vector<std::vector<bool>>& omega_matrix,
                                 std::vector <std::pair <int, int>>& gone_stack){
    for (int i = 0; i < this->state_table.size(); i++){
        if (std::count(this->accept_states.begin(), this->accept_states.end(), i) == 0){
            for (auto fin: this->accept_states){
                if (not omega_matrix[fin][i]){
                    continue;
                }
                omega_matrix[fin][i] = false;
                gone_stack.emplace_back(fin, i);
            }
        }

        for (int j = 0; j < this->state_table.size(); j++){
            if (omega_matrix[i][j] && this->state_table[j]->not_under_simulate(this->state_table[i])){
                omega_matrix[i][j] = false;
                gone_stack.emplace_back(i, j);
            }
        }
    }
}


bool automata::same_alphabets(const std::shared_ptr <automata>& second){
    return this->dict.check_alphabets(second->dict);
}

void automata::remove_rezidual_state(const std::string& state_value, std::vector <int>& covering){
    int state_index = this->dict.get_state_index(state_value);          //remove covered state
    auto state_ptr = this->state_table[state_index];
    this->dict.remove_state(state_index);

    unsigned int old_id, new_id;
    while (this->dict.smooth_vector_state(old_id, new_id)){
        this->state_table[new_id] = this->state_table[old_id];
        this->state_table[new_id]->set_value(static_cast<int> (new_id));
    }

    this->state_table.pop_back();

    ptr_state_vector covering_ptr;
    int index;
    for (const auto& cov: covering){
        covering_ptr.push_back(this->state_table[cov]);
    }

    for (const auto& state: this->state_table){
        state->change_coverable(covering_ptr, state_ptr);
    }
    //ziskat vector pointerov
    //vsetky prechody do tohto stavu sa presunu do stavov co ho pokryvaju
}

void automata::create_rezidual_state(const ptr_state_vector& base, const std::shared_ptr <automata>& rezid,
                                     int base_value, std::vector <ptr_state_vector>& all_states){
    ptr_state_vector state_vec;
    std::set <std::shared_ptr<auto_state>> tmp_set;
    bool recurse;

    for (int i = 0; i < this->alphabet; i++){
        tmp_set.clear();
        state_vec.clear();

        for (const auto& ptr_state: base){
            auto row_ptr = ptr_state->get_trans_row(i);
            if (row_ptr == nullptr){
                continue;
            }
            tmp_set.insert(row_ptr->begin(), row_ptr->end());
        }
        if (tmp_set.empty()){
            continue;
        }

        state_vec.assign(tmp_set.begin(), tmp_set.end());

        auto state_val = this->combine_states(state_vec);
        recurse = rezid->add_state(state_val);
        int new_index = static_cast<int> (rezid->dict.get_state_index(state_val));

        rezid->add_transition(i, base_value, new_index);

        // create new state and add trans and recurse
        if (recurse){
            all_states.push_back(state_vec);
            this->create_rezidual_state(state_vec, rezid, new_index, all_states);
        }
    }
}

bool automata::is_recurse_coverable(int check_index, const std::vector <ptr_state_vector>& states, int start,
                         const std::vector <int>& covering, std::vector <bool>& checked_out, const std::shared_ptr<automata>& rezid){
    auto check = states[check_index];
    std::set <std::shared_ptr<auto_state>> tmp_set;
    std::vector <std::shared_ptr<auto_state>> compare_vec;
    std::vector <int> covers;

    for (int k = start; k < covering.size(); k++){
        int i = covering[k];
        if (checked_out[i]){
            continue;
        }

        if (std::includes(check.begin(), check.end(), states[i].begin(), states[i].end())){
            tmp_set.insert(states[i].begin(), states[i].end());
            covers.push_back(i);
        }
    }
    //is sorted because of set
    compare_vec.assign(tmp_set.begin(), tmp_set.end());

    if (compare_vec == check) {
        if (covers.size() > 2){
            for (int j = 0; j < covers.size()-1; j++){
                if (states[covers[j]].size() < 2){      //is sorted
                    break;
                }
                if (checked_out[covers[j]]){
                    covers[j] = -1;
                    continue;
                }

                if (this->is_recurse_coverable(covers[j], states, j+1, covers, checked_out, rezid)){
                    checked_out[covers[j]] = true;
                    covers[j] = -1;
                }
            }
        }

        std::vector <int> covering_vec;
        for (auto index: covers){
            if (index == -1){
                continue;
            }
            auto name = this->combine_states(states[index]);
            covering_vec.push_back(rezid->dict.get_state_index(name));
        }

        rezid->remove_rezidual_state(this->combine_states(check), covering_vec);
        checked_out[check_index] = true;
        return true;
    }
    return false;
}

void automata::is_coverable(int check_index, const std::vector <ptr_state_vector>& states, int start,
                            std::vector <bool>& checked_out, const std::shared_ptr<automata>& rezid){
    auto check = states[check_index];               // get the state which is being checked
    std::set <std::shared_ptr<auto_state>> tmp_set;
    std::vector <std::shared_ptr<auto_state>> compare_vec;
    std::vector <int> covers;       //vector of indexes which covers the state

    for (int i = start; i < states.size(); i++){
        if (checked_out[i]){
            continue;
        }

        if (std::includes(check.begin(), check.end(), states[i].begin(), states[i].end())){
            tmp_set.insert(states[i].begin(), states[i].end());
            covers.push_back(i);
        }
    }
    compare_vec.assign(tmp_set.begin(), tmp_set.end());

    if (compare_vec == check){
        //check all in the tree structure recursively
        for (int j = 0; j < covers.size()-1; j++){
            if (checked_out[covers[j]]){
                covers[j] = -1;
                continue;
            }

            if (this->is_recurse_coverable(covers[j], states, j+1, covers, checked_out, rezid)){
                checked_out[covers[j]] = true;
                covers[j] = -1;
            }
        }

        std::vector <int> covering_vec;     //indexes of states in automata which are covering the state
        for (auto index: covers){
            if (index == -1){
                continue;
            }
            auto name = this->combine_states(states[index]);
            covering_vec.push_back(rezid->dict.get_state_index(name));
        }

        rezid->remove_rezidual_state(this->combine_states(check), covering_vec);
        checked_out[check_index] = true;
    }
}

std::shared_ptr <automata> automata::rezidual(){
    std::shared_ptr <automata> rezid = std::make_shared <automata> ();      // create new automata with the same alphabet
    for (int i = 0; i < this->alphabet; i++){
        rezid->add_alphabet(this->dict.get_alpha_name(i));
    }

    std::vector <ptr_state_vector> new_states;
    ptr_state_vector new_init;
    for (auto init: this->init_states){                     // make one init state
        new_init.push_back(this->state_table[init]);
    }

    new_states.push_back(new_init);
    auto init_val = combine_states(new_init);
    rezid->add_init_state_force(init_val);

    //create all rezidual states
    this->create_rezidual_state(new_init, rezid, static_cast <int> (rezid->dict.get_state_index(init_val)), new_states);

    std::cout << "Rezid " << rezid->get_state_number() << std::endl;

    std::sort(new_states.begin(), new_states.end(), [](const ptr_state_vector & a, const ptr_state_vector & b){ return a.size() > b.size(); });
    std::vector <bool> checked_out(new_states.size(), false);       //help vector

    for (auto state: new_states){
        // vectors needs to be sorted for includes
        std::sort(state.begin(), state.end());
    }

    for (int i = 0; i < new_states.size()-1; i++){
        if (new_states[i].size() == 1){     //it's sorted
            break;
        }
        if (checked_out[i]){
            continue;
        }

        //check whether it is coverable, if so remove it from the automata
        this->is_coverable(i, new_states, i+1, checked_out, rezid);
    }

    ptr_state_vector accept_ptr;
    for (auto accept: this->accept_states){
        accept_ptr.push_back(this->state_table[accept]);
    }

    for (auto& state: new_states){
        if (has_intersect(state, accept_ptr)){
            if (rezid->dict.state_exists(this->combine_states(state))){
                rezid->add_accept_state(rezid->dict.get_state_index(this->combine_states(state)));
            }
        }
    }

    return rezid;
}


void automata::print(){
    std::cout << "Printing state table\n";
    for (const auto& element: this->state_table){
        if (element == nullptr){
            continue;
        }
        element->print();
    }
    std::cout << "Printing alphabet\n";
    std::cout << this->alphabet << std::endl;

    std::cout << "Printing initial states:\n";
    for (const auto& element: this->init_states){
        std::cout << element << " ";
    }
    std::cout << std::endl;

    std::cout << "Printing accept states\n";
    for (const auto& element: this->accept_states){
        std::cout << element << " ";
    }
    std::cout << std::endl;

    this->dict.print();
    std::cout << "------------------------------------------------------------" << std::endl;
}

void automata::save_to_file(const std::string& filename, const std::string& type){
    std::ofstream output(filename);
    if (output.is_open()){
        output << "@" << type << std::endl << "%Initial";
        for (auto init: this->init_states){
            output << " " << this->dict.get_state_name(init);
        }

        output << std::endl << "%States";
        for (int i = 0; i < this->state_table.size(); i++){
            output << " " << this->dict.get_state_name(i);
        }

        output << std::endl << "%Final";
        for (auto init: this->accept_states){
            output << " " << this->dict.get_state_name(init);
        }

        output << std::endl << std::endl;

        for (int i = 0; i < this->state_table.size(); i++){
            auto from = this->dict.get_state_name(i);
            for (int a = 0; a < this->alphabet; a++){
                auto curr_symbol = this->dict.get_alpha_name(a);
                auto row = this->state_table[i]->get_trans_row(a);
                if (row == nullptr){
                    continue;
                }

                for (const auto& to: *row){
                    output << from << " " << curr_symbol << " " << this->dict.get_state_name(to->get_value()) << std::endl;
                }
            }
        }
        output.close();
    }
    std::cout << std::endl << "File " << filename << " saved" << std::endl;
}



void automata::find_examples(const std::shared_ptr<automata_stats>& stats) {
    std::queue <std::queue<unsigned int>> word_stack;
    std::queue <int> state_stack;
    int max_len = static_cast <int> (this->get_state_number());

    for (auto init: this->init_states){
        state_stack.push(init);
        word_stack.emplace(std::queue <unsigned int>());
    }

    std::queue <unsigned int> current_word;
    int current_state;
    std::queue <unsigned int> tmp_word;

    while(not state_stack.empty()){
        current_word = word_stack.front();
        current_state = state_stack.front();
        word_stack.pop();
        state_stack.pop();

        if (current_word.size() > max_len){
            break;
        }

        if (this->is_final(current_state)){
            stats->add_accept(current_word);
        }
        else {
            stats->add_reject(current_word);
        }

        for (int i = 0; i < this->get_alphabet(); i++){
            if (this->get_next_state(i, current_state) == -1){
                tmp_word = current_word;
                tmp_word.push(i);
                stats->add_reject(tmp_word);
                continue;
            }
            state_stack.push(this->get_next_state(i, current_state));
            tmp_word = current_word;
            tmp_word.push(i);
            word_stack.push(tmp_word);
        }
    }

    stats->set_symbols(this->alphabet);
}

bool sat_intersect(const std::shared_ptr <automata>& first, const std::shared_ptr <automata>& second,
                   std::vector <std::queue<unsigned int>>* add){
    std::vector <std::queue<unsigned int>> word_stack;
    std::vector <std::pair <int, int>> state_stack;
    std::set <std::pair <int, int>> gone_through;

    auto start = std::make_pair(first->get_init(), second->get_init());
    state_stack.push_back(start);
    word_stack.emplace_back(std::queue <unsigned int>());
    gone_through.insert(start);

    std::queue <unsigned int> current_word;
    std::pair <int, int> current_pair;
    std::queue <unsigned int> tmp_word;
    bool result = false;
    unsigned int cnt = 0;

    while(not state_stack.empty()){
        current_word = word_stack.back();
        current_pair = state_stack.back();
        word_stack.pop_back();
        state_stack.pop_back();

        if (first->is_final(current_pair.first) and second->is_final(current_pair.second)){
            add->push_back(current_word);
            cnt++;
            result = true;
        }

        if (cnt > 2){
            break;
        }

        for (int i = 0; i < first->get_alphabet(); i++){
            auto new_pair = std::make_pair(first->get_next_state(i, current_pair.first),
                                           second->get_next_state(i, current_pair.second));

            if (gone_through.find(new_pair) == gone_through.end()){
                state_stack.push_back(new_pair);
                tmp_word = current_word;
                tmp_word.push(i);
                word_stack.push_back(tmp_word);
                gone_through.insert(new_pair);
            }
        }
    }
    return result;
}

bool sat_equal(const std::shared_ptr <automata>& sat, const std::shared_ptr <automata>& orig,
               const std::shared_ptr <automata_stats>& output){
    auto dfa_sat = determine_nfa(sat);
    auto dfa_orig = determine_nfa(orig);
    auto sat_comple = dfa_sat->complement();
    auto orig_comple = dfa_orig->complement();

    if (sat_intersect(dfa_sat, orig_comple, output->get_reject()) or
        sat_intersect(sat_comple, dfa_orig, output->get_accept())){
        return false;
    }
    return true;
}

std::vector <int>& automata::get_init_vec(){
    return this->init_states;
}

bool automata::is_product_final(const std::set <int>& check){
    for (auto state: check){
        if (is_final(state)){
            return true;
        }
    }
    return false;
}

std::vector <int> automata::get_next_vec(const int& symbol, const int& state){
    auto row = this->state_table[state]->get_trans_row(symbol);
    std::vector <int> index_vec;
    if (row == nullptr){
        return index_vec;
    }
    for (const auto& add_state: *row){
        index_vec.push_back(add_state->get_value());
    }
    return index_vec;
}

bool check_product(std::vector <std::pair <int, std::set <int>>>& covering, int state, std::set <int>& product){
    bool result = true;
    for (int i = 0; i < covering.size(); i++){

        if (covering[i].first != state){
            continue;
        }
        if (covering[i].second == product){
            return false;
        }
        if (std::includes(covering[i].second.begin(), covering[i].second.end(), product.begin(), product.end())){
            covering.erase(covering.begin()+i);
            i--;
            continue;
        }
        if (std::includes(product.begin(), product.end(), covering[i].second.begin(), covering[i].second.end())){
            result = false;
        }
    }
    return result;
}

bool chain_part(const std::shared_ptr <automata>& first, const std::shared_ptr <automata>& second){
    std::vector <std::pair <int, std::set <int>>> state_stack;
    std::vector <std::pair <int, std::set <int>>> gone_through;

    auto init_vec = first->get_init_vec();
    auto join_init = second->get_init_vec();
    std::set <int> joined(join_init.begin(), join_init.end());

    std::pair <int, std::set <int>> new_pair;

    for (auto init_state: init_vec){
        new_pair = std::make_pair(init_state, joined);

        state_stack.push_back(new_pair);
        gone_through.push_back(new_pair);
    }

    std::pair <int, std::set <int>> current_pair;
    std::set <int> product_set;

    while(not state_stack.empty()){
        current_pair = state_stack.back();
        state_stack.pop_back();


        if (first->is_final(current_pair.first) and not second->is_product_final(current_pair.second)){
            //std::cout << "FOUND WRONG\n";
            return true;
        }

        for (int i = 0; i < first->get_alphabet(); i++){
            product_set.clear();
            auto first_row = first->get_next_vec(i, current_pair.first);
            for (auto state: current_pair.second){
                auto insert_row = second->get_next_vec(i, state);
                product_set.insert(insert_row.begin(), insert_row.end());
            }

            for (auto state: first_row){

                if (check_product(gone_through, state, product_set)){
                    new_pair = std::make_pair(state, product_set);
                    state_stack.push_back(new_pair);
                    gone_through.push_back(new_pair);
                }
            }
        }
    }
    return false;
}

bool sat_anticahin(const std::shared_ptr <automata>& sat, const std::shared_ptr <automata>& orig){
    if (chain_part(sat, orig) or chain_part(orig, sat)){
        return false;
    }
    return true;
}