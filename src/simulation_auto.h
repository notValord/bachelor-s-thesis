//
// Created by vmvev on 3/19/2023.
//

#ifndef BAKALARKA_SIMULATION_AUTO_H
#define BAKALARKA_SIMULATION_AUTO_H

#include "det_auto.h"

class simul_auto;

void symetric_fragment(std::vector<std::vector<bool>>& omega_matrix);

void simulate_min(const std::shared_ptr <simul_auto>& nfa);

class simul_auto: public det_auto{
public:
    simul_auto(det_auto& base);      //convert constructor

    void minimal_sim(std::vector<std::vector<bool>>& omega_matrix);
    void create_simulate_matrix(std::vector<std::vector<bool>>& omega_matrix);
    void init_card_tables(std::vector <std::vector <std::vector<int>>>& card_tables);
    void init_omega_matrix(std::vector<std::vector<bool>>&, std::vector <std::pair <int, int>>&);
};

#endif //BAKALARKA_SIMULATION_AUTO_H
