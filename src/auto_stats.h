#ifndef BAKALARKA_AUTO_STATS_H
#define BAKALARKA_AUTO_STATS_H

#include <iostream>
#include <queue>
#include <cstring>
#include <sstream>

class automata_stats {
protected:
    unsigned int state_num;
    unsigned int alpha_num;

    std::vector <std::queue<unsigned int>> accept;     //vector index representing words
    std::vector <std::queue<unsigned int>> reject;

public:
    automata_stats(int states, int symbols);

    [[nodiscard]] unsigned int get_states() const;

    [[nodiscard]] unsigned int get_symbols() const;

    std::vector <std::queue<unsigned int>> *get_accept();

    std::vector <std::queue<unsigned int>> *get_reject();

    void set_symbols(int symbols);

    void add_accept(std::queue<unsigned int> &word);

    void add_reject(std::queue<unsigned int> &word);

    void add_words_args(int start, int argc, char *argv[]);

    void clear();

    void print();

    void print_words();
};

#endif //BAKALARKA_AUTO_STATS_H
