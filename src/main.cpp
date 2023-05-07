/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 15.04.2023
* Subject: Bachelor's thesis - 2st part
*/

#include "auto_language_check.h"
#include "min_det_auto.h"
#include "simulation_auto.h"
#include "rezidual_auto.h"
#include "vtf_input.h"
#include "qbf.h"
#include "sat.h"

#include <cmath>
#include <iostream>
#include <ctime>


// output file of the solver, used for generating the automaton
#define SAT_OUT "../solvers/sat_result.txt"
#define SAT_NFA_OUT "../solvers/nfa_result.txt"
#define QBF_OUT "../solvers/qbf_result.txt"

// output file where the generated equivalent automaton is saved
#define SAT_SAVE_FILE "sat_min.vtf"
#define NFA_SAVE_FILE "sat_nfa_min.vtf"
#define SAVE_QBF_FILE "qbf_min.vtf"

// number of clauses when not optimised tseitin is used
#define NOT_CLAUS 2
#define AND_OR_CLAUS 3

// options for input
#define RED 1
#define SAT 2
#define SAT_NFA 3
#define QBF 4

#define INIT 1
#define INIT_HEAD 2
#define CLAUSES 3
#define COMPARE 4

/***
 * Create the object of automaton from the result of the SAT solver from sat_result.txt, is dependant on the solver
 * @param states        number of states of the automaton
 * @param symbols       number of symbols of the alphabet of the automaton
 * @return              pointer to the object of the created automaton, if failed nullptr is returned
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

/***
 * Create the object of automaton from the result of the SAT solver for NFA from nfa_result.txt, is dependant on the solver
 * @param states        number of states of the automaton
 * @param symbols       number of symbols of the alphabet of the automaton
 * @return              pointer to the object of the creates automaton, if failed nullptr is returned
 */
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

/***
 * Create the object of automaton from the result of the QBF solver from qbf_result.txt, is dependant on the solver
 * @param states    number of states of the automaton
 * @param symbols   number of symbols of the alphabet of the automaton
 * @return          pointer to the created object of the automaton, if failed nullptr is returned
 */
