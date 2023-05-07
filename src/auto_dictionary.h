/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#ifndef BAKALARKA_AUTO_DICTIONARY_H
#define BAKALARKA_AUTO_DICTIONARY_H

#include <iostream>
#include <vector>
#include <unordered_map>

/***
 * Class for representing the directory translating the input states and symbols of the alphabet
 *  to a unique index in the automaton structure
 */
class auto_dictionary{
private:
    /** holds the size fo the state vector*/
    unsigned int max_index = 0;
    /** vector with free indexes in the state vector that can be used for new states*/
    std::vector<unsigned int> free_index;

    /** map and a vector responsible for translating the state's name to its index and back*/
    std::unordered_map <std::string, unsigned int> _name_index_state;
    std::vector<std::string> _index_name_state;

    /** map and a vector responsible for translating the symbol's name to its index and back*/
    std::unordered_map <std::string, unsigned int> _name_index_alpha;
    std::vector<std::string> _index_name_alpha;

public:
    /***
     * Tries to smooth out the vector by swapping the index of the last elements to one in the free_index
     * @param old           index of the last changed element
     * @param change        new index that was assign to the element
     * @return              true if a change in the vector happened, else false is returned
     */
    bool smooth_vector_state(unsigned int& old, unsigned int& change);

    /***
     * Adds a new state into the dictionary, if there is an element free_index, the new state gets tne index
     *  of the last element in the free_index vector, else it gets the max index of the state vector
     * @param name      name of the state stored in the dictionary
     * @return          return the index given to the state name
     */
    unsigned int add_state(const std::string& name);
    /***
     * Adds the new symbol into the dictionary at the given index, if the index is ouf of range of the vector,
     *  an error is printed on stderr and no state is added
     * @param name      name of the symbol to add to dictionary
     * @param index     index that should be assigned to the symbol
     */
    void add_alpha(const std::string& name, unsigned int index);

    /***
     * Getter of the index of the state, if there is no such state, program exits
     * @param name      the name of the searched state
     * @return          index of the state given by the name
     */
    unsigned int get_state_index(const std::string& name);
    /***
     * Getter of the name of the state, if indexing out of range, program exits
     * @param index     the index of the searched state
     * @return          the name of the state of the given index
     */
    std::string get_state_name(unsigned int index);

    /***
     * Checker whether there exists the state with the given name
     * @param name      the name of the searched state
     * @return          true if there is such state in the dictionary, else returns false
     */
    bool state_exists(const std::string& name);
    /***
     * Checker whether there exists the state with the given index
     * @param index     the index of the searched state
     * @return          true if there is such state in the dictionary, else returns false
     */
    bool state_exists(int index);

    /***
     * Removes the state given by the name from the dictionary, the state is removed from the map,
     *  if the state was the last state in the vector it is simply removed, if not the cell with the index is
     *  set empty and the index is added to the free_index vector
     * @param name      name of the state to be removed
     */
    void remove_state(const std::string& name);
    /***
     * Removes the state given by the index from the dictionary, the state is removed from the map,
     *  if the state was the last state in the vector it is simply removed, if not the cell with the index is
     *  set empty and the index is added to the free_index vector
     * @param index     index of the state to be removed
     */
    void remove_state(const unsigned int& index);

    /***
     * Changes the name of the state on the index to a new name
     * @param index         index of the state that will have the name changed
     * @param new_name      new name of the state on hte given index
     */
    void change_state_name(const unsigned int& index, const std::string& new_name);

    /***
     * Getter of the index of the symbol, if there is no such symbol, program exits
     * @param name      the name of the searched symbol
     * @return          index of the symbol given by the name
     */
    unsigned int get_alpha_index(const std::string& name);
    /***
     * Getter of the name of the symbol, if indexing out of range, program exits
     * @param index     the index of the searched symbol
     * @return          the name of the symbol of the given index
     */
    std::string get_alpha_name(unsigned int index);

    /***
     * Checker whether there exists the symbol with the given name
     * @param name      the name of the searched symbol
     * @return          true if there is such symbol in the dictionary, else returns false
     */
    bool alpha_exists(const std::string& name);

    /***
     * Removes the symbol given by the name from the dictionary, the symbol is removed from the map,
     *  if the symbol was the last symbol in the vector it is simply removed, if not the symbol is swapped
     *  with the last one in the vector and its index is changed in the map too
     * @param name      the name of the symbol to be removed
     */
    void remove_alpha(const std::string& name);
    /***
     * Removes the symbol given by the index from the dictionary, the symbol is removed from the map,
     *  if the symbol was the last symbol in the vector it is simply removed, if not the symbol is swapped
     *  with the last one in the vector and its index is changed in the map too
     * @param index      the index of the symbol to be removed
     */
    void remove_alpha(unsigned int index);

    /***
     * Getter of the highest free index of the state in the automaton, does not have to be equal to the number of states
     * @return              the max_index with the highest free index in the automaton
     */
    [[nodiscard]] unsigned int get_state_size() const;
    /***
     * Checks whether the alphabet of the given dictionary is equal to the this one
     * @param is_same       the dictionary to compare
     * @return              true if the alphabets are equal, else returns false
     */
    bool check_alphabets(auto_dictionary& is_same);

    /***
     * Debug function for printing the contents of the maps of the object
     */
    void print();
    /***
     * Debug function for printing the contents of vectors of the object
     */
    void print_vec();
};


#endif //BAKALARKA_AUTO_DICTIONARY_H
