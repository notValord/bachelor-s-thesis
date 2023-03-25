
#ifndef BAKALARKA_TSEITSEN_H
#define BAKALARKA_TSEITSEN_H

#include <iostream>
#include <vector>
#include <queue>

// defining indexes of logic operators for tseitsen
#define AND -1
#define OR -2
#define NOT -3

unsigned int dynamic_tseitsen(const std::vector <int>& input, unsigned int max_index);
unsigned int tseitsen(const std::vector <int>& input, unsigned int max_index);

#endif //BAKALARKA_TSEITSEN_H
