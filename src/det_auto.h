//
// Created by vmvev on 3/20/2023.
//

#ifndef BAKALARKA_DET_AUTO_H
#define BAKALARKA_DET_AUTO_H

#include "automata_base.h"
#include "auto_stats.h"

class det_auto;
class rezid_auto;
class power_element;

std::shared_ptr <det_auto> determine_nfa(const std::shared_ptr <det_auto>&);

class det_auto : public automata{
public:
    det_auto(const std::set <std::string>& states, const std::set <std::string>& alphabet,
             const std::vector <std::array<std::string, 3>>& trans,
             const std::set <std::string>& init_states, const std::set <std::string>& fin_states);

    // Constructor of the automata - takes only the alphabet and one state
    det_auto(const std::string& state, const std::vector <std::string>& alphabet);
    det_auto(int alphabet, auto_dictionary& dict);
    det_auto();

    std::shared_ptr <det_auto> copy();

    std::shared_ptr <det_auto> reverse();

    void reverse_accept_states();
    std::shared_ptr <det_auto> complement();


    std::string combine_states(ptr_state_vector states);

    void determine_state(det_auto* old, std::vector <ptr_state_vector>& set_queue,
                         const ptr_state_vector& current_set, int current_state);
    void recurse_find_all_eps(const ptr_state_vector& search_group, int eps_index, ptr_state_vector& ret);
    void remove_eps_transitions();
    std::shared_ptr <det_auto> determine();

    void find_examples(const std::shared_ptr<automata_stats>& stats);
};
#endif //BAKALARKA_DET_AUTO_H
