/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 24.04.2023
* Subject: Bachelor's thesis - 1st part
*/

#include "automata_base.h"

bool has_intersect(ptr_state_vector& first, ptr_state_vector& second){
    std::sort(first.begin(), first.end());      // sorting is required for the set_intersection()
    std::sort(second.begin(), second.end());

    std::vector <std::shared_ptr <auto_state>> tmp(first.size());
    std::vector <std::shared_ptr <auto_state>>::iterator last;

    last = std::set_intersection(first.begin(), first.end(), second.begin(), second.end(), tmp.begin());

    if (last - tmp.begin()){        // intersection found
        return true;
    }
    return false;
}

std::shared_ptr <auto_state> get_smallest_state(const ptr_state_vector& states){
    int value = (*states.begin())->get_value();         // first value
    std::shared_ptr <auto_state> found = nullptr;       // pointer for the state
    for (const auto& element: states){
        int tmp = element->get_value();
        if (tmp <= value){
            value = tmp;
            found = element;
        }
    }
    return found;
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
    else {
        for (const auto& elem: new_row){
            if (std::count(this->transitions[symbol]->begin(), this->transitions[symbol]->end(), elem) == 0){
                this->transitions[symbol]->push_back(elem);
            }
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
            if (help_table[state->get_value()].empty()){
                std::cerr << "Heh?\n";
                exit(-1);
            }
            new_trans.push_back(*help_table[state->get_value()].begin());
        }
        row->clear();
        row->insert(row->begin(), new_trans.begin(), new_trans.end());
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

std::shared_ptr <auto_state> automata::get_state(int index){
    if (index < 0 or index > this->state_table.size()-1){
        return nullptr;
    }
    return this->state_table[index];
}

std::vector <int>& automata::get_init_vec(){
    return this->init_states;
}

const std::vector <int>& automata::get_acc_vec(){
    return this->accept_states;
}

const auto_dictionary& automata::get_dict(){
    return this->dict;
}

const std::vector <std::shared_ptr <auto_state>>& automata::get_state_table(){
    return this->state_table;
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

bool automata::is_product_final(const std::set <int>& check){
    for (auto state: check){
        if (is_final(state)){
            return true;
        }
    }
    return false;
}

bool automata::same_alphabets(const std::shared_ptr <automata>& second){
    return this->dict.check_alphabets(second->dict);
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
