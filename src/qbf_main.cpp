/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 15.04.2023
* Subject: Bachelor's thesis - 2st part
*/
#include "auto_language_check.h"
#include "qbf.h"
#include "vtf_input.h"

// number of clauses when not optimised tseitin is used
#define NOT_CLAUS 2
#define AND_OR_CLAUS 3

// options for input
#define INFO 1
#define CLAUSES 2
#define COMPARE 3

// output file of the QBF solver, used for getting the results
#define QBF_OUT "../build_qbf/result.txt"


// output file where the generated equivalent automaton is saved
#define SAVE_FILE "qbf_min.vtf"

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
uint print_qbf_header(const std::shared_ptr<qbf_stats>& stats){
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

    std::cout << "a";
    print_quant_vars(un_var+acc_var+1, rej_var);
    std::cout << " 0" << std::endl;

    std::cout << "e";
    print_quant_vars(un_var+1, acc_var);
    print_quant_vars(un_var+acc_var+rej_var+1, dynamic_tsei_vars);
    std::cout << " 0" << std::endl;

    return un_var+acc_var+rej_var+1;
}

void print_help(){
    std::cout << "Reduction algorithm for NFA reduction using QBF solvers. Generates a formula solvable by a solver on stdout.\n" << std::endl;
    std::cout << "./qbf_red --get_info vtf_file" << std::endl;
    std::cout << "./qbf_red --clauses -N num -S num [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./qbf_red --compare -N num -S num vtf_file\n" << std::endl;
    std::cout << "Options:\n"
                 "   --get_info                          Gets and prints the info about the automaton in the vtf_file.\n"
                 "                                          The parameters get consist of:\n"
                 "                                              - half of the number of states;\n"
                 "                                              - number of symbols of the alphabet;\n"
                 "                                              - example words that the automaton accepts;\n"
                 "                                              - example words that the automaton rejects\n"
                 "                                                  -> symbols of the words are separated by space, words separated by commas\n"
                 "   --clauses                           Generates and prints onto the stdin the header with the formula for\n"
                 "                                           automaton representation in the QDIMCAS format.\n"
                 "   --compare                           Compare the original automaton from the vtf_file with the one\n"
                 "                                           generated by QBF solver. The result of the solver is expected to be in result.txt.\n"
                 "                                           If they are equivalent, *PASS* is printed to the stdin a generated automaton\n"
                 "                                           is saved into qbf_min.vtf.\n"
                 "                                           Else the found examples of words that doesn't belong to the language\n"
                 "                                           of the automaton are printed.\n"
                 "                                          Symbols of the words are separated by space, words are separated by commas,\n"
                 "                                           accepted words are separated by semicolon from the rejected words.\n"
                 "   -N num                              Sets the number of states of created automaton to *num*.\n"
                 "   -S num                              Sets the number of symbols of created automaton to *num*.\n"
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
 * @param argc      number of arguments
 * @param argv      arguments on the input line
 * @param option    selected option that is returned for further applications
 * @return          pointer to the object of input parameters for QBF reduction
 */
std::shared_ptr <qbf_stats> take_args(int argc, char* argv[], uint &option){
    if (argc < 3){
        std::cerr << "Wrong arguments" << std::endl;
        print_help();
        return nullptr;
    }

    if (strcmp(argv[1], "--get_info") == 0){
        option = INFO;
    }
    else if (strcmp(argv[1], "--clauses") == 0 and argc > 7){
        option = CLAUSES;
    }
    else if (strcmp(argv[1], "--compare") == 0 and argc == 7){
        option = COMPARE;

    }
    else{
        std::cerr << "Wrong arguments" << std::endl;
        print_help();
        return nullptr;
    }

    std::shared_ptr <qbf_stats> input = nullptr;

    if (option == INFO){    // find examples of word of the automaton
        input = std::make_shared <qbf_stats> (0, 0);
        auto automaton = take_input(argv[2]);
        if (automaton == nullptr){
            std::cerr << "Couldn't create automaton" << std::endl;
            return nullptr;
        }
        // output the found info
        std::cout << automaton->get_state_number()/2 << ";" << automaton->get_alphabet() << ";";
        automaton->find_examples(input);
        input->print_words();
        return nullptr;
    }
    else {
        input = std::make_shared <qbf_stats> (std::stoi(argv[3]), std::stoi(argv[5]));
        if (option == CLAUSES){
            input->add_words_args(6, argc, argv);
        }
    }

    return input;
}


/***
 * Create the object of automaton from the result of the QBF solver from result.txt, is dependant on the solver
 * @param states    number of states of the automaton
 * @param symbols   number of symbols of the alphabet of the automaton
 * @return          pointer to the created object of the automaton, if failed nullptr is returned
 */
std::shared_ptr <det_auto> build_result(unsigned int states, unsigned int symbols){
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
            //std::cerr << token << " ";
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
                //std::cout << "Trans" << (index % (states * states)) / states << index / (states * states) << index % states << std::endl;
                result->add_transition(static_cast <int> (index / (states * states)),
                                       static_cast <int> ((index % (states * states)) / states),
                                       static_cast <int> ((index % states)));
            }
            else if (index < states * symbols * states + states) {      // initial variable
                //std::cout << "Init" << std::endl;
                result->add_init_state(static_cast <int> (index - states * symbols * states));
            }
            else {      // final variable
                //std::cout << "Final" << std::endl;
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

int main(int argc, char* argv[]){
    if (0){ // debug
        auto debug = std::make_shared <qbf_stats> (2,2);
        std::queue <unsigned int> heh({0});
        std::queue <unsigned int> eh({1});
        debug->add_accept(heh);
        debug->add_reject(eh);
        uint tsei = print_qbf_header(debug);
        debug->example_clauses(tsei);
        return 1;
    }

    uint option;
    auto stats = take_args(argc, argv, option);
    if (stats == nullptr){
        return 0;
    }

    if (option == CLAUSES){     // print the formula with header
        uint tsei = print_qbf_header(stats);
        stats->example_clauses(tsei);
    }
    else if (option == COMPARE){    // compare the automata for language equivalence
        auto created = build_result(stats->get_states(), stats->get_symbols());
        auto orig = take_input(argv[argc-1]);
        stats->clear();

        if (sat_equal(created, orig, static_cast <std::shared_ptr<automata_stats>>(stats))){    // equal
            created->save_to_file(SAVE_FILE, "QBF_reduced");    // save the created automaton
            std::cout << "PASS" << std::endl;
        }
        else {   // are not equal, print found examples
            stats->print_words();
        }
    }
    else {
        std::cout << "Internal error" << std::endl;
    }
    return 0;
}