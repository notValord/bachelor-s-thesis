#ifndef BAKALARKA_SAT_H
#define BAKALARKA_SAT_H

#include "auto_stats.h"
#include "tseitsen.h"

class sat_stats : public automata_stats{
    public:
        sat_stats(int state, int symbols);
        void determine_clauses() const;
        void complete_clauses() const;
        void example_clauses(unsigned int max_index);
        void recurse_tseitsen(const std::vector<int>& base, unsigned int state, std::queue <unsigned int> word,
                              std::vector<int>& result);
        void recurse_tseitsen_reject(const std::string& base, unsigned int state, std::queue <unsigned int> word);
};
#endif //BAKALARKA_SAT_H
