#include "sat.h"
#include "auto_language_check.h"
#include "vtf_input.h"

#define SAT_OUT "../build_sat/result.txt"

std::shared_ptr <det_auto> build_result(unsigned int states, unsigned int symbols){
    auto result = std::make_shared <det_auto>();
    for (int i = 0; i < states; i++){
        result->add_state(std::to_string(i));
    }
    for (int i = 0; i < symbols; i++){
        result->add_alphabet(std::to_string(i));
    }
    result->add_init_state(0);

    std::ifstream output(SAT_OUT);
    std::string line;
    if (not output.is_open()){
        std::cerr << "Couldn't open file with results form SAT" << std::endl;
        return nullptr;
    }
    getline(output, line);

    std::string token;
    unsigned int index = 0;
    bool end = false;

    while (getline(output, line)) {
        std::stringstream stream(line);
        while (std::getline(stream, token, ' ')) {
            if (index >= states * symbols * states + states) {
                end = true;
                break;
            }
            else if (token == "v"){
                continue;
            }
            else if (token.starts_with('-')) { ;
            } else if (index < states * symbols * states) {
                result->add_transition(static_cast <int> ((index % (states * symbols)) / states),
                                       static_cast <int> (index / (states * symbols)),
                                       static_cast <int> ((index % states)));
            } else {
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

void print_sat_header(){
    std::cout << "c sat\n";
    std::cout << "c\n";
    std::cout << "p cnf 0 0\n";
}

void print_help(){
    std::cout << "./sat --get_info vtf_file" << std::endl;
    std::cout << "./sat --init -N 3 -S 2 [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./sat --add -N 3 -S 2 -M max [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./sat --compare -N 3 -S 2 vtf_file" << std::endl;
}

// ./sat --get_info vtf_file
// ./sat --init -N 3 -S 2 [-A {...}] [-R {...}]
// ./sat --add -N 3 -S 2  -M max [-A {...}] [-R {...}]
// ./sat --compare -N 3 -S 2 vtf_file
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

    if (operation == 1){
        input->add_words_args(6, argc, argv);
    }
    else if (operation == 2){
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
    if (0){
        auto mat = build_result(4,2);
        if (mat != nullptr){
            mat->print();
        }
        return 1;
    }

    unsigned int op_index;
    auto stats = take_args(argc, argv, op_index);
    if (stats == nullptr){
        return 0;
    }

    if (op_index == 1){     // print all clauses
        auto state_num = stats->get_states();
        auto alpha_num = stats->get_symbols();
        //print_sat_header();
        stats->determine_clauses();
        stats->complete_clauses();
        stats->example_clauses(state_num * state_num * alpha_num + state_num);
    }
    else if (op_index == 2){        // add clauses for new example words
        stats->example_clauses(std::stoi(argv[7]));
    }
    else if (op_index == 3){
        auto created = build_result(stats->get_states(), stats->get_symbols());
        auto orig = take_input(argv[argc-1]);
        stats->clear();
        if (sat_equal(created, orig, static_cast <std::shared_ptr<automata_stats>>(stats))){
            created->save_to_file("sat_min.vtf", "SAT_reduced");
            std::cout << "PASS" << std::endl;
        }
        else{
            stats->print_words();
        }
    }
    else {
        ;
    }

    return 0;
}
