//
// Created by vmvev on 3/19/2023.
//

#ifndef BAKALARKA_AUTO_LANGUAGE_CHECK_H
#define BAKALARKA_AUTO_LANGUAGE_CHECK_H

#include "det_auto.h"

bool language_intersect(const std::shared_ptr <det_auto>&, const std::shared_ptr <det_auto>&);

/// Checks whether two given automata have equal languages through determination and complements
bool language_equal(const std::shared_ptr <det_auto>&, const std::shared_ptr <det_auto>&);


bool sat_intersect(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second,
                   std::vector <std::queue<unsigned int>>* add);
bool sat_equal(const std::shared_ptr <det_auto>& sat, const std::shared_ptr <det_auto>& orig,
               const std::shared_ptr <automata_stats>& output);

bool check_product(std::vector <std::pair <int, std::set <int>>>& covering, int state, std::set <int>& product);
bool chain_part(const std::shared_ptr <det_auto>& first, const std::shared_ptr <det_auto>& second);
bool sat_anticahin(const std::shared_ptr <det_auto>& sat, const std::shared_ptr <det_auto>& orig);

#endif //BAKALARKA_AUTO_LANGUAGE_CHECK_H
