//
// Created by vmvev on 3/19/2023.
//

#ifndef BAKALARKA_REZIDUAL_AUTO_H
#define BAKALARKA_REZIDUAL_AUTO_H

#include "det_auto.h"

std::shared_ptr <det_auto> rezidual_auto(const std::shared_ptr <det_auto>& nfa);

class rezid_auto : public det_auto{
public:
    rezid_auto();
    rezid_auto(det_auto& base);      //convert constructor

    void remove_rezidual_state(const std::string& state_value, std::vector <int>& covering);
    void create_rezidual_state(const ptr_state_vector & base, const std::shared_ptr <rezid_auto>& rezid,
                               int base_value, std::vector <ptr_state_vector>& all_states);

    bool is_recurse_coverable(int check_index, const std::vector <ptr_state_vector>& states, int start,
                              const std::vector <int>& covering, std::vector <bool>& checked_out,
                              const std::shared_ptr <rezid_auto>& rezid);

    void is_coverable(int check_index, const std::vector <ptr_state_vector>& states, int start,
                      std::vector <bool>& checked_out, const std::shared_ptr<rezid_auto>& rezid);

    std::shared_ptr <det_auto> rezidual();
};


#endif //BAKALARKA_REZIDUAL_AUTO_H
