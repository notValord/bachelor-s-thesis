cmake_minimum_required(VERSION 3.0)
project(Bakalarka)

set(CMAKE_CXX_STANDARD 20)

add_executable(Bakalarka automata.cpp vtf_input.cpp vtf_input.h main.cpp blbosti.cpp blbosti.h)
