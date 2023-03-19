/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 18.10 2022
* Subject: Bachelor's thesis
*/

#define INPUT_DIRECTORY "../input_automata/1/"

#include "vtf_input.h"


void parse_line(const std::string& line, std::vector <std::string>& ret){
    bool skip = true;                       // skipping the first word
    std::string token;
    std::stringstream text(line);

    while (std::getline(text, token, ' ')){
        if (skip){
            skip = false;
            continue;
        }
        ret.push_back(token);       // pushing words into vector
    }
}

void parse_transition(const std::string& line, const std::shared_ptr <automata>& new_auto){
    std::string token;
    std::vector <std::string> split_vector;
    std::stringstream text(line);

    while (std::getline(text, token, ' ')){
        split_vector.push_back(token);          // get vector of transition
    }

    if (split_vector.size() != 3){
        std::cerr << "Input transition in wrong format" << std::endl;
        return;
    }

    new_auto->add_alphabet(split_vector[1]);        // add symbol to alphabet
    new_auto->add_transition(split_vector[1], split_vector[0], split_vector[2]);
}

std::shared_ptr <automata> take_input(const std::string& file){
    std::string tmp;
    std::ifstream input(file);

    if (input.is_open()){
        std::shared_ptr <automata> new_auto(std::make_shared <automata> ());        // empty new automata

        std::vector <std::string> split_vector;
        while (std::getline(input, tmp)){
            if (tmp.find("States") != std::string::npos){
                split_vector.clear();
                parse_line(tmp, split_vector);

                for (const auto& element: split_vector){
                    new_auto->add_state(element);                   // adding states
                }
            }
            else if (tmp.find("Initial") != std::string::npos){
                split_vector.clear();
                parse_line(tmp, split_vector);

                for (const auto& element: split_vector){
                    new_auto->add_init_state_force(element);        // create and add initial states
                }
            }
            else if (tmp.find("Final") != std::string::npos){
                split_vector.clear();
                parse_line(tmp, split_vector);

                for (const auto& element: split_vector){
                    new_auto->add_accept_state_force(element);      // create and add final states
                }
            }
            else if (tmp.empty() or tmp[0] == '@'){                 // skip comments and empty lines
                continue;
            }
            else {
                parse_transition(tmp, new_auto);                    // create transitions
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