#include "qbf.h"

qbf_stats::qbf_stats(int states, int symbols) : automata_stats(states, symbols) {
    this->state_bin = ceil(log2(states));
}

unsigned int qbf_stats::get_bin() const{
    return this->state_bin;
}

void recurse_init_final(const std::string& expr, uint var_base, uint& result_base, uint iter, uint max){
    if (iter == 0){
        std::cout << expr << result_base << " 0" << std::endl;
        result_base++;
    }
    else{
        recurse_init_final(expr + std::to_string(var_base) + " ", var_base + 1, result_base, iter-1, max);
        if (result_base < max){
            recurse_init_final(expr + "-" + std::to_string(var_base) + " ", var_base + 1, result_base, iter-1, max);
        }
    }
}

void qbf_stats::init_final_clauses(uint state_base, uint end_base){
    uint trans_vars = this->state_num*this->state_num*this->alpha_num + 1;
    recurse_init_final("", state_base, trans_vars, this->state_bin, trans_vars + this->state_num);      //init
    recurse_init_final("", end_base, trans_vars, this->state_bin, trans_vars + this->state_num);        //final
}

void recurse_init_final_reject(const std::vector<int>& base, uint var_base, uint& result_base, uint iter, uint max,
                               std::vector<int>& result){
    if (iter == 0){
        result.insert(result.end(), base.begin(), base.end());
        result.push_back(NOT);
        result.push_back(static_cast <int> (result_base));
        result.push_back(OR);

        result_base++;
    }
    else{
        std::vector <int> tmp = base;
        tmp.push_back(NOT);
        tmp.push_back(static_cast <int> (var_base));
        tmp.push_back(AND);
        recurse_init_final_reject(tmp, var_base + 1, result_base, iter-1, max, result);
        if (result_base < max){
            tmp.pop_back();
            tmp.pop_back();
            tmp.pop_back();
            tmp.push_back(static_cast <int> (var_base));
            tmp.push_back(AND);

            recurse_init_final_reject(tmp, var_base + 1, result_base, iter-1, max, result);
        }
    }
}

void qbf_stats::init_final_clauses_reject(uint state_base, uint end_base, std::vector <int>& result){
    uint trans_vars = this->state_num*this->state_num*this->alpha_num + 1;
    std::vector <int> empty;

    recurse_init_final_reject(empty, state_base, trans_vars, this->state_bin, trans_vars + this->state_num, result);      //init
    recurse_init_final_reject(empty, end_base, trans_vars, this->state_bin, trans_vars + this->state_num, result);        //final
}

void qbf_stats::valid_combinations(uint start){
    for (uint i = this->state_num; i < pow(2, this->state_bin); i++){       //TODO bit shifts
        std::cout << "-" << std::to_string(start) << " ";

        uint cnt_back = this->state_bin-1;
        uint tmp = i;
        while (tmp > 1){
            if (tmp % 2){
                std::cout << "-" << std::to_string(start + cnt_back) << " ";
            }
            else{
                std::cout << std::to_string(start + cnt_back) << " ";
            }
            tmp = tmp / 2;      //TODO bit shifts
            cnt_back--;
        }
        std::cout << "0" << std::endl;
    }
}

void qbf_stats::valid_combinations_reject(uint start, std::vector <int>& input){
    for (uint i = this->state_num; i < pow(2, this->state_bin); i++){       //TODO bit shifts
        input.push_back(static_cast <int> (start));

        uint cnt_back = this->state_bin-1;
        uint tmp = i;
        while (tmp > 1){
            input.push_back(AND);
            if (tmp % 2){
                input.push_back(static_cast <int> (start + cnt_back));
            }
            else{
                input.push_back(NOT);
                input.push_back(static_cast <int> (start + cnt_back));
            }
            tmp = tmp / 2;      //TODO bit shifts
            cnt_back--;
        }
        input.push_back(OR);
    }
}

