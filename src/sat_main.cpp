/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 16.04.2023
* Subject: Bachelor's thesis - 2st part
*/

#include "sat.h"
#include "auto_language_check.h"
#include "vtf_input.h"
#include <cmath>

// output file of the solver, used for generating the automaton
#define SAT_OUT "../build_sat/result.txt"

#define SAT_NFA_OUT "../build_sat/nfa_result.txt"

// output file where the generated equivalent automaton is saved
#define SAVE_FILE "sat_min.vtf"

#define NFA_SAVE_FILE "sat_nfa_min.vtf"

/***
 * Create the object of automaton from the result of the SAT solver from result.txt, is dependant on the solver
 * @param states        number of states of the automaton
 * @param symbols       number of symbols of the alphabet of the automaton
 * @return              pointer to the object of created automaton, if failed nullptr is returned
 */
std::shared_ptr <det_auto> build_result(unsigned int states, unsigned int symbols){
    auto result = std::make_shared <det_auto>();
    for (int i = 0; i < states; i++){       // create states
        result->add_state(std::to_string(i));
    }
    for (int i = 0; i < symbols; i++){      // created alphabet
        result->add_alphabet(std::to_string(i));
    }
    result->add_init_state(0);      // always the initial state

    std::ifstream output(SAT_OUT);
    std::string line;
    if (not output.is_open()){
        std::cerr << "Couldn't open file with results from SAT" << std::endl;
        return nullptr;
    }
    getline(output, line);      // skip first line

    std::string token;
    unsigned int index = 0;
    bool end = false;

    while (getline(output, line)) {
        std::stringstream stream(line);
        while (std::getline(stream, token, ' ')) {      // split line by spaces
            if (index >= states * symbols * states + states) {      // no more variables, end
                end = true;
                break;
            }
            else if (token == "v"){
                continue;
            }
            else if (token.starts_with('-')) {      // ignore false variables
                ;
            }
            else if (index < states * symbols * states) {       // transition variables
                result->add_transition(static_cast <int> (index / (states * states)),
                                       static_cast <int> ((index % (states * states)) / states),
                                       static_cast <int> ((index % states)));
            } else {        // final variables
                result->add_accept_state(static_cast <int> (index - states * symbols * states));
            }
            index++;
        }
        if (end){
            break;
        }
    }

    return result;
}

std::shared_ptr <det_auto> build_nfa_result(unsigned int states, unsigned int symbols){
    auto result = std::make_shared <det_auto>();
    for (int i = 0; i < states; i++){       // create states
        result->add_state(std::to_string(i));
    }
    for (int i = 0; i < symbols; i++){      // created alphabet
        result->add_alphabet(std::to_string(i));
    }

    std::ifstream output(SAT_NFA_OUT);
    std::string line;
    if (not output.is_open()){
        std::cerr << "Couldn't open file with results from SAT" << std::endl;
        return nullptr;
    }
    getline(output, line);

    std::string token;
    unsigned int index = 0;
    bool end = false;

    while (getline(output, line)) {
        std::stringstream stream(line);
        while (std::getline(stream, token, ' ')) {      // split line by spaces
            if (index >= states * symbols * states + 2*states) {      // no more variables, end
                end = true;
                break;
            }
            else if (token == "v"){
                continue;
            }
            else if (token.starts_with('-')) {      // ignore false variables
                ;
            }
            else if (index < states * symbols * states) {       // transition variables
                result->add_transition(static_cast <int> (index / (states * states)),
                                       static_cast <int> ((index % (states * states)) / states),
                                       static_cast <int> ((index % states)));
            } else if (index < states * symbols * states + states){        // final variables
                result->add_init_state(static_cast <int> (index - states * symbols * states));
            }
            else{
                result->add_accept_state(static_cast <int> (index - (states * symbols * states + states)));
            }
            index++;
        }
        if (end){
            break;
        }
    }

    return result;
}

//simple factorial, for negative numbers returns 1
uint fac(uint x){
    uint sum = 1;
    for (x; x > 1; x--){
        sum *= x;
    }
    return sum;
}

/***
 * Prints the number of variables and clauses, only needed for certain solvers
 */
