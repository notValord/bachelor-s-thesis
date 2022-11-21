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
    /*
    std::set <std::string> states = {"s0", "s1", "s2", "s3"};
    std::set <std::string> alphabet = {"a", "b"};
    std::vector <std::array<std::string, 3>> trans = {{"s0", "a", "s1"},
                                                      {"s0", "b", "s2"},
                                                      {"s1", "a", "s3"},
                                                      {"s2", "a", "s3"}};

    std::set <std::string> init_states = {"s0"};
    std::set <std::string> fin_states = {"s3"};*/

    /*
   std::set <std::string> states = {"s0", "s1", "s2", "s3"};
   std::set <std::string> alphabet = {"a", "b", "eps"};
   std::vector <std::array<std::string, 3>> trans = {{"s0", "eps", "s1"},
                                                     {"s0", "b", "s2"},
                                                     {"s1", "a", "s3"},
                                                     {"s1", "b", "s2"},
                                                     {"s1", "eps", "s2"},
                                                     {"s2", "b", "s3"}};

   std::set <std::string> init_states = {"s0"};
   std::set <std::string> fin_states = {"s3"};*/
    //std::shared_ptr<automata> pokus1(std::make_shared<automata> (states, alphabet, trans, init_states, fin_states));


   std::set <std::string> state = {"s1", "s2", "s3", "s4"};
   std::set <std::string> alphabe = {"a", "b"};
   std::vector <std::array<std::string, 3>> tran = {{"s1", "b", "s4"},
                                                    {"s1", "a", "s3"},
                                                    {"s4", "a", "s2"},
                                                    {"s4", "a", "s3"},
                                                    {"s3", "a", "s3"},
                                                    {"s3", "a", "s2"},
                                                    {"s2", "a", "s2"},
                                                    {"s2", "a", "s1"}};

   std::set <std::string> init_state = {"s1"};
   std::set <std::string> fin_state = {"s2", "s3"};
   std::shared_ptr<automata> pokus3(std::make_shared<automata> (state, alphabe, tran, init_state, fin_state));

    simulate_min(pokus3);
    pokus3->print();
    //auto rev = pokus3->reverse();
    //rev->print();

    //auto pokus2 = pokus1->determine();
    //pokus2->print();

    /*
    minimal_dfa(pokus2);
    pokus1->print();
    pokus2->print();

    if (language_equal(pokus2, pokus3)){
        printf("Same auotmata\n");
    }
    else{
        printf("JE TO INE\n");
    }*/
    /*
    auto input_automata = take_input("armcNFA_inclTest_12.vtf");
    if (input_automata){
        input_automata->print();
    }
    else{
        std::cout << "nullptr" << std::endl;
    }*/
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
    auto copy = input_automata->copy();
    //input_automata->print();
    auto time_bef_d = clock();
    auto dfa = det_n_min(input_automata);
    auto time_aft_d = clock();

    bool is_equal = language_equal(input_automata, dfa);
    std::cout << std::fixed << input_file << " " << input_automata->get_state_number() << " "
              << dfa->get_state_number() << " " << (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC <<
              " " << is_equal << " " << (float) (clock() - time_aft_d) / CLOCKS_PER_SEC ;


    time_bef_d = clock();
    simulate_min(copy);
    time_aft_d = clock();
    is_equal = language_equal(input_automata, copy);
    std::cout << std::fixed << " " << copy->get_state_number() << " " <<
                (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC << " " << is_equal << " " <<
                (float) (clock() - time_aft_d) / CLOCKS_PER_SEC << std::endl;
    return 0;
}

void run_all(){
    std::string base = "armcNFA_inclTest_";
    int index = 0;
    int error = 0;
    while (not error) {
        std::string file = base + std::to_string(index) + ".vtf";
        error = run_reduction(file);
        index++;
    }
}

//usage: ./automata {-t min_det}  [--file]
int main(int argc, char* argv[]) {
    if (0){
        debug();
        return 0;
    }

    std::string arg_type, arg_file;
    //if (parse_args(argc, argv, arg_type, arg_file)){
    //    return -1;
    //}

    if (/*arg_file.empty()*/0){
        run_all();
    }
    else{
        //arg_file
        run_all();
        /*
        if (run_reduction("armcNFA_inclTest_4.vtf")){
            std::cerr << "Couldn't parse automata form the input file\n";
        }*/
    }
    return 0;
}
