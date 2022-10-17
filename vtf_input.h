//
// Created by vmvev on 10/9/2022.
//

#ifndef BAKALARKA_VTF_INPUT_H
#define BAKALARKA_VTF_INPUT_H

#include <memory>
#include <fstream>
#include <string>
#include <sstream>
#include "automata.h"

std::shared_ptr <automata> take_input(const std::string& file);

#endif //BAKALARKA_VTF_INPUT_H
