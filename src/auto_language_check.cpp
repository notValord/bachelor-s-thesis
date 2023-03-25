//
// Created by vmvev on 3/19/2023.
//

#include "auto_language_check.h"

bool language_intersect(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second){
    std::vector <std::pair <int, int>> state_stack;
    std::set <std::pair <int, int>> gone_through;

    if (not first->same_alphabets(second)){
        return true;
    }

    auto start = std::make_pair(first->get_init(), second->get_init());
    state_stack.push_back(start);
    gone_through.insert(start);

    while(not state_stack.empty()){
        std::pair <int, int> current_pair = state_stack.back();
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
                gone_through.insert(new_pair);
            }
        }
    }
    return false;
}

bool language_equal(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second){
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

bool sat_intersect(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second,
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

bool sat_equal(const std::shared_ptr <det_auto>& sat, const std::shared_ptr <det_auto>& orig,
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

bool chain_part(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second){
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

bool sat_anticahin(const std::shared_ptr <det_auto>& sat, const std::shared_ptr <det_auto>& orig){
    if (chain_part(sat, orig) or chain_part(orig, sat)){
        return false;
    }
    return true;
}
