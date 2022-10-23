/*
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 18.10 2022
* Subject: Bachelor's thesis
*/

#define INPUT_DIRECTORY "../input_automata/"

#include "vtf_input.h"

void parse_line(const std::string& line, std::vector <std::string>& ret){
    bool skip = true;
    std::string token;
    std::stringstream text(line);

    while (std::getline(text, token, ' ')){
        if (skip){
            skip = false;
            continue;
        }
        ret.push_back(token);
    }
}

void parse_transition(const std::string& line, const std::shared_ptr <automata>& new_auto){
    std::string token;
    std::vector <std::string> split_vector;
    std::stringstream text(line);

    while (std::getline(text, token, ' ')){
        split_vector.push_back(token);
    }
    if (split_vector.size() != 3){
        std::cerr << "Input transition in wrong format" << std::endl;
        return;
    }

    new_auto->add_transition(split_vector[1], split_vector[0], split_vector[2]);
    new_auto->add_alphabet(split_vector[1]);
}

std::shared_ptr <automata> take_input(const std::string& file){
    std::string tmp;
    std::ifstream input(INPUT_DIRECTORY + file);

    if (input.is_open()){
        std::shared_ptr <automata> new_auto(std::make_shared <automata> ());

        while (std::getline(input, tmp)){
            if (tmp.find("States") != std::string::npos){
                std::vector <std::string> split_vector;
                parse_line(tmp, split_vector);
                for (const auto& element: split_vector){
                    new_auto->add_state(element);
                }
            }
            else if (tmp.find("Initial") != std::string::npos){
                std::vector <std::string> split_vector;
                parse_line(tmp, split_vector);
                for (const auto& element: split_vector){
                    new_auto->add_init_state_force(element);
                }
            }
            else if (tmp.find("Final") != std::string::npos){
                std::vector <std::string> split_vector;
                parse_line(tmp, split_vector);
                for (const auto& element: split_vector){
                    new_auto->add_accept_state_force(element);
                }
            }
            else if (tmp.empty() or tmp[0] == '@'){
                continue;
            }
            else {
                parse_transition(tmp, new_auto);
            }
        }
        input.close();
        input.clear();
        return new_auto;
    }
    else{
        std::cerr << "Failed to open an input file" << std::endl;
        return nullptr;
    }
}