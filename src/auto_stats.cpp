/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 15.04.2023
* Subject: Bachelor's thesis - 2st part
*/

#include "auto_stats.h"

automata_stats::automata_stats(int states, int symbols) {
    this->state_num = states;
    this->alpha_num = symbols;
}

automata_stats::automata_stats() {
    ;
}

unsigned int automata_stats::get_states() const{
    return this->state_num;
}

unsigned int automata_stats::get_symbols() const{
    return this->alpha_num;
}

std::vector <std::queue<unsigned int>>* automata_stats::get_accept(){
    return &this->accept;
}

std::vector <std::queue<unsigned int>>* automata_stats::get_reject(){
    return &this->reject;
}

void automata_stats::set_symbols(int symbols){
    this->alpha_num = symbols;
}

void automata_stats::add_accept(std::queue <unsigned int>& word){
    this->accept.push_back(word);
}

void automata_stats::add_reject(std::queue <unsigned int>& word){
    this->reject.push_back(word);
}

// warning: not checking whether the format is correct
void automata_stats::add_words_args(int start, int argc, char* argv[]){
    if (strcmp(argv[start],"-A") == 0){     // words to accept
        for (start++; start < argc; start++){
            if (strcmp(argv[start],"-R") == 0)      // words to reject
                break;

            std::string token;
            std::queue <unsigned int> split_queue;
            std::stringstream word(argv[start]);

            while (std::getline(word, token, ' ')){     // split words by space
                split_queue.push(std::stoi(token));          // get vector of transition
            }
            this->accept.push_back(split_queue);        // add into the vector
        }
    }

    if (start == argc){     // no reject words
        return;
    }

    if (strcmp(argv[start],"-R") == 0){
        for (start++; start < argc; start++){
            std::string token;
            std::queue <unsigned int> split_queue;
            std::stringstream word(argv[start]);

            while (std::getline(word, token, ' ')){
                split_queue.push(std::stoi(token));          // get vector of transition
            }
            this->reject.push_back(split_queue);
        }
    }
}

void automata_stats::clear(){
    this->state_num = 0;
    this->alpha_num = 0;
    this->accept.clear();
    this->reject.clear();
}

void automata_stats::print(){
    std::cerr << "States = " << this->state_num << ", Alphabet = " << this->alpha_num << std::endl;
    std::cerr << "Accept:" << std::endl;
    for (const auto& word: this->accept){
        std::queue <unsigned int> tmp = word;       // each queue must be copied to avoid changing the member
        for (int i = 0; i < word.size(); i++){
            std::cerr << tmp.front() << " ";
            tmp.pop();
        }
        std::cerr << std::endl;
    }

    std::cerr << "Reject:" << std::endl;
    for (const auto& word: this->reject){
        std::queue <unsigned int> tmp = word;
        for (int i = 0; i < word.size(); i++){
            std::cerr << tmp.front() << " ";
            tmp.pop();
        }
        std::cerr << std::endl;
    }
    std::cerr << "---------------------------"<< std::endl;
}

void automata_stats::print_words(){
    // each symbol of the words is separated by space
    // words are seperated by commas
    // accept and reject words are separated by semicolon
    for (auto& word: this->accept){
        while (not word.empty()){
            std::cout << word.front();
            word.pop();

            if (not word.empty()){
                std::cout << " ";
            }
        }

        if (word == this->accept.back()){
            ;
        }
        else{
            std::cout << ",";
        }
    }
    std::cout << ";";

    for (auto& word: this->reject){
        while (not word.empty()){
            std::cout << word.front();
            word.pop();

            if (not word.empty()){
                std::cout << " ";
            }
        }
        if (word == this->reject.back()){
            ;
        }
        else{
            std::cout << ",";
        }
    }
}
