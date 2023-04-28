/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 15.04.2023
* Subject: Bachelor's thesis - 2st part
*/

#ifndef BAKALARKA_TSEITSEN_H
#define BAKALARKA_TSEITSEN_H

#include <iostream>
#include <vector>
#include <queue>

// defining indexes of logic operators used in an input vector for tseitsen
#define AND -1
#define OR -2
#define NOT -3

/***
 *  Transformation from DNF to CNF - optimisation used to reduce the number of created variables
 *  by creating one and gate for each clause till disjunction and one or gate
 *
 * @param input     vector of variables (positive numbers) separated by indexes of logical operators (negative numbers)
 * @param max_index viable number from which new variables can be generated
 * @return          new max_index
 */
unsigned int dynamic_tseitsen(const std::vector <int>& input, unsigned int max_index);

/***
 * Transformation from DNF to CNF using Tseitin transformation, creates a logical circuit
 *  and setting the output to be true
 *
 * @param input     vector of variables (positive numbers) separated by indexes of logical operators (negative numbers)
 * @param max_index viable number from which new variables can be generated
 * @return          new max_index
 */
unsigned int tseitsen(const std::vector <int>& input, unsigned int max_index);

#endif //BAKALARKA_TSEITSEN_H
