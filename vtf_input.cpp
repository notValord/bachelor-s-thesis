//
// Created by vmvev on 10/9/2022.
//

#define INPUT_DIRECTORY "../input_automata/"

#include "vtf_input.h"

std::vector <std::string> parse_line(const std::string& line){
    bool skip = true;
    std::string token;
    std::vector <std::string> split_vector;
    std::stringstream text(line);

    while (std::getline(text, token, ' ')){
        if (skip){
            skip = false;
            continue;
        }
        split_vector.push_back(token);
    }
    return split_vector;
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
        std::shared_ptr <automata> new_auto(new automata());

        while (std::getline(input, tmp)){
            if (tmp.find("States") != std::string::npos){
                auto split_vector = parse_line(tmp);
                for (const auto& element: split_vector){
                    new_auto->add_state(element);
                }
            }
            else if (tmp.find("Initial") != std::string::npos){
                auto split_vector = parse_line(tmp);
                for (const auto& element: split_vector){
                    new_auto->add_init_state_force(element);
                }
            }
            else if (tmp.find("Final") != std::string::npos){
                auto split_vector = parse_line(tmp);
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