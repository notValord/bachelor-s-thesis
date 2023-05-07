/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#include "min_det_auto.h"

std::shared_ptr <det_auto> brzozowski(const std::shared_ptr <det_auto>& nfa){
    auto rev = nfa->reverse();
    auto dfa = determine_nfa(rev);
    rev = dfa->reverse();
    dfa = determine_nfa(rev);
    return dfa;
}


std::shared_ptr <det_auto> det_n_min(const std::shared_ptr <det_auto>& nfa){
    auto dfa = determine_nfa(nfa);
    minimal_dfa(std::static_pointer_cast <min_auto> (dfa));
    return dfa;
}

void minimal_dfa(const std::shared_ptr <min_auto>& dnf){
    std::vector <std::shared_ptr <power_element>> pow_vec_old;
    std::vector <std::shared_ptr <power_element>> pow_vec_new;
    dnf->init_power_set(pow_vec_new);

    do {
        pow_vec_old = std::move(pow_vec_new);       // old gets the new
        pow_vec_new.clear();

        dnf->split_power_state(pow_vec_old, pow_vec_new);       // split power states
    } while (not is_eq(pow_vec_new, pow_vec_old));

    dnf->min_power(pow_vec_new);            // create new automata from the power states
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

min_auto::min_auto(det_auto& base) {
    this->alphabet = base.get_alphabet();
    this->init_states = base.get_init_vec();
    this->accept_states = base.get_acc_vec();
    this->dict = base.get_dict();
    this->state_table = base.get_state_table();
}

void min_auto::min_power(const std::vector <std::shared_ptr<power_element>>& power_set){
    for (const auto& element: power_set){
        auto states_set = element->get_set();           // set of states representing the new state
        auto trans = element->get_trans();              // behaviour of the state
        std::string new_value = combine_states(states_set);     // merge the values

        std::shared_ptr <auto_state> stays = get_smallest_state(states_set);        // change value in dict
        this->dict.change_state_name(stays->get_value(), new_value);

        stays->set_power_state(trans);          // change the selected state to represent the whole set

        for (const auto& state: states_set){
            if (state != stays){            // if it is not the state that is being kept
                this->dict.remove_state(state->get_value());        // remove state from dict and automata
                state->clear_trans();
                this->state_table[state->get_value()] = nullptr;


                auto search = std::find(this->accept_states.begin(), this->accept_states.end(), state->get_value());
                if (search != this->accept_states.end()){       // remove from accept state as well
                    this->accept_states.erase(search);
                }

                search = std::find(this->init_states.begin(), this->init_states.end(), state->get_value());
                if (search != this->init_states.end()) {
                    this->accept_states.erase(search);          //could be expensive
                }
            }
        }
    }

    unsigned int old_id, new_id;
    while (this->dict.smooth_vector_state(old_id, new_id)){     // smooth the vector after removal of states
        this->state_table[new_id] = this->state_table[old_id];          // change states in the table as well
        this->state_table[new_id]->set_value(static_cast<int> (new_id));        // change the value of the state

        auto search = std::find(this->accept_states.begin(), this->accept_states.end(), old_id);
        if (search != this->accept_states.end()){       //change also is it was an accepting state
            *search = static_cast<int> (new_id);
        }
        search = std::find(this->init_states.begin(), this->init_states.end(), old_id);
        if (search != this->init_states.end()) {            // change if it was an initial state
            *search = static_cast <int> (new_id);          //could be expensive
        }
    }

    this->state_table.resize(this->dict.get_state_size());
}


void min_auto::init_power_set(std::vector <std::shared_ptr<power_element>>& pow_set){
    ptr_state_vector accept_vect;
    for (auto index: this->accept_states){
        accept_vect.push_back(this->state_table[index]);
    }

    auto final = std::make_shared<power_element>(accept_vect);      // final closure
    auto non_final =  std::make_shared<power_element>();
    for (int i = 0; i < this->state_table.size(); i++){         // get non-final closure
        if (std::find(this->accept_states.begin(), this->accept_states.end(), i) == this->accept_states.end()){
            non_final->add_state(this->state_table[i]);
        }
    }
    pow_set.push_back(final);
    pow_set.push_back(non_final);
}


void min_auto::split_power_state(std::vector <std::shared_ptr <power_element>>& previous,
                                 std::vector <std::shared_ptr <power_element>>& next){
    std::vector <ptr_state_vector> help_table(this->state_table.size());
    init_power_hash(help_table, previous);

    for (const auto& closure: previous){
        auto old_set = closure->get_set();
        if (old_set.size() == 1){                           // no need to divide, it is single element
            std::vector <ptr_state_vector> trans;           // update transition for the single closure
            std::shared_ptr <auto_state> only_element = *old_set.begin();
            only_element->get_pow_trans(trans, help_table);         // get its transition

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
