#include "automata.h"
#include "qbf.h"
#include "vtf_input.h"

#define NOT_CLAUS 2
#define AND_OR_CLAUS 3

#define INFO 1
#define CLAUSES 2
#define COMPARE 3

#define QBF_OUT "../build_qbf/result.txt"

void print_quant_vars(uint from, uint num, bool exist){
    if (num == 0){
        return;
    }

    if (exist){
        std::cout << "e";
    }
    else {
        std::cout << "a";
    }

    for (uint i = 0; i < num; i++){
        std::cout << " " << from+i;
    }
    std::cout << " 0" << std::endl;
}

uint count_bin_zero(uint bin, uint bits){
    do {
      if (bin & 1){
          bits--;
      }
      bin = bin >> 1;
    } while (bin > 0);
    return bits;
}

uint print_qbf_header(const std::shared_ptr<qbf_stats>& stats){
    uint states = stats->get_states();
    uint alpha = stats->get_symbols();
    uint bin = stats->get_bin();
    uint un_var = states * states * alpha + 2 * states;
    uint acc_var = 0, rej_var = 0, not_var = 0, and_or_var = 0, claus = 0;

    for (const auto& word: *stats->get_accept()){
        if (word.empty()){
            claus += states;        // epsilon clauses, no vars
            continue;
        }
        acc_var += (word.size() + 1) * bin;         // new quant vars for a word

        claus += 2*states + states*states*word.size();         // init + final + transition clauses
        claus += static_cast<uint>((pow(2, bin) - states)) * (word.size()+1);        // + valid clauses
    }

    uint sub_zero = 0;
    for (int i = (1 << bin) - 1; i >= states; i--){
        sub_zero += count_bin_zero(i, bin);
    }
    uint zero = (1 << (bin-1)) * bin - sub_zero;

    for (const auto& word: *stats->get_reject()){
        if (word.empty()){
            claus += states;         // epsilon clauses, no vars
            continue;
        }
        rej_var += (word.size() + 1) * bin;         // new quant vars for a word

        and_or_var += 2*states * bin;       // init + final or gates
        and_or_var += states*states*word.size() * 2*bin;        // trans or gates
        and_or_var += static_cast<uint>((pow(2, bin) - states)) * (word.size()+1) * (bin-1);    // valid and gates

        and_or_var += 2*states + states*states*word.size();     // + or gates
        and_or_var += static_cast<uint>((pow(2, bin) - states)) * (word.size()+1) - 1;      // + or gates

        not_var += 2*(states + zero);          // init + final not gates
        not_var += (states*states + zero*2*states) * word.size();       // trans not gates, end not and not for state vars
        not_var += (word.size()+1) * sub_zero;      // valid not gates

        claus++;        //  add tseitsen result clause
    }

    claus += and_or_var*AND_OR_CLAUS + not_var*NOT_CLAUS;       // tseitsen clauses
    claus++;        // add the clause for setting 0 as initial state

    //std::cout << un_var << " " << acc_var << " " << rej_var << " " << and_or_var << " " << not_var << std::endl;
    //std::cout << zero << " " << sub_zero << std::endl;
    std::cout << "c qbf" << std::endl;
    std::cout << "c" << std::endl;
    std::cout << "p cnf " << un_var + acc_var + rej_var + and_or_var + not_var << " " << claus << std::endl;

    print_quant_vars(un_var+1, acc_var, true);
    print_quant_vars(un_var+acc_var+1, rej_var, false);
    print_quant_vars(un_var+acc_var+rej_var+1, and_or_var + not_var, true);

    return un_var+acc_var+rej_var+1;
}

void print_help(){
    std::cout << "./qbf_red --get_info vtf_file" << std::endl;
    std::cout << "./qbf_red --clauses -N 3 -S 2 [-A {...}] [-R {...}]" << std::endl;
    std::cout << "./qbf_red --compare -N 3 -S 2 vtf_file" << std::endl;
}

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

    if (option == INFO){
        input = std::make_shared <qbf_stats> (0, 0);
        auto automaton = take_input(argv[2]);
        if (automaton == nullptr){
            std::cerr << "Couldn't create automaton" << std::endl;
            return nullptr;
        }
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

std::shared_ptr <automata> build_result(unsigned int states, unsigned int symbols){
    auto result = std::make_shared<automata>();
    for (int i = 0; i < states; i++){
        result->add_state(std::to_string(i));
    }
    for (int i = 0; i < symbols; i++){
        result->add_alphabet(std::to_string(i));
    }

    std::ifstream output(QBF_OUT);
    std::string line;
    if (not output.is_open()){
        std::cerr << "Couldn't open file with results form SAT" << std::endl;
        return nullptr;
    }
    getline(output, line);  // skip first two lines
    getline(output, line);

    std::string token;
    unsigned int index = 0;
    bool end = false;

    while (getline(output, line)) {
        std::stringstream stream(line);
        while (std::getline(stream, token, ' ')) {
            //std::cerr << token << " ";
            if (index >= states * symbols * states + 2 * states) {
                end = true;
                break;
            }
            else if (token == "V" or token == "0"){
                continue;
            }
            else if (token.starts_with('-')) {
                ;
            }
            else if (index < states * symbols * states) {
                //std::cout << "Trans" << (index % (states * states)) / states << index / (states * states) << index % states << std::endl;
                result->add_transition(static_cast <int> (index / (states * states)),
                                       static_cast <int> ((index % (states * states)) / states),
                                       static_cast <int> ((index % states)));
            }
            else if (index < states * symbols * states + states) {
                //std::cout << "Init" << std::endl;
                result->add_init_state(static_cast <int> (index - states * symbols * states));
            }
            else{
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
    if (0){
        auto created = build_result(4,2);
        created->print();
        return 1;
    }

    uint option;
    auto stats = take_args(argc, argv, option);
    if (stats == nullptr){
        return 0;
    }

    if (option == CLAUSES){
        //stats->print();
        uint tsei = print_qbf_header(stats);
        stats->example_clauses(tsei);
    }
    else if (option == COMPARE){
        auto created = build_result(stats->get_states(), stats->get_symbols());
        //created->print();
        auto orig = take_input(argv[argc-1]);
        stats->clear();
        if (sat_equal(created, orig, static_cast <std::shared_ptr<automata_stats>>(stats))){
            created->save_to_file("qbf_min.vtf", "QBF_reduced");
            std::cout << "PASS" << std::endl;
        }
        else{
            stats->print_words();
        }
    }
    else {
        std::cout << "Internal error" << std::endl;
    }
    return 0;
}