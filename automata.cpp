/*
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 3.10 2022
* Subject: Bachelor's thesis
*/

#include <iostream>
#include "automata.h"

// Delimeter for combining states
#define DELIM "."

#if 1
/// Connects two given strings in alphabetical order
std::string combine_string(const std::string& first, const std::string& second){
    return first < second? first + DELIM + second : second + DELIM + first;
}

/**
 * Takes values of passed states and combine them into one in alphabetical order
 * @param states Set of pointers to automata states to be merged together
 * @return Value of single state
 */
std::string combine_states(state_set states){
    if (states.size() == 1){
        return (*states.begin())->get_value();
    }

    auto tmp = *(states.begin());
    std::string first = tmp->get_value();
    states.erase(tmp);
    for (const auto& second: states){
        first = combine_string(first, second->get_value());
    }
    return first;
}

/// Checks whether two sets of pointer to states have an intersection
bool has_intersect(const state_set& first,
                   const state_set& second){
    std::vector <std::shared_ptr <auto_state>> tmp(5);
    std::vector <std::shared_ptr <auto_state>>::iterator last;
    last = std::set_intersection(first.begin(), first.end(), second.begin(), second.end(), tmp.begin());
    if (last - tmp.begin()){
        return true;
    }
    return false;
}

/**
 * Determines the state by taking its transitions for each symbol, combining them into one state
 * and adding a single transition
 * @param dfa Pointer to the automata which is being created
 * @param set_queue Pointer to vector of states waiting to be processed
 * @param current_set The current set of states being processed
 * @param current_state Value of newly created state
 */
void determine_state(const std::shared_ptr<automata>& dfa, std::vector <state_set>* set_queue,
                     const state_set& current_set, const std::string& current_state){
    for (const auto& symbol: dfa->get_alphabet()){
        state_set new_state_set;
        std::string new_value;

        // gets the new  set of states
        for (const auto& state: current_set){
            auto search = state->get_trans_row(symbol);
            if (search.empty()){
                continue;
            }
            new_state_set.insert(search.begin(), search.end());
        }

        if (new_state_set.empty()){
            new_value = DEAD;
        }
        else{
            new_value = combine_states(new_state_set);
        }

        if (dfa->add_state(new_value)){
            set_queue->push_back(new_state_set);
        }

        dfa->add_transition(symbol, current_state, new_value);
    }
}


//TODO nieco je tu velmi zle
void minimal_dfa(const std::shared_ptr <automata>& dnf){
    std::list <power_element> pow_set_old;
    std::list <power_element> pow_set_new;
    dnf->init_power_set(pow_set_new);
    do {
        pow_set_old = pow_set_new;
        pow_set_new.clear();
        //auto time = clock();
        dnf->split_power_state(pow_set_old, pow_set_new);
        //std::cout << std::fixed << (float) (clock() - time) / CLOCKS_PER_SEC << std::endl;
    }  while(pow_set_old != pow_set_new);

    //auto tim = clock();
    dnf->min_power(pow_set_new);
    //std::cout << std::fixed << (float) (clock() - tim) / CLOCKS_PER_SEC << std::endl;
}

/// Determines the given automata and returns a new one
std::shared_ptr <automata> determine_nfa(const std::shared_ptr <automata>& nfa){
    nfa->remove_eps_transitions();
    auto dfa = nfa->determine();
    return dfa;
}

std::shared_ptr <automata> det_n_min(const std::shared_ptr <automata>& nfa){
    auto dfa = determine_nfa(nfa);
    minimal_dfa(dfa);
    return dfa;
}

