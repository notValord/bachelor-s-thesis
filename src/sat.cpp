#include "sat.h"

sat_stats::sat_stats(int states, int symbols) : automata_stats(states, symbols) {
    ;
}

void sat_stats::determine_clauses() const{
    unsigned int index = 1;
    for (int i = 1; i <= this->alpha_num * this->state_num; i++){
        for (int j = 0; j < this->state_num; j++){
            for (int k = j+1; k < this->state_num; k++){
                std::cout << "-" << index+j << " -" << index + k << " 0\n";     // determinism
            }
        }
        index += this->state_num;
    }
}

void sat_stats::complete_clauses() const{
    unsigned int index = 1;
    for (int i = 1; i <= this->alpha_num * this->state_num; i++){
        for (int j = 0; j < this->state_num; j++){
            std::cout << index+j << " ";             // completeness
        }
        index += this->state_num;
        std::cout << "0\n";
    }
}

void sat_stats::recurse_tseitsen(const std::vector<int>& base, unsigned int state, std::queue <unsigned int> word,
                                      std::vector<int>& result){
    unsigned int symb_index = word.front();
    std::vector<int> addition;
    std::vector<int> tmp;
    word.pop();

    for (int i = 1; i <= this->state_num; i++){
        addition = base;
        addition.push_back(static_cast <int> (symb_index * this->state_num + (state - 1) * this->state_num * this->alpha_num + i));
        addition.push_back(AND);

        if (word.empty()) {
            addition.push_back(static_cast <int> (this->state_num * this->state_num * this->alpha_num + i));
            result.insert(result.end(), addition.begin(), addition.end());
            result.push_back(OR);
        }
        else{
            this->recurse_tseitsen(addition, i, word, result);
        }
    }
}

void sat_stats::recurse_tseitsen_reject(const std::string& base, unsigned int state, std::queue <unsigned int> word){
    unsigned int symb_index = word.front();
    word.pop();
    std::string addition;

    for (int i = 1; i <= this->state_num; i++){
        addition = base;
        addition += std::to_string(symb_index * this->state_num + (state - 1) * this->state_num * this->alpha_num + i) + " -";
        if (word.empty()) {
            addition += std::to_string(this->state_num * this->state_num * this->alpha_num + i) + " 0\n";
            std::cout << addition;
        }
        else{
            this->recurse_tseitsen_reject(addition, i, word);
        }
    }
}

void sat_stats::example_clauses(unsigned int max_index) {
    max_index++;

    for(auto word: this->accept){
        std::vector <int> expression;
        std::vector <int> result;
        if (word.empty()){
            std::cout << this->state_num * this->state_num * this->alpha_num + 1 << " 0\n";
            //special case epsilon
            continue;
        }

        unsigned int start_index = word.front();
        word.pop();

        for (int i = 1; i <= this->state_num; i++){
            expression.clear();
            expression.push_back(static_cast <int> (start_index * this->state_num + i));
            expression.push_back(AND);

            if (word.empty()) { // accepts a single letter
                expression.push_back(static_cast <int> (this->state_num * this->state_num * this->alpha_num + i));
                result.insert(result.end(), expression.begin(), expression.end());
                result.push_back(OR);
            }
            else{
                this->recurse_tseitsen(expression, i, word, result);
            }
        }

        max_index = tseitsen(result, max_index);
    }

    for(auto word: this->reject){
        std::string expression;

        if (word.empty()){
            std::cout << "-" << this->state_num * this->state_num * this->alpha_num + 1 << " 0\n";
            //special case epsilon
            continue;
        }

        unsigned int start_index = word.front();
        word.pop();

        for (int i = 1; i <= this->state_num; i++){
            expression = "-" + std::to_string(start_index * this->state_num + i) + " -";
            if (word.empty()) { // rejects a single letter
                expression += std::to_string(this->state_num * this->state_num * this->alpha_num + i) + " 0\n";
                std::cout << expression;
            }
            else {
                this->recurse_tseitsen_reject(expression, i, word);
            }
        }
    }
}
