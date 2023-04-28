//
// Created by vmvev on 3/19/2023.
//

#include "rezidual_auto.h"

std::shared_ptr <det_auto> rezidual_auto(const std::shared_ptr <det_auto>& nfa){
    auto reverse_nfa = nfa->reverse();
    auto rev_dfa = determine_nfa(reverse_nfa);
    //std::cout << "After rev " << rev_dfa->get_state_number() << std::endl;
    auto rez_prepare = std::static_pointer_cast <rezid_auto> (rev_dfa->reverse());
    return rez_prepare->rezidual();
}

rezid_auto::rezid_auto() : det_auto() {
    ;
}

rezid_auto::rezid_auto(det_auto& base) {
    this->alphabet = base.get_alphabet();
    this->init_states = base.get_init_vec();
    this->accept_states = base.get_acc_vec();
    this->dict = base.get_dict();
    this->state_table = base.get_state_table();
}

void rezid_auto::remove_rezidual_state(const std::string& state_value, std::vector <int>& covering){
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

void rezid_auto::create_rezidual_state(const ptr_state_vector& base, const std::shared_ptr <rezid_auto>& rezid,
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

bool rezid_auto::is_recurse_coverable(int check_index, const std::vector <ptr_state_vector>& states, int start,
                                    const std::vector <int>& covering, std::vector <bool>& checked_out,
                                    const std::shared_ptr <rezid_auto>& rezid){
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

void rezid_auto::is_coverable(int check_index, const std::vector <ptr_state_vector>& states, int start,
                            std::vector <bool>& checked_out, const std::shared_ptr <rezid_auto>& rezid){
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

std::shared_ptr <det_auto> rezid_auto::rezidual(){
    std::shared_ptr <rezid_auto> rezid = std::make_shared <rezid_auto> ();      // create new automata with the same alphabet
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

    //std::cout << "Rezid " << rezid->get_state_number() << std::endl;

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
