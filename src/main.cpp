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
                                                      {"s2", "b", "s3"}};

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
   std::set <std::string> fin_state = {"s2"};
   std::shared_ptr<automata> pokus3(std::make_shared<automata> (state, alphabe, tran, init_state, fin_state));



    auto input_automata = take_input("../input_automata/2/armcNFA_inclTest (1000).vtf");
    if (input_automata == nullptr) {
        exit(-1);
    }

    auto test = take_input("../input_automata/2/armcNFA_inclTest (1001).vtf");
    if (input_automata == nullptr) {
        exit(-1);
    }

    /*
    auto rezidual = rezidual_auto(input_automata);
   std::cout << "DOne states " << rezidual->get_state_number() << std::endl;*/

    //auto uh = input_automata->reverse();
    //uh->print();
    //auto hmm = det_n_min(uh);
    //hmm->print();
    //uh = hmm->reverse();
    //hmm = det_n_min(uh);
    //uh->print();
    //std::cout << "Done states " << hmm->get_state_number() << std::endl;
    if (sat_anticahin(input_automata, test)){
        std::cout << "SAME automata" << std::endl;
    }
    else{
        std::cout << "DIFFER!!!!" << std::endl;
    }
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

int run_reduction(const std::string& input_file, const std::string& arg_type){
    auto input_automata = take_input(input_file);
    if (input_automata == nullptr) {
        return -1;
    }
    auto copy = input_automata->copy();
    //input_automata->print();

    long time_bef_d;
    long time_aft_d;
    bool is_equal;
    std::cout << std::fixed << input_file << "; " << input_automata->get_state_number() << "; ";

    if (arg_type == "min_det" or arg_type == "all"){
        time_bef_d = clock();
        auto dfa = det_n_min(input_automata);
        time_aft_d = clock();

        is_equal = sat_anticahin(input_automata, dfa);
        std::cout << std::fixed << dfa->get_state_number() << "; " << (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC <<
                  "; " << is_equal << "; " << (float) (clock() - time_aft_d) / CLOCKS_PER_SEC;
    }

    if (arg_type == "brz" or arg_type == "all"){
        time_bef_d = clock();
        auto dfa = brzozowski(input_automata);
        time_aft_d = clock();

        is_equal = sat_anticahin(input_automata, dfa);
        std::cout << std::fixed << dfa->get_state_number() << "; " << (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC <<
                  "; " << is_equal << "; " << (float) (clock() - time_aft_d) / CLOCKS_PER_SEC;
    }

    if (arg_type == "sim" or arg_type == "all"){
         time_bef_d = clock();
         simulate_min(copy);
         time_aft_d = clock();
         is_equal = sat_anticahin(input_automata, copy);
         std::cout << std::fixed << copy->get_state_number() << "; " <<
                     (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC << "; " << is_equal << "; " <<
                     (float) (clock() - time_aft_d) / CLOCKS_PER_SEC;
    }

    /*if (not std::filesystem::create_directory(SAVE_DIR)){
        std::cerr << "Error while creating a directory in save_to_file" << std::endl;
        exit(-2);
    }*/

    if (arg_type == "rez" or arg_type == "all"){
        time_bef_d = clock();
        auto rez_done = rezidual_auto(input_automata);
        time_aft_d = clock();
        if (input_automata->get_state_number() < 350){
            is_equal = sat_anticahin(input_automata, rez_done);
        }
        else{
            is_equal = true;
        }
        //rez_done->save_to_file(input_file.substr(input_file.rfind("/") + 1), "REZIDUAL");
        std::cout << std::fixed << rez_done->get_state_number() << "; " <<
                  (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC << "; " << is_equal << "; " <<
                  (float) (clock() - time_aft_d) / CLOCKS_PER_SEC;
    }
    std::cout << std::endl;

    return 0;
}

void run_all(const std::string& arg_type){
    std::string base = "armcNFA_inclTest (";
    int index = 1;
    int error = 0;
    while (not error) {
        std::string file = base + std::to_string(index) + ").vtf";
        error = run_reduction(file, arg_type);
        index++;
    }
}

//usage: ./automata {-t min_det/sim/rez/all}  [--file]
int main(int argc, char* argv[]) {
    if (1){
        //run_all("min_det");
        debug();
        return 0;
    }

    std::string arg_type, arg_file;
    if (parse_args(argc, argv, arg_type, arg_file)){
        return -1;
    }

    if (arg_file.empty()){
        run_all(arg_type);
    }
    else{
        if (run_reduction(arg_file, arg_type)){
            std::cerr << "Couldn't parse automata form the input file\n";
        }
    }
    return 0;
}
