/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#include "simulation_auto.h"

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

void simulate_min(const std::shared_ptr <simul_auto>& nfa){
    std::vector<std::vector <bool>> simulate_matrix;
    nfa->create_simulate_matrix(simulate_matrix);
    nfa->minimal_sim(simulate_matrix);
}


simul_auto::simul_auto(det_auto& base){
    this->alphabet = base.get_alphabet();
    this->init_states = base.get_init_vec();
    this->accept_states = base.get_acc_vec();
    this->dict = base.get_dict();
    this->state_table = base.get_state_table();
}

void simul_auto::minimal_sim(std::vector<std::vector<bool>>& omega_matrix){
    std::vector <ptr_state_vector> tmp_states;
    std::vector <ptr_state_vector> help_table(omega_matrix.size());

    ptr_state_vector state_vec;
    for (int i = 0; i < omega_matrix.size(); i++){
        state_vec.clear();
        for (int j = i; j < omega_matrix[i].size(); j++){
            if (omega_matrix[i][j]){        // is simulated
                if (i != j){
                    std::fill(omega_matrix[j].begin(), omega_matrix[j].end(), false);   // set the row to false
                }
                state_vec.push_back(this->state_table[j]);
            }
        }
        if (not state_vec.empty()){
            tmp_states.push_back(state_vec);
        }
        for (const auto& element: state_vec){       // set every state to the set it belongs to
            help_table[element->get_value()] = state_vec;
        }
    }

    for (auto& closure: tmp_states){
        if (closure.size() == 1){
            (*closure.begin())->check_simul_trans(help_table);      // check the transitions
        }
        else{
            std::string new_value = combine_states(closure);        // create a single state

            std::shared_ptr <auto_state> stays = *closure.begin();        // change value in dict, always take the first state
            this->dict.change_state_name(stays->get_value(), new_value);

            stays->check_simul_trans(help_table);

            for (const auto& state: closure) {
                if (state != stays) {           // remove all states that aren't kept
                    this->dict.remove_state(state->get_value());        // remove state from dict and automata
                    state->clear_trans();
                    this->state_table[state->get_value()] = nullptr;


                    auto search = std::find(this->accept_states.begin(), this->accept_states.end(), state->get_value());
                    if (search != this->accept_states.end()) {      // remove from accept state
                        this->accept_states.erase(search);
                        if (std::count(this->accept_states.begin(), this->accept_states.end(), stays->get_value()) == 0) {
                            this->accept_states.push_back(stays->get_value());      // if wasn't in accept states then add
                        }
                    }

                    search = std::find(this->init_states.begin(), this->init_states.end(), state->get_value());
                    if (search != this->init_states.end()) {
                        this->init_states.erase(search);          //could be expensive
                        if (std::count(this->init_states.begin(), this->init_states.end(), stays->get_value()) == 0) {
                            this->init_states.push_back(stays->get_value());        // if had initial then add
                        }
                    }
                }
            }
        }
    }

    unsigned int old_id, new_id;
    while (this->dict.smooth_vector_state(old_id, new_id)){     // smooth vector
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

void simul_auto::create_simulate_matrix(std::vector<std::vector<bool>>& ret_matrix){
    unsigned long state_num = this->state_table.size();
    std::vector<std::vector<std::vector<int>>> card_tables(this->alphabet);
    this->init_card_tables(card_tables);

    auto rev_auto = this->reverse();

    std::vector<bool> row(state_num, true);
    std::vector<std::vector<bool>>omega_matrix(state_num, row);
    std::vector <std::pair <int, int>> gone_stack;
    this->init_omega_matrix(omega_matrix, gone_stack);

    std::shared_ptr <auto_state> tmp_state = nullptr;

    while (not gone_stack.empty()){
        auto curr_pair = gone_stack.back();     // get the state
        gone_stack.pop_back();

        for (int symbol = 0; symbol < this->alphabet; symbol++){
            tmp_state = rev_auto->get_state(curr_pair.second);      // get previous state
            auto trans_rev_k = tmp_state->get_trans_row(symbol);
            if (trans_rev_k == nullptr){
                continue;
            }
            std::vector<std::vector<int>>& curr_card_table = card_tables[symbol];

            for (const auto& k: *trans_rev_k){
                int k_index = k->get_value();
                curr_card_table[k_index][curr_pair.first]--;        // remove transition

                if (curr_card_table[k_index][curr_pair.first] == 0){
                    tmp_state = rev_auto->get_state(curr_pair.first);
                    auto trans_remove = tmp_state->get_trans_row(symbol);
                    if (trans_remove == nullptr){
                        continue;
                    }

                    for (const auto& remove_state: *trans_remove){
                        int remove_index = remove_state->get_value();

                        if (omega_matrix[remove_index][k_index]){
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


void simul_auto::init_card_tables(std::vector <std::vector <std::vector<int>>>& card_tables){
    int state_num = static_cast <int> (this->state_table.size());

    for (int i = 0; i < this->alphabet; i++) {
        std::vector <std::vector <int>> column(state_num);

        for (int j = 0; j < state_num; j++){
            int card = this->state_table[j]->get_trans_card(i);     // get number of transitions

            std::vector <int> row(state_num);
            std::fill(row.begin(), row.end(), card);

            column[j] = std::move(row);
        }
        card_tables[i] = std::move(column);
    }
}


void simul_auto::init_omega_matrix(std::vector<std::vector<bool>>& omega_matrix,
                                 std::vector <std::pair <int, int>>& gone_stack){
    for (int i = 0; i < this->state_table.size(); i++){
        if (std::count(this->accept_states.begin(), this->accept_states.end(), i) == 0){
            for (auto fin: this->accept_states){        // final and non-final pairs
                if (not omega_matrix[fin][i]){
                    continue;
                }
                omega_matrix[fin][i] = false;
                gone_stack.emplace_back(fin, i);
            }
        }

        for (int j = 0; j < this->state_table.size(); j++){     // checking for transitions
            if (omega_matrix[i][j] && this->state_table[j]->not_under_simulate(this->state_table[i])){
                omega_matrix[i][j] = false;
                gone_stack.emplace_back(i, j);
            }
        }
    }
}