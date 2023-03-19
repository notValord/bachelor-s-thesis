//
// Created by vmvev on 11/7/2022.
//

#ifndef BAKALARKA_AUTO_DICTIONARY_H
#define BAKALARKA_AUTO_DICTIONARY_H

#include <iostream>
#include <vector>
#include <unordered_map>

class auto_dictionary{
private:
    unsigned int max_index = 0;                 // size of state vector
    std::vector<unsigned int> free_index;

    std::unordered_map <std::string, unsigned int> _name_index_state;
    std::vector<std::string> _index_name_state;

    std::unordered_map <std::string, unsigned int> _name_index_alpha;
    std::vector<std::string> _index_name_alpha;

public:
    bool smooth_vector_state(unsigned int& old, unsigned int& change);

    unsigned int add_state(const std::string& name);
    void add_alpha(const std::string& name, unsigned int index);

    unsigned int get_state_index(const std::string& name);
    std::string get_state_name(unsigned int index);

    bool state_exists(const std::string& name);
    bool state_exists(int index);

    void remove_state(const std::string& name);
    void remove_state(const unsigned int& index);

    void change_state_name(const unsigned int& index, const std::string& new_name);

    unsigned int get_alpha_index(const std::string& name);
    std::string get_alpha_name(unsigned int index);

    bool alpha_exists(const std::string& name);

    void remove_alpha(const std::string& name);
    void remove_alpha(unsigned int index);

    unsigned int get_state_size() const;
    bool check_alphabets(auto_dictionary& is_same);

    void print();
    void print_vec();
};


#endif //BAKALARKA_AUTO_DICTIONARY_H
