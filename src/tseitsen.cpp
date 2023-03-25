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

void recurse_tsei(std::queue <int> part, unsigned int max_index, const std::string& clause, bool result){
    if (part.empty()){
        if (result){
            std::cout << clause << " " << max_index << " 0" << std::endl;
        }
        else{
            std::cout << clause << " -" << max_index << " 0" << std::endl;
        }
        return;
    }

    bool not_flag = false;
    int tmp = part.front();
    part.pop();
    if (tmp != AND){
        std::cerr << "Wrong format for tseitsen" << std::endl;
        return;
    }

    tmp = part.front();
    part.pop();

    if (tmp == NOT){
        tmp = part.front();
        part.pop();

        not_flag = true;
    }

    if (tmp == AND or tmp == OR or tmp == NOT){
        std::cerr << "Wrong format for tseitsen" << std::endl;
        return;
    }

    recurse_tsei(part, max_index, clause + " " + std::to_string(tmp), result and not_flag);
    recurse_tsei(part, max_index, clause + " -" + std::to_string(tmp), result and (not not_flag));
}

void tsei_part(std::queue <int>& part, unsigned int max_index){
    bool not_flag = false;
    int tmp = part.front();
    part.pop();

    if (tmp == NOT){
        tmp = part.front();
        part.pop();

        not_flag = true;
    }

    if (tmp == AND or tmp == OR or tmp == NOT){
        std::cerr << "Wrong format for tseitsen" << std::endl;
        return;
    }

    recurse_tsei(part, max_index, std::to_string(tmp), not_flag);
    recurse_tsei(part, max_index, "-" + std::to_string(tmp), not not_flag);

}

void work_and_not_chain(std::queue <int>& and_chain, unsigned int max_index){
    std::string save;
    int elem;
    bool not_flag = false;
    while (not and_chain.empty()){
        elem = and_chain.front();
        and_chain.pop();

        if (elem == NOT){
            std::cout << "-";
            not_flag = true;
        }
        else if (elem == OR){
            std::cerr << "Wrong format for tseitsen" << std::endl;
            return;
        }
        else if (elem != AND){
            std::cout << elem << " -" << max_index << " 0" << std::endl;
            if (not_flag){
                save += std::to_string(elem) + " ";
            }
            else{
                save += "-" + std::to_string(elem) + " ";
            }
            not_flag = false;
        }
    }
    std::cout << save << max_index << " 0" << std::endl;
}

// pocet klauzul sa rovna pocet orov v celku, jedna nova premenna
void work_or_chain(const std::vector <int>& or_chain, unsigned int max_index){
    /*for (auto elem: or_chain){
        std::cout << "-" << elem << " " << max_index << " 0" << std::endl;
    }*/
    for (auto elem: or_chain){
        std::cout << elem << " ";
    }
    std::cout << "0" << std::endl;
}

unsigned int dynamic_tseitsen(const std::vector <int>& input, unsigned int max_index){
    std::queue <int> part;
    std::vector <int> or_chain;
    for(auto index: input){
        if (index != OR){
            part.push(index);
        }
        else{
            work_and_not_chain(part, max_index);
            part = std::queue <int> ();        // clear queue
            or_chain.push_back(static_cast <int> (max_index));
            max_index++;
        }
    }
    if (not part.empty()){
        work_and_not_chain(part, max_index);
        or_chain.push_back(static_cast <int> (max_index));
        max_index++;
    }
    work_or_chain(or_chain, max_index);
    //std::cout << max_index << " 0" << std::endl;
    return max_index;
}

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