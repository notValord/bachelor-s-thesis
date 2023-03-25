//
// Created by vmvev on 3/19/2023.
//

#ifndef BAKALARKA_MIN_DET_AUTO_H
#define BAKALARKA_MIN_DET_AUTO_H

#include "det_auto.h"

class power_element;
class min_auto;

std::shared_ptr <det_auto> det_n_min(const std::shared_ptr <det_auto>& nfa);

std::shared_ptr <det_auto> brzozowski(const std::shared_ptr <det_auto>& nfa);

void minimal_dfa(const std::shared_ptr <min_auto>&);

void insert_pow_set(const std::shared_ptr <power_element>& adding,
                    std::vector <std::shared_ptr<power_element>>& power_set);

void init_power_hash(std::vector <ptr_state_vector>& help_table,
                     const std::vector <std::shared_ptr<power_element>>& previous);

/// Checks whether two vectors of pointers to power element are equal
bool is_eq(std::vector <std::shared_ptr <power_element>>&, std::vector <std::shared_ptr <power_element>>&);

class power_element{
private:
    ptr_state_vector same_set;      //todo pointre/referencie
    std::vector <ptr_state_vector> transition;

public:
    friend bool operator==(const power_element& first, const power_element& second){
        if (first.same_set == second.same_set){
            return true;
        }
        return false;
    }

    friend bool operator!=(const power_element& first, const power_element& second){
        if (first.same_set != second.same_set){
            return true;
        }
        return false;
    }

    power_element(const std::shared_ptr <auto_state>& state, std::vector<ptr_state_vector>& trans);

    explicit power_element(ptr_state_vector set);

    power_element();

    ptr_state_vector& get_set();
    std::vector<ptr_state_vector>& get_trans();

    void add_state(const std::shared_ptr <auto_state>& state);
    void set_trans(const std::vector<ptr_state_vector>& new_trans);
};

class min_auto : public det_auto {
public:
    min_auto(det_auto& base);

    void min_power(const std::vector <std::shared_ptr<power_element>>& power_set);
    void init_power_set(std::vector <std::shared_ptr<power_element>>& pow_set);
    void split_power_state(std::vector <std::shared_ptr <power_element>>& previous,
                           std::vector <std::shared_ptr <power_element>>& next);
};

#endif //BAKALARKA_MIN_DET_AUTO_H
