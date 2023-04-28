/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 16.04.2023
* Subject: Bachelor's thesis - 2st part
*/
#include "sat.h"

sat_stats::sat_stats(int states, int symbols) : automata_stats(states, symbols) {
    ;
}

void sat_stats::determine_clauses() const{
    for (int index = 1; index <= this->alpha_num * this->state_num * this->state_num; index += this->state_num){   // every row
        for (int j = 0; j < this->state_num; j++){                  // elements in row
            for (int k = j+1; k < this->state_num; k++){            // combinations
                std::cout << "-" << index + j << " -" << index + k << " 0\n";     // determinism
            }
        }
    }
}

void sat_stats::complete_clauses() const{
    for (int index = 1; index <= this->alpha_num * this->state_num * this->state_num; index += this->state_num){   // every row
        for (int j = 0; j < this->state_num; j++){              // elements in a row
            std::cout << index+j << " ";             // completeness
        }
        std::cout << "0\n";
    }
}

void sat_stats::recurse_tseitsen(const std::vector<int>& base, unsigned int state, std::queue <unsigned int> word,
                                      std::vector<int>& result){
    unsigned int symb_index = word.front();
    word.pop();
    std::vector<int> addition;

    for (int i = 1; i <= this->state_num; i++){     // for variable in a row
        addition = base;
        addition.push_back(static_cast <int> (symb_index * this->state_num * this->state_num +
                                                (state - 1) * this->state_num + i));
        addition.push_back(AND);

        if (word.empty()) {     // end of the word
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
        addition += std::to_string(symb_index * this->state_num * this->state_num +
                                        (state - 1) * this->state_num + i) + " -";
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
    for(auto word: this->accept){
        std::vector <int> expression;       // partial expression
        std::vector <int> result;           // output for tseitin
        if (word.empty()){
            std::cout << this->state_num * this->state_num * this->alpha_num + 1 << " 0\n";
            //special case epsilon, set state 1 to be final
            continue;
        }

        unsigned int start_index = word.front();
        word.pop();

        for (int i = 1; i <= this->state_num; i++){
            expression.clear();
            expression.push_back(static_cast <int> (start_index * this->state_num * this->state_num + i));
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

        /*
        //debug info
        std::cerr << "Result: ";
        for (auto elem: result){
            std::cerr << elem << " ";
        }
        std::cerr << std::endl;
         */

        max_index = dynamic_tseitsen(result, max_index);
    }

    for(auto word: this->reject){
        std::string expression;

        if (word.empty()){
            std::cout << "-" << this->state_num * this->state_num * this->alpha_num + 1 << " 0\n";
            //special case epsilon, state 1 cannot be final
            continue;
        }

        unsigned int start_index = word.front();
        word.pop();

        for (int i = 1; i <= this->state_num; i++){
            expression = "-" + std::to_string(start_index * this->state_num * this->state_num + i) + " -";
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

void sat_stats::recurse_nfa_tseitsen(const std::vector<int>& base, unsigned int state, std::queue <unsigned int> word,
                                 std::vector<int>& result){
    unsigned int symb_index = word.front();
    word.pop();
    std::vector<int> addition;

    for (int i = 1; i <= this->state_num; i++){     // for variable in a row
        addition = base;
        addition.push_back(static_cast <int> (symb_index * this->state_num * this->state_num +
                                              (state - 1) * this->state_num + i));
        addition.push_back(AND);

        if (word.empty()) {     // end of the word
            addition.push_back(static_cast <int> (this->state_num * this->state_num * this->alpha_num + this->state_num + i));
            result.insert(result.end(), addition.begin(), addition.end());
            result.push_back(OR);
        }
        else{
            this->recurse_nfa_tseitsen(addition, i, word, result);
        }
    }
}

void sat_stats::recurse_nfa_tseitsen_reject(const std::string& base, unsigned int state, std::queue <unsigned int> word){
    unsigned int symb_index = word.front();
    word.pop();
    std::string addition;

    for (int i = 1; i <= this->state_num; i++){
        addition = base;
        addition += std::to_string(symb_index * this->state_num * this->state_num +
                                   (state - 1) * this->state_num + i) + " -";
        if (word.empty()) {
            addition += std::to_string(this->state_num * this->state_num * this->alpha_num + this->state_num + i) + " 0\n";
            std::cout << addition;
        }
        else{
            this->recurse_nfa_tseitsen_reject(addition, i, word);
        }
    }
}

void sat_stats::example_nfa_clauses(unsigned int max_index) {
    std::cout << this->state_num * this->state_num * this->alpha_num + 1 << " 0\n"; // set initial state 1

    for(auto word: this->accept) {
        std::vector<int> expression;
        std::vector<int> result;
        if (word.empty()) {
            std::cout << this->state_num * this->state_num * this->alpha_num + state_num + 1 << " 0\n";
            //special case epsilon, set state 1 to be final state
            continue;
        }

        unsigned int start_index = word.front();
        word.pop();

        for (int j = 0; j < this->state_num; j++) {
            for (int i = 1; i <= this->state_num; i++) {
                expression.clear();
                //set init
                expression.push_back(static_cast <int> (this->state_num * this->state_num * this->alpha_num + j + 1));
                expression.push_back(AND);
                expression.push_back(static_cast <int> (start_index * this->state_num * this->state_num + j * this->state_num + i));
                expression.push_back(AND);

                if (word.empty()) { // accepts a single letter
                    expression.push_back(static_cast <int> (this->state_num * this->state_num * this->alpha_num + this->state_num + i));
                    result.insert(result.end(), expression.begin(), expression.end());
                    result.push_back(OR);
                } else {
                    this->recurse_nfa_tseitsen(expression, i, word, result);
                }
            }
        }
        /*
        //debug info
        std::cerr << "Result: ";
        for (auto elem: result){
            std::cerr << elem << " ";
        }
        std::cerr << std::endl;
        */

        max_index = dynamic_tseitsen(result, max_index);
    }

    for(auto word: this->reject){
        std::string expression;

        if (word.empty()){
            for (int i = 1; i <= this->state_num; i++){
                std::cout << "-" << this->state_num * this->state_num * this->alpha_num + i <<
                            " -" << this->state_num * this->state_num * this->alpha_num + this->state_num + i << " 0\n";
            }
            //special case epsilon
            continue;
        }

        unsigned int start_index = word.front();
        word.pop();

        for (int j = 0; j < this->state_num; j++) {
            for (int i = 1; i <= this->state_num; i++) {
                expression = "-" + std::to_string(this->state_num * this->state_num * this->alpha_num + j + 1);
                expression += " -" + std::to_string(start_index * this->state_num * this->state_num + j * this->state_num + i) + " -";
                if (word.empty()) { // rejects a single letter
                    expression += std::to_string(this->state_num * this->state_num * this->alpha_num + this->state_num + i) + " 0\n";
                    std::cout << expression;
                } else {
                    this->recurse_nfa_tseitsen_reject(expression, i, word);
                }
            }
        }
    }
}
