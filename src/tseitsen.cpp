#include "tseitsen.h"

void work_and_not_chain(std::queue <int>& and_chain, unsigned int max_index){
    std::string save;
    int elem;
    bool not_flag = false;

    while (not and_chain.empty()){
        elem = and_chain.front();   // get the first element
        and_chain.pop();

        if (elem == NOT){       // negate the variable for the output
            std::cout << "-";
            not_flag = true;
        }
        else if (elem == OR){
            std::cerr << "Wrong format for tseitsen" << std::endl;
            return;
        }
        else if (elem != AND){  // variable
            std::cout << elem << " -" << max_index << " 0" << std::endl;
            if (not_flag){      // save the opposite variable
                save += std::to_string(elem) + " ";
            }
            else{
                save += "-" + std::to_string(elem) + " ";
            }
            not_flag = false;
        }
    }

    // print the last saved clause
    std::cout << save << max_index << " 0" << std::endl;
}


void work_or_chain(const std::vector <int>& or_chain){
    // as the output is set to be true, clauses can be optimised
    // clauses where output is not negated don't affect the result and can be omitted
    // the last clause where output is negated is printed,
    // but as the output doesn't affect the clause, it can be left out

    for (auto elem: or_chain){
        std::cout << elem << " ";
    }
    std::cout << "0" << std::endl;
}

unsigned int dynamic_tseitsen(const std::vector <int>& input, unsigned int max_index){
    std::queue <int> part;          // and-clause, queue to keep the correct order of the operators
    std::vector <int> or_chain;     // or-clause

    for (auto index: input){
        if (index != OR){       // until or is found save the and-clause
            part.push(index);
        }
        else {      // resolve the and-clause
            work_and_not_chain(part, max_index);
            part = std::queue <int> ();        // clear queue
            or_chain.push_back(static_cast <int> (max_index));      // save created variable to or-clause
            max_index++;
        }
    }

    if (not part.empty()){      // finish the last clause
        work_and_not_chain(part, max_index);
        or_chain.push_back(static_cast <int> (max_index));
        max_index++;
    }

    work_or_chain(or_chain);     // resolve or-clause
    return max_index;
}



// constant clauses used as a representation of corresponding gates
void not_gate(unsigned int in_1, unsigned int out){
    std::cout << in_1 << " " << out << " 0\n";
    std::cout << "-" << in_1 << " -" << out << " 0\n";
}

void or_gate(unsigned int in_1, unsigned int in_2, unsigned int out){
    std::cout << in_1 << " " << in_2 << " -" << out << " 0\n";
    std::cout << "-" << in_2 << " " << out << " 0\n";
    std::cout << "-" << in_1 << " " << out << " 0\n";
}

void and_gate(unsigned int in_1, unsigned int in_2, unsigned int out){
    std::cout << in_1 << " -" << out << " 0\n";
    std::cout << in_2 << " -" << out << " 0\n";
    std::cout << "-" << in_1 <<  " -" << in_2 << " " << out << " 0\n";
}

unsigned int tseitsen(const std::vector <int>& input, unsigned int max_index){
    unsigned int first, second, and_save = -1;  // variables used for saving the inputs into the gate
    int i = 0;  // index for the input

    if (input[i] == NOT){   // first element - not operator is executed and a new variable is created
        not_gate(input[i+1], max_index);
        first = max_index;
        max_index++;
        i++;
    }
    else if (input[i] < 0){
        std::cerr << "Wrong format of input to tseitsen\n";
        return 0;
    }
    else {      // first element is variables
        first = input[i];
    }

    for (i++; i+1 < input.size(); i += 2){      // cycle over the operators (every second)
        if (input[i] == AND){       // has higher priority, execute
            if (input[i+1] == NOT){     // is followed by not, execute first
                i++;
                not_gate(input[i+1], max_index);
                second = max_index;
                max_index++;
            }
            else {      // followed by variable
                second = input[i+1];
            }

            and_gate(first, second, max_index);
            first = max_index;
            max_index++;
        }
        else if (input[i] == OR){
            if (and_save == -1){        // wait for the second operand, does not have the priority
                and_save = first;
                if (input[i+1] == NOT){     // followed by not, execute first
                    i++;
                    not_gate(input[i+1], max_index);
                    first = max_index;
                    max_index++;
                }
                else{       // followed by variable
                    first = input[i+1];
                }
            }
            else{                       // execute or, 2 operands obtained, save the result for the next or
                or_gate(and_save, first, max_index);
                and_save = max_index;
                max_index++;

                if (input[i+1] == NOT){     // followed by not, execute
                    i++;
                    not_gate(input[i+1], max_index);
                    first = max_index;
                    max_index++;
                }
                else {          // followed by variable
                    first = input[i+1];
                }
            }
        }
    }

    if (and_save){          // print the last gate
        or_gate(and_save, first, max_index);
    }
    std::cout << max_index << " 0\n";       // set the output to true
    return max_index+1;
}