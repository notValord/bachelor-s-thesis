//
// Created by vmvev on 11/7/2022.
//

#include "auto_dictionary.h"


bool auto_dictionary::smooth_vector_state(unsigned int& old, unsigned int& change){
    std::string end_elem;
    while(end_elem.empty()){
        end_elem = _index_name_state.back();   // take last state in the vector
        _index_name_state.pop_back();
        max_index--;
    }
    old = _index_name_state.size();

    do{
        if (free_index.empty()){
            _index_name_state.push_back(end_elem);
            max_index++;
            return false;
        }

        change = free_index.back();                 // take a free index from vector
        free_index.pop_back();
    } while (change >= old);

    _index_name_state[change] = end_elem;       // move last element to free index
    _name_index_state[end_elem] = change;       // change its index in the map

    return true;
}

unsigned int auto_dictionary::add_state(const std::string& name) {
    if (free_index.empty()) {                   // no free indexes
        _name_index_state[name] = max_index;
        _index_name_state.push_back(name);
        max_index++;
        return max_index - 1;
    }

    unsigned int new_index = free_index.back();
    free_index.pop_back();
    _name_index_state[name] = new_index;
    _index_name_state[new_index] = name;

    return new_index;
}

void auto_dictionary::add_alpha(const std::string& name, unsigned int index){
    if (index > this->_index_name_alpha.size()){
        std::cerr<< "Indexing out of range in add_alpha\n";
        return;
    }
    else{
        this->_name_index_alpha[name] = index;
        this->_index_name_alpha.push_back(name);
    }
}

unsigned int auto_dictionary::get_state_index(const std::string& name){
    auto search = _name_index_state.find(name);
    if (search == _name_index_state.end()){
        std::cerr << "Couldn't find the name" << std::endl;
        exit(-1);
    }
    return search->second;
}

std::string auto_dictionary::get_state_name(const unsigned int index){
    if (index < max_index){
        return _index_name_state[index];
    }

    std::cerr << "Indexing out of bounds" << std::endl;
    exit(-1);
}

bool auto_dictionary::state_exists(const std::string& name){
    auto search = _name_index_state.find(name);
    if (search == _name_index_state.end()){
        return false;
    }
    return true;
}

bool auto_dictionary::state_exists(int index){
    if (index >= this->_index_name_state.size()){
        return false;
    }
    if (this->_index_name_state[index].empty()){
        return false;
    }
    return true;
}

void auto_dictionary::remove_state(const std::string& name){
    unsigned int index = get_state_index(name);
    _name_index_state.erase(name);
    if (index == max_index-1){
        _index_name_state.pop_back();
        max_index--;
    }
    else{
        _index_name_state[index] = "";
        free_index.push_back(index);
    }
}

void auto_dictionary::remove_state(const unsigned int& index){
    std::string name = get_state_name(index);
    _name_index_state.erase(name);
    if (index == max_index-1){
        _index_name_state.pop_back();
        max_index--;
    }
    else{
        _index_name_state[index] = "";
        free_index.push_back(index);
    }
}

void auto_dictionary::change_state_name(const unsigned int& index, const std::string& new_name){
    if (state_exists(static_cast<int> (index))){
        auto old_name = this->_index_name_state[index];
        this->_index_name_state[index] = new_name;
        this->_name_index_state.erase(old_name);
        this->_name_index_state[new_name] = index;
    }
}

unsigned int auto_dictionary::get_alpha_index(const std::string& name){
    auto search = _name_index_alpha.find(name);
    if (search == _name_index_alpha.end()){
        std::cerr << "Couldn't find the name" << std::endl;
        exit(-1);
    }
    return search->second;
}

std::string auto_dictionary::get_alpha_name(const unsigned int index){
    if (index < this->_index_name_alpha.size()){
        return _index_name_alpha[index];
    }
    std::cerr << "Indexing out of bounds" << std::endl;
    exit(-1);
}

bool auto_dictionary::alpha_exists(const std::string& name){
    auto search = _name_index_alpha.find(name);
    if (search == _name_index_alpha.end()){
        return false;
    }
    return true;
}

void auto_dictionary::remove_alpha(const std::string& name){     //not expecting high removing of symbols
    unsigned int index = get_alpha_index(name);
    std::cout << this->_name_index_alpha.erase(name) << std::endl;

    if (index == this->_index_name_alpha.size()-1){
        this->_index_name_alpha.pop_back();
    }
    else{
        auto last_elem = this->_index_name_alpha.back();
        this->_index_name_alpha.pop_back();
        this->_index_name_alpha[index] = last_elem;
        this->_name_index_alpha[last_elem] = index;
    }
}

void auto_dictionary::remove_alpha(unsigned int index) {     //not expecting high removing of symbols
    std::string name = get_alpha_name(index);
    _name_index_alpha.erase(name);
    if (index == max_index - 1) {
        _index_name_alpha.pop_back();
    } else {
        auto last_elem = this->_index_name_alpha.back();
        _index_name_alpha.pop_back();
        _index_name_alpha[index] = last_elem;
        _name_index_alpha[last_elem] = index;
    }

}

unsigned int auto_dictionary::get_state_size() const{
    return this->max_index;
}


bool auto_dictionary::check_alphabets(auto_dictionary& is_same){
    if (this->_index_name_alpha.size() != is_same._index_name_alpha.size()){
        return false;
    }

    for (int i = 0; i < this->_index_name_alpha.size(); i++){
        if (this->_index_name_alpha[i] != is_same._index_name_alpha[i]){
            return false;
        }
    }
    return true;
}

void auto_dictionary::print(){
    std::cout << "------------------------Dictionary--------------------\n";
    std::cout << "States::" << std::endl;
    for (const auto& element: this->_name_index_state){
        std::cout << element.first << " -> " << element.second << std::endl;
    }

    std::cout << "Alphabet::" << std::endl;
    for (const auto& element: this->_name_index_alpha){
        std::cout << element.first << " -> " << element.second << std::endl;
    }
    std::cout << "MAX index:: " << max_index << std::endl;
    std::cout << "--------------------------------------------------------\n";
}

void auto_dictionary::print_vec(){
    std::cout << "------------------------Dictionary--------------------\n";
    std::cout << "States::" << std::endl;
    for (int i = 0; i < this->_index_name_state.size(); i++){
        std::cout << i << " -> " << this->_index_name_state[i] << std::endl;
    }

    std::cout << "Alphabet::" << std::endl;
    for (int i = 0; i < this->_index_name_alpha.size(); i++){
        std::cout << i << " -> " << this->_index_name_alpha[i] << std::endl;
    }
    std::cout << "--------------------------------------------------------\n";
}