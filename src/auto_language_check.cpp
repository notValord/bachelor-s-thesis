/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#include "auto_language_check.h"

bool language_intersect(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second){
    std::vector <std::pair <int, int>> state_stack;         // states to go through
    std::set <std::pair <int, int>> gone_through;           // already visited states

    if (not first->same_alphabets(second)){
        return true;
    }

    auto start = std::make_pair(first->get_init(), second->get_init());
    state_stack.push_back(start);
    gone_through.insert(start);

    while(not state_stack.empty()){
        std::pair <int, int> current_pair = state_stack.back();     // get state
        state_stack.pop_back();

        if (first->is_final(current_pair.first) and second->is_final(current_pair.second)){
            //std::cout << "FOUND WORD "<< std::endl;     // found counter example
            return true;
        }

        for (int i = 0; i < first->get_alphabet(); i++){        // get all next states
            auto new_pair = std::make_pair(first->get_next_state(i, current_pair.first),
                                           second->get_next_state(i, current_pair.second));

            if (gone_through.find(new_pair) == gone_through.end()){     // if wasn't visited, add to stack
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
    auto first_comple = dfa_first->complement();        // get complements
    auto second_comple = dfa_second->complement();

    if (language_intersect(dfa_first, second_comple) or
        language_intersect(first_comple, dfa_second)){      // check the intersection of languages
        return false;
    }
    return true;
}

bool sat_intersect(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second,
                   std::vector <std::queue<unsigned int>>* add){
    std::vector <std::queue<unsigned int>> word_stack;      // words
    std::vector <std::pair <int, int>> state_stack;         // states to go through
    std::set <std::pair <int, int>> gone_through;           // already visited states

    auto start = std::make_pair(first->get_init(), second->get_init());
    state_stack.push_back(start);
    word_stack.emplace_back(std::queue <unsigned int>());
    gone_through.insert(start);

    std::queue <unsigned int> current_word;
    std::pair <int, int> current_pair;
    std::queue <unsigned int> tmp_word;
    bool result = false;
    unsigned int cnt = 0;       // counter for finding words as counter examples
    uint max_len = 0;           // max length of the word to find

    while(not state_stack.empty()){
        current_word = word_stack.back();       // get word
        current_pair = state_stack.back();      // get state
        word_stack.pop_back();
        state_stack.pop_back();

        if (max_len && current_word.size() > max_len){      // too long word, ignore
            continue;
        }

        if (first->is_final(current_pair.first) and second->is_final(current_pair.second)){
            add->push_back(current_word);       // found counter example
            cnt++;
            result = true;
            if (max_len == 0){      // set max length to the found word
                max_len = current_word.size();
            }
        }

        if (cnt > 2){       // find max 3 exmaples
            break;
        }

        for (int i = 0; i < first->get_alphabet(); i++){
            auto new_pair = std::make_pair(first->get_next_state(i, current_pair.first),
                                           second->get_next_state(i, current_pair.second));

            if (gone_through.find(new_pair) == gone_through.end()){     // get all next states
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
    auto sat_comple = dfa_sat->complement();        // get complements
    auto orig_comple = dfa_orig->complement();

    if (sat_intersect(dfa_sat, orig_comple, output->get_reject()) or
        sat_intersect(sat_comple, dfa_orig, output->get_accept())){         // check the intersection of the languages
        return false;
    }
    return true;
}


bool check_product(std::vector <std::pair <int, std::set <int>>>& covering, int state, std::set <int>& product){
    bool result = true;
    for (int i = 0; i < covering.size(); i++){
        if (covering[i].first != state){    // if the product state doesn't have the same state then continue
            continue;
        }
        if (covering[i].second == product){     // if has the same state and product end resolution
            return false;
        }
        if (std::includes(covering[i].second.begin(), covering[i].second.end(), product.begin(), product.end())){
            // if the product is in the covering vec, remove covering vec, the product will be added when true returned
            covering.erase(covering.begin()+i);
            i--;
            continue;
        }
        if (std::includes(product.begin(), product.end(), covering[i].second.begin(), covering[i].second.end())){
            result = false;         // if the covering vector is in the production end resolution
        }
    }
    return result;
}

bool chain_part(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second){
    std::vector <std::pair <int, std::set <int>>> state_stack;          // states to go through
    std::vector <std::pair <int, std::set <int>>> gone_through;         // already visited states

    auto init_vec = first->get_init_vec();
    auto join_init = second->get_init_vec();
    std::set <int> joined(join_init.begin(), join_init.end());

    std::pair <int, std::set <int>> new_pair;

    for (auto init_state: init_vec){        // get all pairs of state : set_of_states
        new_pair = std::make_pair(init_state, joined);

        state_stack.push_back(new_pair);
        gone_through.push_back(new_pair);
    }

    std::pair <int, std::set <int>> current_pair;
    std::set <int> product_set;

    while(not state_stack.empty()){
        current_pair = state_stack.back();      // get pair
        state_stack.pop_back();

        if (first->is_final(current_pair.first) and not second->is_product_final(current_pair.second)){
            //std::cout << "FOUND WRONG\n";         // found conter example
            return true;
        }

        for (int i = 0; i < first->get_alphabet(); i++){
            product_set.clear();
            auto first_row = first->get_next_vec(i, current_pair.first);    // get next states
            for (auto state: current_pair.second){      // get next product
                auto insert_row = second->get_next_vec(i, state);
                product_set.insert(insert_row.begin(), insert_row.end());
            }

            for (auto state: first_row){
                if (check_product(gone_through, state, product_set)){       // check product state if add
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
    if (chain_part(sat, orig) or chain_part(orig, sat)){        // check the intersection of languages of automata
        return false;
    }
    return true;
}
