/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 15.04.2023
* Subject: Bachelor's thesis - 2st part
*/

#ifndef BAKALARKA_AUTO_STATS_H
#define BAKALARKA_AUTO_STATS_H

#include <iostream>
#include <queue>
#include <cstring>
#include <sstream>

/***
 * Class for representing the input parameters for SAT and QBF reduction
 */
class automata_stats {
protected:
    /** holds the number of states of the created automaton*/
    unsigned int state_num;
    /** holds the number of symbols of the created automaton*/
    unsigned int alpha_num;

    /** sets of example words defining the outomaton's language*/
    std::vector <std::queue<unsigned int>> accept;     //vector index representing words
    std::vector <std::queue<unsigned int>> reject;

public:
    /***
     * Constructor - sets the number of states and symbols, vectors are set to be empty
     * @param states    number of states
     * @param symbols   number of symbols
     */
    automata_stats(int states, int symbols);

    /***
     * Getter of state_num
     * @return  the number of states
     */
    [[nodiscard]] unsigned int get_states() const;

    /***
     * Getter of alpha_num
     * @return  number of symbols
     */
    [[nodiscard]] unsigned int get_symbols() const;

    /***
     * Getter of accept
     * @return  the pointer to the set of accepted words
     * @warning can be a pointer to an empty vector
     */
    std::vector <std::queue<unsigned int>> *get_accept();

    /***
     * Getter of reject
     * @return  the pointer to the set of rejected word
     * @warning can be a pointer to an empty vector
     */
    std::vector <std::queue<unsigned int>> *get_reject();

    /***
     * Setter for alpha_num
     * @param symbols   number of symbols to be set to
     */
    void set_symbols(int symbols);

    /***
     * Adds a new word to the the set of accepted words
     * @param word  new word to be added
     * @warning     repetition of words is not detected
     */
    void add_accept(std::queue<unsigned int> &word);

    /***
     * Adds a new word to the the set of rejected words
     * @param word  new word to be added
     * @warning     repetition of words is not detected
     */
    void add_reject(std::queue<unsigned int> &word);

    /***
     * Takes the words from the input line and adds them input the example sets
     * @param start index from where the the words start on the input line
     * @param argc  number of arguments on the input line
     * @param argv  the input line
     * @warning     expects the correct format, else an error could happen
     */
    void add_words_args(int start, int argc, char *argv[]);

    /***
     * Clears the instance, sets the number of states and symbols to zero, sets of words are emptied
     */
    void clear();

    /***
     * Debug function printing the members of the class to the output
     */
    void print();

    /***
     * Prints the accepted and rejected words in the command line in a correct format for further processing
     *  Symbols represented by numbers are separated by space, words are separated by commas,
     *  accept words are separated by semicolon from reject states
     */
    void print_words();
};

#endif //BAKALARKA_AUTO_STATS_H