std::shared_ptr <det_auto> build_qbf_result(unsigned int states, unsigned int symbols){
    auto result = std::make_shared <det_auto> ();
    for (int i = 0; i < states; i++){       // add states
        result->add_state(std::to_string(i));
    }
    for (int i = 0; i < symbols; i++){      // add symbols
        result->add_alphabet(std::to_string(i));
    }

    std::ifstream output(QBF_OUT);
    std::string line;
    if (not output.is_open()){
        std::cerr << "Couldn't open file with results form QBF" << std::endl;
        return nullptr;
    }
    getline(output, line);  // skip first two lines
    getline(output, line);

    std::string token;
    unsigned int index = 0;
    bool end = false;

    while (getline(output, line)) {
        std::stringstream stream(line);
        while (std::getline(stream, token, ' ')) {  // split by spaces
            if (index >= states * symbols * states + 2 * states) {  // no more variables
                end = true;
                break;
            }
            else if (token == "V" or token == "0"){
                continue;
            }
            else if (token.starts_with('-')) {  // ignore false variables
                ;
            }
            else if (index < states * symbols * states) {   // transition variable
                result->add_transition(static_cast <int> (index / (states * states)),
                                       static_cast <int> ((index % (states * states)) / states),
                                       static_cast <int> ((index % states)));
            }
            else if (index < states * symbols * states + states) {      // initial variable
                result->add_init_state(static_cast <int> (index - states * symbols * states));
            }
            else {      // final variable
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
uint fac(int x){
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

    claus += states * alpha * (fac(static_cast<int>(states)) / (fac(static_cast<int>(states)-2) * 2));  // determinism clauses
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
 * Prints quantified variables
 * @param from  starting index of the variable
 * @param num   number of variables
 */
void print_quant_vars(uint from, uint num){
    if (num == 0){
        return;
    }

    for (uint i = 0; i < num; i++){
        std::cout << " " << from+i;
    }
}

/***
 * Counts the number of zero bits in the invalid combinations
 * @param bin   value fo the binary vector
 * @param bits  number of bits of the binary vector
 * @return      number of zero bits
 */
uint count_bin_zero(uint bin, uint bits){
    do {
        if (bin & 1){
            bits--;
        }
        bin = bin >> 1;
    } while (bin > 0);
    return bits;
}

/***
 * Prints the number of variables, clauses and quantified clauses for QDIMACS FORMAT
 * @param stats     input parameters for the qbf reduction
 * @return          first free index of the variable for CNF transformation (including state variables)
 */
uint print_qbf_header(const std::shared_ptr<qbf_stats>& stats, bool qute=false){
    uint states = stats->get_states();      // number of state
    uint alpha = stats->get_symbols();      // number of symbols
    uint bin = stats->get_bin();            // size of binary vector
    uint un_var = states * states * alpha + 2 * states;     // number of unquantified variables
    uint acc_var = 0, rej_var = 0, not_var = 0, and_or_var = 0, claus = 0, dynamic_tsei_vars = 0, all_vars_for_tsei = 0;

    for (const auto& word: *stats->get_accept()){
        if (word.empty()){
            claus++;        // epsilon clause, no vars
            continue;
        }
        acc_var += (word.size() + 1) * bin;         // new quant vars for a word, state variables

        claus += 2*states + states*states*word.size();         // init + final + transition clauses
        claus += static_cast<uint>((pow(2, bin) - states)) * (word.size()+1);        // + valid clauses
    }

    /*
    uint sub_zero = 0;      // number of zeros in the invalid combinations
    for (int i = (1 << bin) - 1; i >= states; i--){
        sub_zero += count_bin_zero(i, bin);
    }
    uint zero = (1 << (bin-1)) * bin - sub_zero;    // number of zeros in the valid combinations
     */

    for (const auto& word: *stats->get_reject()){
        if (word.empty()){
            claus += states;         // epsilon clauses, no vars
            continue;
        }
        rej_var += (word.size() + 1) * bin;         // new quant vars for a word, state variables

        //------------------------Basic tseitsen clauses
        /*
        and_or_var += 2*states * bin;       // and gates (ig), init + final or gates
        and_or_var += states*states*word.size() * 2*bin;        // and gates, trans or gates
        and_or_var += static_cast<uint>((pow(2, bin) - states)) * (word.size()+1) * (bin-1);    // valid and gates

        and_or_var += 2*states + states*states*word.size();     // + or gates
        and_or_var += static_cast<uint>((pow(2, bin) - states)) * (word.size()+1) - 1;      // + or gates

        not_var += 2*(states + zero);          // init + final not gates
        not_var += (states*states + zero*2*states) * word.size();       // trans not gates, end not and not for state vars
        not_var += (word.size()+1) * sub_zero;      // valid not gates*/


        //---------------------Dynamic tseisen clauses---------------------
        dynamic_tsei_vars += 2*states + states*states*word.size();      // single var for each init, final
        dynamic_tsei_vars += static_cast<uint>((pow(2, bin) - states)) * (word.size()+1);   // and trans clauses

        claus += 2 * states * ((bin+1)+1);       // init + final clauses
        claus += states*states*word.size() * ((2*bin+1)+1);       // trans clauses
        claus += static_cast<uint>((pow(2, bin) - states)) * (word.size()+1) * (bin+1);   // valid clauses
        //-----------------------------------------------------------------
        claus++;        //  add tseitsen result clause
    }

    //claus += and_or_var*AND_OR_CLAUS + not_var*NOT_CLAUS;       // basic tseitsen clauses
    claus++;        // add the clause for setting 0 as initial state

    //std::cout << un_var << " " << acc_var << " " << rej_var << " " << and_or_var << " " << not_var << std::endl;
    //std::cout << zero << " " << sub_zero << std::endl;
    std::cout << "p cnf " << un_var + acc_var + rej_var + dynamic_tsei_vars << " " << claus << std::endl;

    if (qute){
        std::cout << "e";
        print_quant_vars(1, un_var);
        std::cout << " 0" << std::endl;
    }

    if (rej_var != 0){
        std::cout << "a";
        print_quant_vars(un_var+acc_var+1, rej_var);
        std::cout << " 0" << std::endl;
    }

    if (acc_var !=0 and dynamic_tsei_vars != 0){
        std::cout << "e";
        print_quant_vars(un_var+1, acc_var);
        print_quant_vars(un_var+acc_var+rej_var+1, dynamic_tsei_vars);
        std::cout << " 0" << std::endl;
    }

    return un_var+acc_var+rej_var+1;
}



/***
 * Prints help onto the stdout
 */
void print_help(){
    std::cout << "Usage for general reduction algorithms of FSA.\n" << std::endl;
    std::cout << "./fsa_red {-t reduction_type} {-f vtf_file} [--debug]\n";
    std::cout << "Options:\n"
                 "   -t                             Sets the type of reduction to be done on automata, one of these\n"
                 "                                      reduction types has to be chosen: {hop | brz | sim | rez | all},\n"
                 "                                      the type all is possible only in debugging mode\n"
                 "   -f                             Sets the .vtf file with input automata on which the reduction will be done\n"
                 "   --save                         Saves the result automaton in the result_auto.vtf file\n"
                 "   --debug                        Prints the debug information as number of states, time required for the reduction"
                 "                                  and more onto the stdout\n" << std::endl;

    std::cout << "For reduction algorithm for DFA and NFA reduction using SAT solvers and NFA reduction using QBF solvers."
                 " Generates a formula in CNF solvable by a solver on stdout.\n" << std::endl;
    std::cout << "./fsa_red --get_info vtf_file" << std::endl;
    std::cout << "./fsa_red --sat_init_header -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./fsa_red --sat_init -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./fsa_red --sat_nfa_init -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./fsa_red --sat_add_clauses -N num -S num -M max [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./fsa_red --sat_nfa_add_clauses -N num -S num -M max [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./fsa_red --sat_compare -N num -S num vtf_file" << std::endl;
    std::cout << "./fsa_red --sat_nfa_compare -N num -S num vtf_file\n" << std::endl;

    std::cout << "./fsa_red --qbf_clauses -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./fsa_red --qbf_clauses_qute -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./fsa_red --qbf_compare -N num -S num vtf_file\n" << std::endl;

    std::cout << "Options:\n"
                 "   --get_info                     Gets and prints the info about the automaton in the vtf_file.\n"
                 "                                      The got parameters consist of:\n"
                 "                                          - half of the number of states;\n"
                 "                                          - number of symbols of the alphabet;\n"
                 "                                          - example words that the automaton accepts;\n"
                 "                                          - example words that the automaton rejects\n"
                 "                                              -> symbols of the words are separated by space, words separated by commas\n"
                 "   --sat_init_header              Generates and prints onto the stdin the whole formula for DFA representation\n"
                 "                                      in the DIMACS format together with the header.\n"
                 "   --sat_init                     Generates and prints onto the stdin the whole formula for DFA representation\n"
                 "                                      in the DIMACS format together without the header.\n"
                 "   --sat_nfa_init                 Generates and prints onto the stdin the whole formula for NFA representation\n"
                 "                                      in the DIMACS format together without the header.\n"
                 "   --sat_add_clauses              Generates and prints onto the stdin clauses for DFA representation for example words given on the input\n"
                 "   --sat_add_nfa_clauses          Generates and prints onto the stdin clauses for NFA representation for example words given on the input\n"
                 "   --sat_compare                  Compare the original automaton from the vtf_file with the one\n"
                 "                                      generated by SAT solver (DFA). The result of the solver is expected to be in sat_result.txt.\n"
                 "                                      If they are equivalent, *PASS* is printed to the stdin a generated automaton\n"
                 "                                      is saved into sat_min.vtf.\n"
                 "                                      Else the found examples of words that doesn't belong to the language\n"
                 "                                      of the automaton are printed ont stdout.\n"
                 "                                      Symbols of the words are separated by space, words are separated by commas,\n"
                 "                                      accepted words are separated by semicolon from the rejected words.\n"
                 "   --sat_nfa_compare              Compare the original automaton from the vtf_file with the one\n"
                 "                                      generated by SAT solver (NFA). The result of the solver is expected to be in nfa_result.txt.\n"
                 "                                      If they are equivalent, *PASS* is printed to the stdin a generated automaton\n"
                 "                                      is saved into sat_nfa_min.vtf.\n"
                 "                                      Else the found examples of words that doesn't belong to the language\n"
                 "                                      of the automaton are printed ont stdout.\n"
                 "                                      Symbols of the words are separated by space, words are separated by commas,\n"
                 "                                      accepted words are separated by semicolon from the rejected words.\n"
                 "   --qbf_clauses                  Generates and prints onto the stdin the header with the formula for\n"
                 "                                      automaton representation in the QDIMCAS format.\n"
                 "   --qbf_clauses_qute             Generates and prints onto the stdin the header as --qbf_clauses but every variable\n"
                 "                                      is quantified required by the QUTE solver.\n"
                 "   --qbf_compare                  Compare the original automaton from the vtf_file with the one\n"
                 "                                      generated by QBF solver(NFA). The result of the solver is expected to be in qbf_result.txt.\n"
                 "                                      If they are equivalent, *PASS* is printed to the stdin a generated automaton\n"
                 "                                      is saved into qbf_min.vtf.\n"
                 "                                      Else the found examples of words that doesn't belong to the language\n"
                 "                                      of the automaton are printed.\n"
                 "                                      Symbols of the words are separated by space, words are separated by commas,\n"
                 "                                      accepted words are separated by semicolon from the rejected words.\n"
                 "   vtf_file                       File in .vtf format with the automaton used for the reduction.\n"
                 "   -N num                         Sets the number of states of created automaton to *num*.\n"
                 "   -S num                         Sets the number of symbols of created automaton to *num*.\n"
                 "   -M max                         Sets the index of the last used variable in CNF formula to *max*, used when creating new variables\n"
                 "   -A {...}                       Sets the example words that the automaton should accept.\n"
                 "                                      The flag is followed by one and more words as separate arguments. Symbols of the\n"
                 "                                      words are separated by space. Symbols of the word are expected to be represented\n"
                 "                                      by a number.\n"
                 "   -R {...}                       Sets the example words that the automaton should reject.\n"
                 "                                      The flag is followed by one and more words as separate arguments. Symbols of the\n"
                 "                                      words are separated by space. Symbols of the word are expected to be represented\n"
                 "                                      by a number.\n";
}

/***
 * Parses the arguments on the input line
 * @param argc          number of arguments
 * @param argv          arguments on the input line
 * @param application   selected application that is returned for further usage
 * @param operation     selected option that is returned for further usage
 * @return              pointer to the object of input parameters for SAT/QBF reduction,
 *                       if no further action is needed or an error occurred nullptr is returned
 */
std::shared_ptr <automata_stats> take_args(int argc, char* argv[], unsigned int &application, unsigned int &operation){
    if (argc < 3){
        std::cerr << "Wrong arguments" << std::endl;
        print_help();
        return nullptr;
    }

    std::shared_ptr<automata_stats> input = nullptr;
    if (strcmp(argv[1], "-t") == 0 and argc > 4){
        application = RED;
        if (argc == 6){
            operation = 1;
        }
        else{
            operation = 0;
        }
    }
    else if (strcmp(argv[1], "--get_info") == 0 and argc == 3){
        input = std::make_shared <automata_stats> (0, 0);
        auto automaton = take_input(argv[2]);
        if (automaton == nullptr){
            std::cerr << "Couldn't create automaton" << std::endl;
            return nullptr;
        }

        uint states = automaton->get_state_number()/2;
        if (states < 2)     // set minimum to 2 states
            states = 2;

        // output the found info
        std::cout << states << ";" << automaton->get_alphabet() << ";";
        automaton->find_examples(input);
        input->print_words();
        return nullptr;
    }
    else if (strcmp(argv[1], "--sat_init_header")== 0 and argc > 7){
        operation = INIT_HEAD;
        application = SAT;
    }
    else if (strcmp(argv[1], "--sat_init") == 0 and argc > 7){
        operation = INIT;
        application = SAT;
    }
    else if (strcmp(argv[1], "--sat_nfa_init") == 0 and argc > 7){
        operation = INIT;
        application = SAT_NFA;
    }
    else if (strcmp(argv[1], "--sat_add_clauses") == 0 and argc > 9){
        operation = CLAUSES;
        application = SAT;
    }
    else if (strcmp(argv[1], "--sat_nfa_add_clauses") == 0 and argc > 9){
        operation = CLAUSES;
        application = SAT_NFA;
    }
    else if (strcmp(argv[1], "--sat_compare") == 0 and argc == 7){
        operation = COMPARE;
        application = SAT;
    }
    else if (strcmp(argv[1], "--sat_nfa_compare") == 0 and argc == 7){
        operation = COMPARE;
        application = SAT_NFA;
    }
    else if (strcmp(argv[1], "--qbf_clauses_qute") == 0 and argc > 7){
        operation = INIT;
        application = QBF;
    }
    else if (strcmp(argv[1], "--qbf_clauses") == 0 and argc > 7){
        operation = CLAUSES;
        application = QBF;
    }
    else if (strcmp(argv[1], "--qbf_compare") == 0 and argc == 7){
        operation = COMPARE;
        application = QBF;
    }
    else{
        std::cerr << "Wrong arguments" << std::endl;
        print_help();
        return nullptr;
    }

    if (application != RED) {
        input = std::make_shared<sat_stats>(std::stoi(argv[3]), std::stoi(argv[5]));

        if (operation == INIT or (application == QBF and operation == CLAUSES)) {
            input->add_words_args(6, argc, argv);
        } else if (operation == CLAUSES and application != QBF) {
            input->add_words_args(8, argc, argv);
        }
    }
    return input;
}

void time_reduction(std::shared_ptr<det_auto>(*reduction)(const std::shared_ptr<det_auto>&), const std::shared_ptr<det_auto>& input){
    long time_bef_d;
    long time_aft_d;
    bool is_equal;

    time_bef_d = clock();
    auto result = reduction(input);
    time_aft_d = clock();

    is_equal = sat_anticahin(input, result);
    std::cout << std::fixed << result->get_state_number() << "; " << (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC <<
              "; " << is_equal << "; " << (float) (clock() - time_aft_d) / CLOCKS_PER_SEC << "; ";
}

int run_reduction(const std::string& arg_type, const std::string& input_file, unsigned int option){
    auto input_automata = take_input(input_file);
    if (input_automata == nullptr) {
        return -1;
    }
    auto copy = input_automata->copy();

    if (option == 0){
        std::shared_ptr <det_auto> result = nullptr;
        if (arg_type == "hop"){
            result = det_n_min(input_automata);
        }
        else if (arg_type == "brz"){
            result = brzozowski(input_automata);
        }
        else if (arg_type == "sim"){
            simulate_min(std::static_pointer_cast<simul_auto>(copy));
        }
        else if (arg_type == "rez"){
            result = rezidual_auto(input_automata);
        }
        else{
            ;
        }
        if (result != nullptr){
            result->save_to_file(input_file.substr(input_file.rfind("/") + 1), arg_type);
        }
        else {
            copy->save_to_file(input_file.substr(input_file.rfind("/") + 1), arg_type);
        }

    }
    else if (option == 1){      //debug
        std::cout << std::fixed << input_file << "; " << input_automata->get_state_number() << "; ";
        if (arg_type == "hop" or arg_type == "all"){
            time_reduction(&det_n_min, input_automata);
        }
        if (arg_type == "brz" or arg_type == "all"){
            time_reduction(&brzozowski, input_automata);
        }
        if (arg_type == "sim" or arg_type == "all"){
            long time_bef_d;
            long time_aft_d;
            bool is_equal;
            time_bef_d = clock();
            simulate_min(std::static_pointer_cast<simul_auto>(copy));
            time_aft_d = clock();
            is_equal = sat_anticahin(input_automata, copy);
            std::cout << std::fixed << copy->get_state_number() << "; " <<
                      (float) (time_aft_d - time_bef_d) / CLOCKS_PER_SEC << "; " << is_equal << "; " <<
                      (float) (clock() - time_aft_d) / CLOCKS_PER_SEC  << "; ";
        }
        if (arg_type == "rez" or arg_type == "all"){
            time_reduction(&rezidual_auto, input_automata);
        }
        std::cout << std::endl;
    }

    return 0;
}

void run_sat(int argc, char* argv[], const std::shared_ptr <sat_stats>& stats, unsigned int option){
    if (option == INIT){     // print all clauses without header for dfa
        auto state_num = stats->get_states();
        auto alpha_num = stats->get_symbols();
        stats->determine_clauses();
        stats->complete_clauses();
        stats->example_clauses(state_num * state_num * alpha_num + state_num + 1);
    }
    else if (option == INIT_HEAD){
        auto state_num = stats->get_states();
        auto alpha_num = stats->get_symbols();
        print_sat_header(stats);
        stats->determine_clauses();
        stats->complete_clauses();
        stats->example_clauses(state_num * state_num * alpha_num + state_num + 1);
    }
    else if (option == CLAUSES){        // add clauses for new example words
        stats->example_clauses(std::stoi(argv[7]) + 1);
    }
    else if (option == COMPARE){
        auto created = build_result(stats->get_states(), stats->get_symbols());
        auto orig = take_input(argv[argc-1]);
        stats->clear();
        if (sat_equal(created, orig, static_cast <std::shared_ptr<automata_stats>>(stats))){    // equal
            created->save_to_file(SAT_SAVE_FILE, "SAT_reduced");    // save the generated automaton
            std::cout << "PASS" << std::endl;
        }
        else{       // aren't equal
            stats->print_words();
        }
    }
    else {
        ;
    }
}

void run_nfa_sat(int argc, char* argv[], const std::shared_ptr <sat_stats>& stats, unsigned int option){
    if (option == INIT){
        auto state_num = stats->get_states();
        auto alpha_num = stats->get_symbols();
        std::cout << state_num * state_num * alpha_num + 1 << " 0" << std::endl;    // print intial state 1
        stats->example_nfa_clauses(state_num * state_num * alpha_num + 2*state_num + 1);
    }
    else if (option == CLAUSES){
        stats->example_nfa_clauses(std::stoi(argv[7]) + 1);
    }
    else if (option == COMPARE){
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
    else {
        ;
    }
}

void run_qbf(int argc, char* argv[], const std::shared_ptr <qbf_stats>& stats, unsigned int option){
    if (option == INIT){     // print the formula with header for QUTE solver
        uint tsei = print_qbf_header(stats, true);
        stats->example_clauses(tsei);
    }
    else if (option == CLAUSES){     // print the formula with header
        uint tsei = print_qbf_header(stats);
        stats->example_clauses(tsei);
    }
    else if (option == COMPARE){    // compare the automata for language equivalence
        auto created = build_qbf_result(stats->get_states(), stats->get_symbols());
        auto orig = take_input(argv[argc-1]);
        stats->clear();

        if (sat_equal(created, orig, static_cast <std::shared_ptr<automata_stats>>(stats))){    // equal
            created->save_to_file(SAVE_QBF_FILE, "QBF_reduced");    // save the created automaton
            std::cout << "PASS" << std::endl;
        }
        else {   // are not equal, print found examples
            stats->print_words();
        }
    }
    else {
        ;
    }
}

int main(int argc, char* argv[]) {
    unsigned int application=0, option=0;
    auto stats = take_args(argc, argv, application, option);

    if (application == RED){
        run_reduction(argv[2], argv[4], option);
    }
    else if (application == SAT){
        run_sat(argc, argv, std::static_pointer_cast <sat_stats> (stats), option);
    }
    else if (application == SAT_NFA){
        run_nfa_sat(argc, argv, std::static_pointer_cast <sat_stats> (stats), option);
    }
    else if (application == QBF){
        std::shared_ptr <qbf_stats> qb = std::make_shared<qbf_stats>(*stats);
        run_qbf(argc, argv, qb, option);
    }
    else {
        ;
    }
    return 0;
}
