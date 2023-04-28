/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 18.10.2022
* Subject: Bachelor's thesis - 1st part
*/

#ifndef BAKALARKA_VTF_INPUT_H
#define BAKALARKA_VTF_INPUT_H

#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include "det_auto.h"

/**
 * Parses the line from the file for states as well as initial and accept states using string stream
 * @param line  input line read from the file
 * @param ret   vector returned back with parsed states from the line
 */
void parse_line(const std::string& line, std::vector <std::string>& ret);

/**
 * Parses the transition line for the automaton, when in correct format
 *          creates a new transition withing the new_auto
 * @param line      input line read from the file
 * @param new_auto  pointer to the automaton
 */
void parse_transition(const std::string& line, const std::shared_ptr <det_auto>& new_auto);

/**
 * Takes input from the file and parse the data into newly created automaton object
 * @param file  file in vtf format with input automaton
 * @return      pointer to created automaton object, if there was a failure nullptr is returned
 */
std::shared_ptr <det_auto> take_input(const std::string& file);

#endif //BAKALARKA_VTF_INPUT_H
