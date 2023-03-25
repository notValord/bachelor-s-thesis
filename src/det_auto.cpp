//
// Created by vmvev on 3/20/2023.
//

#include "det_auto.h"

#define DELIM "."

det_auto::det_auto(const std::set<std::string> &states, const std::set<std::string> &alphabet,
                   const std::vector<std::array<std::string, 3>> &trans, const std::set<std::string> &init_states,
                   const std::set<std::string> &fin_states)
        : automata(states, alphabet, trans, init_states, fin_states) {
    ;
}

det_auto::det_auto(const std::string& state, const std::vector <std::string>& alphabet) : automata(state, alphabet){
    ;
};

det_auto::det_auto(int alphabet, auto_dictionary& dict) : automata(alphabet, dict){
    ;
};

det_auto::det_auto() : automata() {
    ;
}

std::shared_ptr <det_auto> determine_nfa(const std::shared_ptr <det_auto>& nfa){
    nfa->remove_eps_transitions();
    auto dfa = nfa->determine();
    return dfa;
}

std::shared_ptr <det_auto> det_auto::copy(){
    std::shared_ptr <det_auto> copy = std::make_shared <det_auto> (this->alphabet, this->dict); //huh

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

std::shared_ptr <det_auto> det_auto::reverse(){
    std::shared_ptr <det_auto> reverse = std::make_shared <det_auto> (this->alphabet, this->dict);

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

void det_auto::reverse_accept_states(){
    std::vector <int> new_accept;
    for (int i = 0; i < this->state_table.size(); i++){
        if (std::find(this->accept_states.begin(), this->accept_states.end(), i) == this->accept_states.end()){
            new_accept.push_back(i);
        }
    }
    this->accept_states.clear();
    this->accept_states = std::move(new_accept);
}

//todo bude presunut kopiu alebo idk
std::shared_ptr <det_auto> det_auto::complement(){
    auto comple = this->copy();

    comple->reverse_accept_states();
    return comple;
}


std::string det_auto::combine_states(ptr_state_vector states){
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

void det_auto::determine_state(det_auto* old, std::vector <ptr_state_vector>& set_queue,
                               const ptr_state_vector& current_set, int current_state){
    ptr_state_vector new_state_vect;
    std::set <std::shared_ptr<auto_state>> tmp_set;
    std::string new_value;

    for (int i = 0; i < old->get_alphabet(); i++){
        new_state_vect.clear();
        tmp_set.clear();

        // gets the new  set of states
        for (const auto& state: current_set){
            auto search = state->get_trans_row(i);
            if (search == nullptr){
                continue;
            }

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

void det_auto::recurse_find_all_eps(const ptr_state_vector& search_group, int eps_index, ptr_state_vector& ret){
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

void det_auto::remove_eps_transitions(){
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

std::shared_ptr <det_auto> det_auto::determine(){
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

    std::shared_ptr <det_auto> dfa(std::make_shared <det_auto> (new_value, alpha));
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

void det_auto::find_examples(const std::shared_ptr<automata_stats>& stats) {
    this->remove_eps_transitions();
    auto dfa = this->determine();
    std::queue <std::queue<unsigned int>> word_stack;
    std::queue <int> state_stack;
    int max_len = static_cast <int> (this->get_state_number());

    for (auto init: dfa->init_states){
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

        if (dfa->is_final(current_state)){
            stats->add_accept(current_word);
        }
        else {
            stats->add_reject(current_word);
            if (dfa->dict.get_state_name(current_state) == DEAD){
                continue;
            }
        }

        for (int i = 0; i < dfa->get_alphabet(); i++){
            if (dfa->get_next_state(i, current_state) == -1){
                tmp_word = current_word;
                tmp_word.push(i);
                stats->add_reject(tmp_word);
                continue;
            }
            state_stack.push(dfa->get_next_state(i, current_state));
            tmp_word = current_word;
            tmp_word.push(i);
            word_stack.push(tmp_word);
        }
    }

    stats->set_symbols(dfa->alphabet);
}
