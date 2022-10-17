/*
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 9.10 2022
* Subject: Bachelor's thesis
*/

#include "automata.h"
#include "vtf_input.h"
#include <iostream>
#include <ctime>

void debug(){
    /**
   std::set <std::string> states = {"0", "1", "2", "3"};
   std::set <std::string> alphabet = {"a", "b", "eps"};
   std::vector <std::array<std::string, 3>> trans = {{"0", "eps", "1"},
                                                     {"0", "b", "2"},
                                                     {"1", "a", "3"},
                                                     {"1", "b", "2"},
                                                     {"1", "eps", "2"},
                                                     {"2", "b", "3"}};

   std::set <std::string> init_states = {"0"};
   std::set <std::string> fin_states = {"2"};
   automata pokus1(states, alphabet, trans, init_states, fin_states);

   std::set <std::string> state = {"0", "1", "2"};
   std::set <std::string> alphabe = {"a", "b", "eps"};
   std::vector <std::array<std::string, 3>> tran = {{"0", "eps", "1"},
                                                    {"0", "b", "2"},
                                                    {"1", "b", "2"},
                                                    {"1", "eps", "2"}};

   std::set <std::string> init_state = {"0"};
   std::set <std::string> fin_state = {"2"};
   automata pokus2(state, alphabe, tran, init_state, fin_state);**/
    //auto rev = pokus1.reverse();
    //rev.print();
    //pokus1.print();
    //determine_nfa(pokus1);
}

int parse_args(int argc, char* argv[], std::string& type, std::string& file){
    if (argc == 3){
        type = argv[2];
        file = "";
    }
    else if (argc == 4){
        type = argv[2];
        file = argv[3];
    }
    else{
        std::cerr << "Wrong input format\n" <<
                     "Usage: ./automata {-t min_det}  [--file]\n" <<
                     "\t -t - sets the type of reduction to be done on automata (min_det)\n" <<
                     "\t --file - file with input automata on which the reduction will be done" <<
                     "\t        - if not specified, the reduction will be done on"
                     " every armcNFA_inclTest_X.vtf automata in the input directory\n";
        return 2;
    }
    return 0;
}

int run_reduction(const std::string& input_file){
    auto input_automata = take_input(input_file);
    if (input_automata == nullptr) {
        return -1;
    }
    auto time_bef_d = clock();
    auto dfa = det_n_min(input_automata);
    auto time_aft_d = clock();

    bool is_equal = language_equal(input_automata, dfa);
    std::cout << std::fixed << input_file << " " << input_automata->get_state_number() << " "
              << dfa->get_state_number() << " " << (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC <<
              " " << is_equal << " " << (float) (clock() - time_aft_d) / CLOCKS_PER_SEC << std::endl;
    return 0;
}

void run_all(){
    std::string base = "armcNFA_inclTest_";
    int index = 0;
    int error = 0;
    while (not error) {
        std::string file = base + std::to_string(index)+ ".vtf";
        error = run_reduction(file);
        index++;
    }
}

//usage: ./automata {-t min_det}  [--file]
int main(int argc, char* argv[]) {
    std::string arg_type, arg_file;
    parse_args(argc, argv, arg_type, arg_file);

    if (arg_file.empty()){
        run_all();
    }
    else{
        if (run_reduction(arg_file)){
            std::cerr << "Couldn't parse automata form the input file\n";
        }
    }
    return 0;
}
