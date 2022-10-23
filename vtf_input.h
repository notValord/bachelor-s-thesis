/*
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 18.10 2022
* Subject: Bachelor's thesis
*/

#ifndef BAKALARKA_VTF_INPUT_H
#define BAKALARKA_VTF_INPUT_H

#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include "automata.h"

/**
 * Parses the line from the file for states as well as initial and accept states using string stream
 * @param line  input line read from the file
 * @param ret   vector returned back with parsed states from the line
 */
void parse_line(const std::string& line, std::vector <std::string>& ret);

/**
 * Parses the transition line for the automata, when in correct format
 *          creates a new transition withing the new_auto
 * @param line      input line read from the file
 * @param new_auto  pointer to the automata
 */
void parse_transition(const std::string& line, const std::shared_ptr <automata>& new_auto);

/**
 * Takes input from the file and parse the data into newly created automata object
 * @param file  file in vtf format with input automata
 * @return      pointer to created automata object, if there was a failure nullptr is returned
 */
std::shared_ptr <automata> take_input(const std::string& file);

#endif //BAKALARKA_VTF_INPUT_H