void print_sat_header(const std::shared_ptr<sat_stats>& stats){
    uint states = stats->get_states();      // number of state
    uint alpha = stats->get_symbols();      // number of symbols
    uint un_var = states * states * alpha + states;     // number of unquantified variables
    uint claus = 0, dynamic_tsei_vars = 0;

    claus += states * alpha * (fac(states) / (fac(states-2) * 2));
    claus += states * alpha;    // completeness clauses
    for (const auto& word: *stats->get_reject()){
        if (word.empty()){
            claus++;        // epsilon clause, no vars
            continue;
        }

        claus += static_cast<uint>(pow(states, static_cast<uint>(word.size())));        //transition clauses
    }

    for (const auto& word: *stats->get_accept()){
        if (word.empty()){
            claus += states;         // epsilon clauses, no vars
            continue;
        }


        //---------------------Dynamic tseisen clauses---------------------
        dynamic_tsei_vars += static_cast<uint>(pow(states, static_cast<uint>(word.size())));      // single var trans clause

        // number fo clauses times variables in each clause + 1
        claus += static_cast<uint>(pow(states, static_cast<uint>(word.size()))) * (static_cast<uint>(word.size()) + 2);
        //-----------------------------------------------------------------
        claus++;        //  add tseitsen result clause aka final or clause
    }

    std::cout << "p cnf " << un_var + dynamic_tsei_vars << " " << claus << std::endl;
}

/***
 * Prints help onto the stdout
 */
void print_help(){
    std::cout << "Reduction algorithm for DFA reduction using SAT solvers. Generates a formula solvable by a solver on stdout.\n" << std::endl;
    std::cout << "./sat_red --get_info vtf_file" << std::endl;
    std::cout << "./sat_red --init_header -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./sat_red --init -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./sat_red --nfa -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./sat_red --add -N num -S num -M max [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./sat_red --nfa_add -N num -S num -M max [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./sat_red --compare -N num -S num vtf_file" << std::endl;
    std::cout << "./sat_red --nfa_compare -N num -S num vtf_file\n" << std::endl;
    std::cout << "Options:\n"
                 "   --get_info                          Gets and prints the info about the automaton in the vtf_file.\n"
                 "                                          The parameters get consist of:\n"
                 "                                              - half of the number of states;\n"
                 "                                              - number of symbols of the alphabet;\n"
                 "                                              - example words that the automaton accepts;\n"
                 "                                              - example words that the automaton rejects\n"
                 "                                                  -> symbols of the words are separated by space, words separated by commas\n"
                 "   --init                             Generates and prints onto the stdin the whole formula for\n"
                 "                                           automaton representation in the CNF format.\n"
                 "   --add                              Generates and prints onto the stdin clauses for example words given on the input\n"
                 "   --compare                          Compare the original automaton from the vtf_file with the one\n"
                 "                                           generated by SAT solver. The result of the solver is expected to be in result.txt.\n"
                 "                                           If they are equivalent, *PASS* is printed to the stdin a generated automaton\n"
                 "                                           is saved into sat_min.vtf.\n"
                 "                                           Else the found examples of words that doesn't belong to the language\n"
                 "                                           of the automaton are printed.\n"
                 "                                          Symbols of the words are separated by space, words are separated by commas,\n"
                 "                                           accepted words are separated by semicolon from the rejected words.\n"
                 "   -N num                              Sets the number of states of created automaton to *num*.\n"
                 "   -S num                              Sets the number of symbols of created automaton to *num*.\n"
                 "   -M max                              Sets the index of the last used variable to *max*, used when creating new variables\n"
                 "   -A {...}                            Sets the example words that the automaton should accept.\n"
                 "                                           The flag is followed by one and more words as separate arguments. Symbols of the\n"
                 "                                           words are separated by space. Symbols of the word are expected to be represented\n"
                 "                                           by a number.\n"
                 "   -R {...}                            Sets the example words that the automaton should reject.\n"
                 "                                           The flag is followed by one and more words as separate arguments. Symbols of the\n"
                 "                                           words are separated by space. Symbols of the word are expected to be represented\n"
                 "                                           by a number.\n"
                 "   vtf_file                            File in vtf format with the automaton used for the reduction.\n";
}

/***
 * Parses the arguments on the input line
 * @param argc          number of arguments
 * @param argv          arguments on the input line
 * @param operation     selected option that is returned for further applications
 * @return              pointer to the object of input parameters for SAT reduction,
 *                       if no further action is needed or an error occurred nullptr is returned
 */
