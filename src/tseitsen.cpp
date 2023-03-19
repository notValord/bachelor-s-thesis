#include "tseitsen.h"

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

//todo pridat not

unsigned int tseitsen(const std::vector <int>& input, unsigned int max_index){
    unsigned int first, second, and_save = -1;
    int i = 0;

    if (input[i] == NOT){
        not_gate(input[i+1], max_index);
        first = max_index;
        max_index++;
        i++;
    }
    else if (input[i] < 0){
        std::cerr << "Wrong format of input to tseitsen\n";
        return 0;
    }
    else{
        first = input[i];
    }

    for ( i++; i+1 < input.size(); i += 2){
        if (input[i] == AND){       // has higher priority, execute
            //std::cout << "AND\n";
            if (input[i+1] == NOT){
                i++;
                not_gate(input[i+1], max_index);
                second = max_index;
                max_index++;
            }
            else{
                second = input[i+1];
            }

            and_gate(first, second, max_index);
            first = max_index;
            max_index++;
        }
        else if (input[i] == OR){
            if (and_save == -1){        // wait for the second operand
                and_save = first;
                if (input[i+1] == NOT){
                    i++;
                    not_gate(input[i+1], max_index);
                    first = max_index;
                    max_index++;
                }
                else{
                    first = input[i+1];
                }
            }
            else{                       // execute
                //std::cout << "OR\n";
                or_gate(and_save, first, max_index);
                and_save = max_index;
                max_index++;

                if (input[i+1] == NOT){
                    i++;
                    not_gate(input[i+1], max_index);
                    first = max_index;
                    max_index++;
                }
                else{
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