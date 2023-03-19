#ifndef BAKALARKA_QBF_H
#define BAKALARKA_QBF_H

#include <cmath>

#include "tseitsen.h"
#include "auto_stats.h"

using uint = unsigned int;

void recurse_init_final(const std::string& expr, uint var_base, uint& result_base, uint iter, uint max);

class qbf_stats : public automata_stats {
    uint state_bin;

public:
    qbf_stats(int states, int symbols);
    [[nodiscard]] unsigned int get_bin() const;

    void init_final_clauses(uint state_base, uint end_base);
    void init_final_clauses_reject(uint state_base, uint end_base, std::vector <int>& result);

    void valid_combinations(uint start);
    void valid_combinations_reject(uint start, std::vector <int>& input);

    void recurse_accept(const std::string& expr, uint var_base, uint& trans_base, uint iter, uint& cnt, bool end=false);
    void accept_clauses(uint var, uint trans);

    void recurse_reject(const std::vector <int>& base, uint var_base, uint& trans_base, uint iter, uint& cnt,
                                   std::vector <int>& result, bool end=false);
    void reject_clauses(uint var, uint trans, std::vector <int>& result);

    void example_clauses(uint tsei_start);
};

#endif //BAKALARKA_QBF_H
