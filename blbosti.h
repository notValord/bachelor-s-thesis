//
// Created by vmvev on 10/9/2022.
//

#ifndef BAKALARKA_BLBOSTI_H
#define BAKALARKA_BLBOSTI_H

#if 0
#include <iostream>
#include <set>
#include <unordered_map>

class auto_state {
private:
    std::string value;
    std::unordered_map<std::string, std::set<auto_state *>> transitions;

public:
    // Constructor of tha state needing its value
    explicit auto_state(const std::string &value) {
        this->value = value;
    }

    // Method print - prints the value and all transitions of the state
    void print() {
        std::cout << "State value" << std::endl;
        std::cout << this->value << std::endl;

        std::cout << "Transitions:" << std::endl;
        for (const auto &element: this->transitions) {
            std::cout << element.first << " -> ";
            for (const auto &elem: element.second) {
                std::cout << elem->value << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    void add_transition(const std::string& symbol, auto_state* trans_to){
        auto search = this->transitions.find(symbol);
        if (search == this->transitions.end()){
            std::set <auto_state*> new_sett;
            new_sett.insert(trans_to);
            //auto new_set = new std::set <auto_state*>;
            //new_set->insert(trans_to);
            this->transitions[symbol] = new_sett;
        }
        else{
            if (search->second.count(trans_to)){
                std::cerr << "Trying to add a duplicate transition in add_transition" << std::endl;
                return;
            }
            search->second.insert(trans_to);
        }
    }
};
#endif
#endif //BAKALARKA_BLBOSTI_H