void qbf_stats::recurse_accept(const std::string& expr, uint var_base, uint& trans_base, uint iter, uint& cnt, bool end){
    if (iter == 0){
        cnt++;
        if (end){
            std::cout << expr << trans_base << " 0" << std::endl;
            trans_base++;
        }
        else{
            uint tmp = 0;
            this->recurse_accept(expr, var_base, trans_base, this->state_bin, tmp, true);
        }
    }
    else{
        this->recurse_accept(expr + std::to_string(var_base) + " ", var_base+1, trans_base, iter-1, cnt, end);
        if (cnt < this->state_num){
            this->recurse_accept(expr + "-" + std::to_string(var_base) + " ", var_base+1, trans_base, iter-1, cnt, end);
        }
    }
}

void qbf_stats::accept_clauses(uint var, uint trans){
    std::string base;
    uint counter = 0;
    recurse_accept(base, var, trans, this->state_bin, counter);
}

void qbf_stats::recurse_reject(const std::vector <int>& base, uint var_base, uint& trans_base, uint iter, uint& cnt,
                               std::vector <int>& result, bool end){
    if (iter == 0){
        cnt++;
        if (end){
            result.insert(result.end(), base.begin(), base.end());
            result.push_back(NOT);
            result.push_back(static_cast <int> (trans_base));
            result.push_back(OR);

            trans_base++;
        }
        else{
            uint tmp = 0;
            this->recurse_reject(base, var_base, trans_base, this->state_bin, tmp, result, true);
        }
    }
    else{
        auto tmp = base;
        tmp.push_back(NOT);
        tmp.push_back(static_cast <int> (var_base));
        tmp.push_back(AND);
        this->recurse_reject(tmp, var_base+1, trans_base, iter-1, cnt, result, end);
        if (cnt < this->state_num){
            tmp.pop_back();
            tmp.pop_back();
            tmp.pop_back();

            tmp.push_back(static_cast <int> (var_base));
            tmp.push_back(AND);
            this->recurse_reject(tmp, var_base+1, trans_base, iter-1, cnt, result, end);
        }
    }
}

void qbf_stats::reject_clauses(uint var, uint trans, std::vector <int>& result){
    std::vector <int> base;
    uint counter = 0;
    recurse_reject(base, var, trans, this->state_bin, counter, result);
}

void qbf_stats::example_clauses(uint tsei_start){
    unsigned int base = this->state_num * this->state_num * this->alpha_num;
    uint var = this->state_num * this->state_num * this->alpha_num + 2 * this->state_num + 1;

    std::cout << base+1 << " 0" << std::endl;   //set the first state 0 az an initial state

    for (auto word: this->accept){
        if (word.empty()){
            for (int i = 1; i <= this->state_num; i++){
                std::cout << "-" << base + i << " " << base + i + this->state_num << " 0" << std::endl;
            }
            //special case epsilon
            continue;
        }

        this->init_final_clauses(var, var + this->state_bin * word.size());
        this->valid_combinations(var);
        while (not word.empty()){
            auto symbol = word.front();
            word.pop();
            accept_clauses(var, 1 + symbol * this->state_num * this->state_num);
            var += this->state_bin;
            this->valid_combinations(var);
        }
        var += this->state_bin;
    }

    for (auto word: this->reject){
        std::vector <int> result;

        if (word.empty()){
            for (int i = 1; i <= this->state_num; i++){
                std::cout << "-" << base + i << " -" << base + i + this->state_num << " 0" << std::endl;
            }
            //special case epsilon
            continue;
        }

        this->init_final_clauses_reject(var, var + this->state_bin * word.size(), result);
        this->valid_combinations_reject(var, result);

        while (not word.empty()){
            auto symbol = word.front();
            word.pop();
            reject_clauses(var, 1 + symbol * this->state_num * this->state_num, result);
            var += this->state_bin;
            this->valid_combinations_reject(var, result);
        }
        var += this->state_bin;

        /*
        std::cerr << "Result: " << std::endl;
        for (auto elem: result){
            std::cerr << elem << " ";
        }
        std::cerr << std::endl;
         */

        result.pop_back();
        tsei_start = dynamic_tseitsen(result, tsei_start);
        result.clear();
    }
}