std::shared_ptr <sat_stats> take_args(int argc, char* argv[], unsigned int &operation){
    if (argc < 3){
        std::cerr << "Wrong arguments" << std::endl;
        print_help();
        return nullptr;
    }

    if (strcmp(argv[1], "--init") == 0 and argc > 5){
        operation = 1;
    }
    else if (strcmp(argv[1], "--add") == 0 and argc > 7){
        operation = 2;
    }
    else if (strcmp(argv[1], "--compare") == 0 and argc == 7){
        operation = 3;
    }
    else if (strcmp(argv[1], "--get_info") == 0){
        operation = 4;
    }
    else if (strcmp(argv[1], "--nfa") == 0){
        operation = 5;
    }
    else if (strcmp(argv[1], "--nfa_add") == 0){
        operation = 6;
    }
    else if (strcmp(argv[1], "--nfa_compare") == 0){
        operation = 7;
    }
    else if (strcmp(argv[1], "--init_header") == 0){
        operation = 8;
    }
    else{
        std::cerr << "Wrong arguments" << std::endl;
        print_help();
        return nullptr;
    }

    std::shared_ptr<sat_stats> input;
    if (operation == 4){
        input = std::make_shared<sat_stats>(0, 0);
    }
    else{
        input = std::make_shared<sat_stats>(std::stoi(argv[3]), std::stoi(argv[5]));
    }

    if (operation == 1 or operation == 5 or operation == 8){
        input->add_words_args(6, argc, argv);
    }
    else if (operation == 2 or operation == 6){
        input->add_words_args(8, argc, argv);
    }
    else if (operation == 4){
        auto automaton = take_input(argv[2]);
        if (automaton == nullptr){
            std::cerr << "Couldn't create automaton" << std::endl;
            return nullptr;
        }
        std::cout << automaton->get_state_number()/2 << ";" << automaton->get_alphabet() << ";";
        automaton->find_examples(static_cast<std::shared_ptr<automata_stats>>(input));
        input->print_words();
        return nullptr;
    }
    return input;
}


int main(int argc, char* argv[]){
    if (0){     // debug
        sat_stats test(3,2);
        std::queue <uint> heh({0,1});
        test.add_reject(heh);
        test.example_nfa_clauses(3*2*3+2*3+1);
        return 0;
    }

    unsigned int op_index;
    auto stats = take_args(argc, argv, op_index);
    if (stats == nullptr){
        return 0;
    }

    if (op_index == 1){     // print all clauses without header for dfa
        auto state_num = stats->get_states();
        auto alpha_num = stats->get_symbols();
        stats->determine_clauses();
        stats->complete_clauses();
        stats->example_clauses(state_num * state_num * alpha_num + state_num + 1);
    }
    else if (op_index == 2){        // add clauses for new example words
        stats->example_clauses(std::stoi(argv[7]) + 1);
    }
    else if (op_index == 3){
        auto created = build_result(stats->get_states(), stats->get_symbols());
        auto orig = take_input(argv[argc-1]);
        stats->clear();
        if (sat_equal(created, orig, static_cast <std::shared_ptr<automata_stats>>(stats))){    // equal
            created->save_to_file(SAVE_FILE, "SAT_reduced");    // save the generated automaton
            std::cout << "PASS" << std::endl;
        }
        else{       // aren't equal
            stats->print_words();
        }
    }
    else if (op_index == 5){
        auto state_num = stats->get_states();
        auto alpha_num = stats->get_symbols();
        std::cout << state_num * state_num * alpha_num + 1 << " 0" << std::endl;    // print intial state 1
        stats->example_nfa_clauses(state_num * state_num * alpha_num + 2*state_num + 1);
    }
    else if (op_index == 6){
        stats->example_nfa_clauses(std::stoi(argv[7]) + 1);
    }
    else if (op_index == 7){
        auto created = build_nfa_result(stats->get_states(), stats->get_symbols());
        auto orig = take_input(argv[argc-1]);
        stats->clear();
        if (sat_equal(created, orig, static_cast <std::shared_ptr<automata_stats>>(stats))){    // equal
            created->save_to_file(NFA_SAVE_FILE, "SAT_reduced");    // save the generated automaton
            std::cout << "PASS" << std::endl;
        }
        else{       // aren't equal
            stats->print_words();
        }
    }
    else if (op_index == 8){    // print all clauses with header
        auto state_num = stats->get_states();
        auto alpha_num = stats->get_symbols();
        print_sat_header(stats);
        stats->determine_clauses();
        stats->complete_clauses();
        stats->example_clauses(state_num * state_num * alpha_num + state_num + 1);
    }
    else {
        ;
    }

    return 0;
}