/// Checks whether two given automatas have intersection in their languages
bool language_intersect(const std::shared_ptr <automata>& first,const std::shared_ptr <automata>& second){
    std::vector <std::string> word_stack;
    std::vector <std::pair <std::string, std::string>> state_stack;
    std::set <std::pair <std::string, std::string>> gone_through;

    if (first->get_alphabet() != second->get_alphabet()){
        return true;
    }

    state_stack.emplace_back(first->get_init(), second->get_init());
    word_stack.emplace_back("");
    gone_through.insert(std::make_pair(first->get_init(), second->get_init()));

    while(not state_stack.empty()){
        std::string current_word = word_stack.back();
        std::pair <std::string, std::string> current_pair = state_stack.back();
        word_stack.pop_back();
        state_stack.pop_back();

        if (first->is_final(current_pair.first) and second->is_final(current_pair.second)){
            std::cout << "FOUND WORD: " << current_word << std::endl;
            return true;
        }

        for (const auto& symbol: first->get_alphabet()){
            auto new_pair = std::make_pair(first->get_next_state(symbol, current_pair.first),
                                              second->get_next_state(symbol, current_pair.second));

            if (gone_through.find(new_pair) == gone_through.end()){
                state_stack.push_back(new_pair);
                word_stack.push_back(current_word + symbol);
                gone_through.insert(new_pair);
            }
        }
    }
    return false;
}

/// Checks whether two given automata have equal languages
bool language_equal(const std::shared_ptr <automata>& first, const std::shared_ptr <automata>& second){
    auto dfa_first = determine_nfa(first);
    auto dfa_second = determine_nfa(second);
    auto first_comple = dfa_first->complement();
    auto second_comple = dfa_second->complement();

    if (language_intersect(dfa_first, second_comple) or
        language_intersect(first_comple, dfa_second)){
        return false;
    }
    else{
        return true;
    }
}

//todo neico tu je velmi zle
state_set find_power_closure(std::list <power_element>& power_set, const std::shared_ptr <auto_state>& needle){
    for(auto closure: power_set){
        auto tmp = closure.get_set();
        if (tmp.find(needle) != tmp.end()){
            return tmp;
        }
    }
    state_set empty;
    return empty;
}
//todo aj tu je nieco zle
void insert_pow_set(power_element adding, std::list <power_element>& power_set, const state_set& old){
    for (auto& element: power_set){
        if (element.get_trans().empty()){
            continue;
        }
        if (element.get_trans() == adding.get_trans()){
            element.add_state(*(adding.get_set().begin()));
            return;
        }
    }
    power_set.push_back(adding);
}

std::shared_ptr <auto_state> get_smallest_state(const state_set& states){
    std::string value = (*states.begin())->get_value();
    std::shared_ptr <auto_state> found = nullptr;
    for (const auto& element: states){
        std::string tmp = element->get_value();
        if (tmp <= value){
            value = tmp;
            found = element;
        }
    }
    return found;
}


power_element::power_element(const std::shared_ptr <auto_state>& state,
                             std::unordered_map <std::string, state_set>& trans){
    this->same_set.insert(state);
    this->transition = trans;
}

power_element::power_element(state_set set){
    this->same_set = std::move(set);
}

power_element::power_element(){
    ;
}

state_set power_element::get_set(){
    return this->same_set;
}

std::unordered_map <std::string, state_set> power_element::get_trans(){
    return this->transition;
}

void power_element::add_state(const std::shared_ptr <auto_state>& state){
    this->same_set.insert(state);
}

void power_element::set_trans(const std::unordered_map <std::string, state_set>& new_trans){
    this->transition = new_trans;
}




// Method print - prints the value and all transitions of the state
void auto_state::print(){
    std::cout << "State value" << std::endl;
    std::cout << this->value << std::endl;

    std::cout << "Transitions:" << std::endl;
    for (const auto& element: this->transitions){
        std::cout << element.first << " -> ";
        for (const auto& elem: element.second){
            std:: cout << elem->value << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

// Method has_eps - checks whether the state has any transition through epsilon symbol,
//                  if so returns true
bool auto_state::has_eps(){
    if (this->transitions.count(EPS)){
        return true;
    }
    else{
        return false;
    }
}

// Method get_value - returns the value of the state
std::string auto_state::get_value(){
    return this->value;
}

// Method get_trans_row - takes a symbol and returns a pointer of the set of states
//                          reachable through such symbol
state_set auto_state::get_trans_row(const std::string& symbol){
    auto search = this->transitions.find(symbol);
    if (search == this->transitions.end()){
        state_set empty;
        return empty;
    }
    else{
        return search->second;
    }
}

std::unordered_map <std::string, state_set> auto_state::get_trans(){
    return this->transitions;
}

//todo smarismaria
void auto_state::get_pow_trans(std::list <power_element>& previous,
                               std::unordered_map <std::string, state_set>& new_trans,
                               std::unordered_map <std::shared_ptr <auto_state>, state_set>& helper){
    for (const auto& row: this->transitions){
        state_set found_closure;
        auto search = *(row.second.begin());
        if (helper.find(search) != helper.end()){
            found_closure = helper[search];
        }
        else{
            found_closure = find_power_closure(previous, search);
            if (found_closure.empty()){
                std::cerr << "Couldn't find any closure with given state in get_pow_trans" << std::endl;
                continue;
            }

            helper[search] = found_closure;
        }

        new_trans[row.first] = found_closure;
    }
}

// Method add_transition - takes a symbol and pointer to another state, creates a new transition
//                       - shows and error message if adding a duplicate transition
void auto_state::add_transition(const std::string& symbol, const std::shared_ptr <auto_state>& trans_to){
    auto search = this->transitions.find(symbol);
    if (search == this->transitions.end()){
        state_set new_set;
        new_set.insert(trans_to);
        this->transitions[symbol] = new_set;
    }
    else{
        if (search->second.count(trans_to)){
            std::cerr << "Trying to add a duplicate transition in add_transition" << std::endl;
            return;
        }
        search->second.insert(trans_to);
    }
}

// Method add_transition_row - adds a multiple transitions through the same symbol
void auto_state::add_transition_row(const std::string& symbol,
                        const state_set& new_trans){
    auto search = this->transitions.find(symbol);
    if (search == this->transitions.end()){
        state_set new_set;
        new_set.insert(new_trans.begin(), new_trans.end());
        this->transitions[symbol] = new_set;
    }
    else{
        search->second.insert(new_trans.begin(), new_trans.end());
    }
}

// Method get_eps_transitions - returns epsilon transitions of the current state,
//                              if there are none returns and empty set
state_set auto_state::get_eps_transitions(){
    auto search_set = this->transitions.find(EPS);
    if (search_set == this->transitions.end()){
        state_set found;
        return found;
    }
    else{
        return search_set->second;
    }
}

// Method replace_eps_trans - deletes all epsilon transitions and adds all of the transitions form the
//                              set replace passed to the method
void auto_state::replace_eps_trans(const state_set& replace){
    this->transitions.erase(EPS);
    for (const auto& state: replace){
        for (const auto& new_trans: state->transitions){
            this->add_transition_row(new_trans.first, new_trans.second);
        }
    }
}

std::shared_ptr <auto_state> auto_state::get_next(const std::string& symbol){
    auto search = this->transitions.find(symbol);
    if (search == this->transitions.end()){
        return nullptr;
    }
    if (search->second.size() > 1){
        return nullptr;
    }
    else{
        return *(search->second.begin());   // gets the first element
    }
}

void auto_state::reverse_trans(const std::shared_ptr <automata>& reverse){
    for (const auto& row: this->transitions){
        for (const auto& state: row.second){
            reverse->add_state(state->get_value());
            reverse->add_transition(row.first, state->get_value(), this->value);
        }
    }
}

void auto_state::clear_trans(){
    this->transitions.clear();
}

void auto_state::set_power_state(const std::string& new_val,
                                 std::unordered_map <std::string, state_set>& new_trans){
    this->value = new_val;
    for (const auto& element: new_trans){
        auto smallest_state = get_smallest_state(element.second);
        this->transitions[element.first].clear();
        this->transitions[element.first].insert(smallest_state);
    }
}

//TODO je to extra hnusne sprav to nromalne
void auto_state::copy_state(const std::shared_ptr <automata>& comple){
    comple->add_state(this->value);
    if (this->value == DEAD){
        return;
    }

    for (const auto& element: this->transitions){
        for (const auto& state: element.second){
            comple->add_state(state->value);
            comple->add_transition(element.first, this->value, state->value);
        }
    }
}


// Method recurse_find_all_eps - recursive function used to get epsilon closure of the passed group,
//                               returns the found closure
state_set automata::recurse_find_all_eps( const state_set& search_group){
    state_set found;
    for (const auto& state: search_group){
        state_set new_add = state->get_eps_transitions();

        bool end = true;
        state_set next_search;
        for (const auto& new_state: new_add){
            if (found.count(new_state) == 0){
                found.insert(new_state);
                next_search.insert(new_state);
                end = false;
            }
        }
        if (not end){
            auto new_insert = this->recurse_find_all_eps(next_search);
            found.insert(new_insert.begin(), new_insert.end());
        }
    }
    return found;
}

// Constructor of the automata - takes a set of states, alphabet, initial and accept states
//                               and a vector of transitions
automata::automata(const std::set <std::string>& states, const std::set <std::string>& alphabet,
         const std::vector <std::array<std::string, 3>>& trans,
         const std::set <std::string>& init_states, const std::set <std::string>& fin_states)
{
    for (const std::string& state_number: states){
        std::shared_ptr <auto_state> new_state(new auto_state(state_number));
        this->state_table[state_number] = new_state;
    }

    this->alphabet = alphabet;

    for (const std::array <std::string, 3>& triple: trans){
        if (triple.size() < 3){
            std::cerr << "Wrong notation of transitions in automata constructor" << std::endl;
            continue;
        }

        auto search_from = this->state_table.find(triple[0]);
        if (search_from == this->state_table.end()){
            std::cerr << "Nonexistent state for transition in automata constructor" << std::endl;
            continue;
        }
        auto search_to = this->state_table.find(triple[2]);
        if (search_to == this->state_table.end()){
            std::cerr << "Nonexistent state for transition in automata constructor" << std::endl;
            continue;
        }
        search_from->second->add_transition(triple[1], search_to->second);
    }

    for (const std::string& in_state: init_states){
        auto search = this->state_table.find(in_state);
        if (search == this->state_table.end()) {
            std::cerr << "Nonexistent initial state in automata constructor" << std::endl;
        }
        else {
            this->init_states.insert(search->second);
        }
    }

    for (const std::string& fin_state: fin_states){
        auto search = this->state_table.find(fin_state);
        if (search == this->state_table.end()) {
            std::cerr << "Nonexistent final state in automata constructor" << std::endl;
        }
        else {
            this->accept_states.insert(search->second);
        }
    }
}

// Constructor of the automata - takes only the alphabet and one state
automata::automata (const std::string& state, const std::set <std::string>& alphabet){
    std::shared_ptr <auto_state> new_state(new auto_state(state));
    this->alphabet = alphabet;
    this->state_table[state] = new_state;
}

automata::automata() {
    ;
}

// Copy constructor TODO je to zle
std::shared_ptr <automata> automata::copy(){
    std::shared_ptr <automata> copy(new automata((*this->init_states.begin())->get_value(),
                                                            this->alphabet));

    for (const auto& element: this->state_table){
        element.second->copy_state(copy);
    }

    for (const auto& state: this->init_states){
        copy->add_init_state(state->get_value());
    }

    for (const auto& state: this->accept_states){
        copy->add_accept_state(state->get_value());
    }
    return copy;
}

void automata::min_power(const std::list <power_element>& power_set){
    this->state_table.clear();
    for (auto element: power_set){
        auto states_set = element.get_set();
        auto trans = element.get_trans();
        std::string new_value = combine_states(states_set);

        std::shared_ptr <auto_state> stays = get_smallest_state(states_set);

        stays->set_power_state(new_value, trans);
        this->state_table[new_value] = stays;

        for (const auto& state: states_set){
            if (state != stays){
                state->clear_trans();
                this->accept_states.erase(state);
                this->init_states.erase(state);
            }
        }
    }
}

// Method add_state - adds a new state to the automata, if the state is already present returns false
//                  - in case of adding a dead state the infinite loop is also added and
//                    always returns false
bool automata::add_state(const std::string& state_value){
    if (this->state_table.find(state_value) != this->state_table.end()){
        return false;
    }
    std::shared_ptr <auto_state> new_state(new auto_state(state_value));
    this->state_table[state_value] = new_state;
    if (state_value == DEAD){
        for (const auto& symbol: this->alphabet){
            new_state->add_transition(symbol, new_state);
        }
        return false;
    }
    else{
        return true;
    }
}

void automata::add_alphabet(const std::string& new_symbol){
    this->alphabet.insert(new_symbol);
}

// Method add_transition - adds a new transitions to the automata
//                       - if some of the states doesn't exist in the automata an error message is shown
void automata::add_transition(const std::string& symbol, const std::string& from, const std::string& to){
    auto search_from = this->state_table.find(from);
    if (search_from == this->state_table.end()){
        std::cerr << "Nonexistent state for transition in automata constructor" << std::endl;
        return;
    }
    auto search_to = this->state_table.find(to);
    if (search_to == this->state_table.end()){
        std::cerr << "Nonexistent state for transition in automata constructor" << std::endl;
        return;
    }
    search_from->second->add_transition(symbol, search_to->second);
}

void automata::add_init_state(const std::string& init_state){
    auto search = this->state_table.find(init_state);
    if (search == this->state_table.end()) {
        std::cerr << "Nonexistent initial state in automata constructor" << std::endl;
    }
    else {
        this->init_states.insert(search->second);
    }
}

void automata::add_init_state_force(const std::string& init_state){
    this->add_state(init_state);
    auto search = this->state_table.find(init_state);
    if (search != this->state_table.end()) {
        this->init_states.insert(search->second);
    }
    else {
        std::cerr << "Something wrong in add_init_state_force" <<std::endl;
    }
}

// Method add_accept_state - adds accept state to the automata
//                         - if the state doesn't exist in the automata an error message is shown
void automata::add_accept_state(const std::string& fin_state){
    auto search = this->state_table.find(fin_state);
    if (search == this->state_table.end()) {
        std::cerr << "Nonexistent final state in automata constructor" << std::endl;
    }
    else {
        this->accept_states.insert(search->second);
    }
}

void automata::add_accept_state(const std::shared_ptr <auto_state>& fin_state){
    this->accept_states.insert(fin_state);
}

void automata::add_accept_state_force(const std::string& fin_state){
    this->add_state(fin_state);
    auto search = this->state_table.find(fin_state);
    if (search != this->state_table.end()) {
        this->accept_states.insert(search->second);
    }
    else {
        std::cerr << "Something wrong in add_accept_state_force" <<std::endl;
    }
}


std::set <std::string> automata::get_alphabet(){
    return this->alphabet;
}

std::string automata::get_init(){
    return (*this->init_states.begin())->get_value();
}

unsigned long automata::get_state_number(){
    return this->state_table.size();
}

bool automata::is_final(const std::string& state){
    auto search = this->state_table.find(state);
    if (search == this->state_table.end()){
        std::cerr << "Searching nonexistent final state in is_final " << state << std::endl;
        return false;
    }
    if (this->accept_states.count(search->second)){
        return true;
    }
    return false;
}

std::string automata::get_next_state(const std::string& symbol, const std::string& state){
    auto search = this->state_table.find(state);
    if (search == this->state_table.end()){
        std::cerr << "Searching nonexistent state in get_next_state" << std::endl;
        return "";
    }
    auto next = search->second->get_next(symbol);
    if (next == nullptr){
        std::cerr << "Error while searching for next state in get_next_state" << std::endl;
        return "";
    }
    else{
        return next->get_value();
    }
}

// Method remove_eps_transitions - find epsilon closures for all states and using them replaces all
//                                 epsilon transitions in the automata
void automata::remove_eps_transitions(){
    for (const auto& state: this->state_table){
        if (not state.second->has_eps()){
            continue;
        }

        state_set eps_closure;
        state_set tmp_set = {state.second};
        eps_closure = this->recurse_find_all_eps(tmp_set);

        if (has_intersect(eps_closure, this->accept_states)){
            this->add_accept_state(state.second);
        }

        state.second->replace_eps_trans(eps_closure);
    }
    this->alphabet.erase(EPS);
}

// Method determine - creates and returns a new deterministic automata representing the same language
std::shared_ptr <automata> automata::determine(){
    std::vector <state_set> set_queue;
    set_queue.push_back(this->init_states);
    std::string new_value = combine_states(this->init_states);
    std::shared_ptr <automata> dfa(new automata(new_value, this->alphabet));
    dfa->add_init_state(new_value);

    while (not set_queue.empty()){
        auto state_set = set_queue.back();
        set_queue.pop_back();
        new_value = combine_states(state_set);

        if (has_intersect(this->accept_states, state_set)){
            dfa->add_accept_state(new_value);
        }

        determine_state(dfa, &set_queue, state_set, new_value);
    }

    return dfa;
}

std::shared_ptr <automata> automata::reverse(){
    std::shared_ptr <automata> reverse(new automata((*this->accept_states.begin())->get_value(),
                                                            this->alphabet));

    for (const auto& state: this->state_table){
        reverse->add_state(state.second->get_value());
        state.second->reverse_trans(reverse);
    }

    for (const auto& init_state: this->init_states){
        reverse->add_accept_state(init_state->get_value());
    }
    for (const auto& final_state: this->accept_states){
        reverse->add_init_state(final_state->get_value());
    }

    return reverse;
}


//todo no kamo co tooto je
void automata::split_power_state(std::list <power_element>& previous, std::list <power_element>& next){
    std::unordered_map <std::shared_ptr <auto_state>, state_set> help_table;
    for (auto closure: previous){
        if (closure.get_set().size() == 1){
            std::unordered_map <std::string, state_set> trans;
            (*closure.get_set().begin())->get_pow_trans(previous, trans, help_table);
            closure.set_trans(trans);
            next.push_back(closure);
            continue;
        }

        std::list <power_element> new_part;
        auto old_set = closure.get_set();
        for (const auto& element: old_set){
            //auto time = clock();
            std::unordered_map <std::string, state_set> trans;
            element->get_pow_trans(previous, trans, help_table);
            //auto tim = clock();
            insert_pow_set(power_element(element, trans), new_part, old_set);
            //std::cout << std::fixed << (float) (tim - time) / CLOCKS_PER_SEC << " " <<
            //         (float) (clock() - tim) / CLOCKS_PER_SEC << std::endl;
        }
        next.insert(next.end(), new_part.begin(), new_part.end());
    }
}


void automata::reverse_accept_states(){
    for (const auto& element: this->state_table){
        if (this->accept_states.find(element.second) != this->accept_states.end()){
            this->accept_states.erase(element.second);
        }
        else{
            this->accept_states.insert(element.second);
        }

    }
}

std::shared_ptr <automata> automata::complement(){
    auto comple = this->copy();

    comple->reverse_accept_states();
    return comple;
}

// Method print - prints the automata
void automata::print(){
    std::cout << "Printing state table\n";
    for (const auto& element: this->state_table){
        //std::cout << element.first << " points to " << std::endl;
        element.second->print();
    }
    std::cout << "Printing alphabet\n";
    for (const auto& element: this->alphabet){
        std::cout << element << " ";
    }
    std::cout << std::endl;

    std::cout << "Printing initial states:\n";
    for (const auto& element: this->init_states){
        std::cout << element->get_value() << " ";
    }
    std::cout << std::endl;

    std::cout << "Printing accept states\n";
    for (const auto& element: this->accept_states){
        std::cout << element->get_value() << " ";
    }
    std::cout << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
}



#endif