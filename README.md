# Efficient Reduction of Finite Automata
#### Author: Veronika Molnárová, xmolna08
#### Project: Bachelor's thesis, 2022/2023
---
---
## Project description
This project implements a program in C++ 20 with the main focus on the reduction of finite state automata. The application takes an input automaton in a `.vtf` file and tries to find its smaller equivalent that is after successful reduction saved into another `.vtf` file. The application implements the reduction by finding *minimal deterministic automaton* (DFA) by either *basic Hopcroft's algorithm* or *Brzozowski's algorithm* and direct reduction of non-deterministic automata (NFA), namely *reduction through a relation of simulation* and *reduction by transformation into canonical residual automaton*. On top of that, the application implements a new possibility of automata *reduction by utilizing SAT solvers* for finding the minimal DFA or minimal NFA and QBF solvers for finding the minimal NFA.


## Requirements

This project was run and tested on `WSL2` with specifications `Ubuntu 20.04.5 LTS (GNU/Linux 5.10.16.3-microsoft-standard-WSL2 x86_64)`. The requirements for this project include:
- make
- g++ at least version 9 for the correct compilation for C++20

## Compilation
For the compilation of this project use the attached `Makefile` with the command `make` in the root directory. That should compile the source files and create a binary file `fsa_red` in the `build/` directory. If the directory doesn't exist, it will be created.  

All of the tested solvers are included in this project, mostly as runnable binary files. Only the `qute` needs to be firstly initialized as a submodule by running the git command:

```
git submodule update --init --recursive
```
The compilation of this solver fo into the directory `solvers/qute` and follow the description of compilation in README file in its repository. After successful compilation you should end up with a binary file `qute` in the directory `solvers/qute`.

## How to run the project
The running of this project can be divided into two sections:
- reduction through known algorithms (minimal DFA, simulation, residual)
- reduction utilizing SAT and QBF solvers

All of the options for the program are listed when running the program with wrong arguments or run the application with `--help` option.

### Reduction through known algorithms
The usage of program for the known reduction types is:

```
./fsa_red {-t reduction_type} {-f vtf_file} [--debug]
```

where
- `reduction type` is one of the options: 
    - `hop` -- for minimal DFA through Hopcroft's algorithm
    - `brz` -- for minimal DFA through Brzozozwski's algorithm
    - `sim` -- for reduction utilizing the relation of simulation
    - `rez` -- for reduction by transformation into canonical residual automaton
    - `all` -- for running all of the reductions, can be only used together with the option `--debug`
- `vtf_file` is the path to the `.vtf` file with the input automaton on which the reduction will be done
- `--debug` is used for testing when the correctness of the reduction is checked by language equivalence of the automata's languages through antichains. The information containing the name of the input file, number of states of the original automaton, number of states of the reduction automaton, time required for reduction, the result of the language check, and time required for the language check is printed onto the output separated by a semicolon. If all reductions were selected, the reduction will be run sequentially in order of the list above and the debug information will be printed in a single line, one reduction after another.

After the reduction is done the result automaton is saved into a `.vtf` file in the directory from where the program is run with the same name as the input automaton. This doesn't apply when the `--debug` option is selected.

For the testing of the reduction algorithm, is also attached a shell script `test_all.sh` in the `build/` directory. This script will run all files with automata in the directory `input_automata/1/` one after another for all reductions with the debug option.

### Reduction utilizing SAT and QBF solvers
As the reduction utilizing SAT and QBF solvers requires alternating running of our program and solver the bash scripts are provided for the reduction. The bash scripts included in the `solvers/` directory are:

- `unit_test_sat.sh` and `unit_test_qbf.sh` -- for testing the performance of selected solvers (`MiniSat` and `Kissat` for SAT solvers and `CAQE`, `DepQBF` and `qute` for QBF solvers). The testing is done by creating a set of clauses for representing an automaton with the given number of states and one of the selected sizes of the input sets of words (S - small, M - medium, L - large, XS - extra small only for QBF solvers). The solvers are then timed for how long it takes them to solve the given formula. Results are printed onto the output. General usage of the scripts, where N is the number of states for the automaton that the clause is representing and size is one of the sizes for the alphabet, is: 
```
./unit_test_sat.sh N size
./unit_test_qbf.sh N size
```

The output of the stript should look like this:
```
./unit_test_sat.sh 2 M
States: 2; Symbols: 2; Accepted words:  0,0 1,1 0 1,1 1 0 1,1 1 0 1 1; Rejected words:  1,1 1,0 0 0,0 1 0 1,1 0 1 0 1
======================================================================
Generate clauses time dfa no header:          0.104
Variables:                                    72
Clauses:                                      457
MiniSat time:                                 0.030

Generate clauses time dfa with header:        0.017
Clauses:                                      9
Kissat time:                                  0.022

-----------------------------------------------------------------------
```

- `run_minisat.sh`, `run_kissat.sh`, `run_nfa_minisat.sh`, and `run_qbf.sh` -- for running the reduction algorithm utilizing SAT and QBF solvers. The scripts run the reductions minimal DFA using `Minisat`, minimal DFA using `Kissat`, minimal NFA using `MiniSat`, and minimal NFA using `CAQE` in the corresponding order. The single input argument is allowed in a form of a path to a `.vtf` file with the input automaton that is going to be reduced. If no argument is given, the reduction will be done on an `input_automata/test_auto/armcNFA_inclTest_6.vtf` automaton set as the basic automaton in the script. The automata provided for testing are located in the directory `input_automata/test_auto/`. The reduction algorithm consists of generating the clauses representing an automaton based on the size of the alphabet and samples of words that the automaton should accept and reject. The words are updated until an equivalent automaton is produced. These algorithms can be run only on small automata to around 5-6 states, else the reduction can last hours. The scripts are meant to be run from the `solvers/` directory and the reduced automaton is saved into a `sat|sat_min|qbf_min.vtf` file in the running directory. The running of the script may look like this:
 
```
./run_nfa_minisat.sh
Got info about the automaton:
      Reduction of states to: 2, size of the alphabet: 2
      Accepted words:0 0 0
      Rejected words:,0,1,0 0,0 1,0 0 1
--[AUTO LOG]--              INITIAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - UNSATISFIABLE, INCREASING NUMBER OF STATES - 3
--[AUTO LOG]--              INITIAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - UNSATISFIABLE, INCREASING NUMBER OF STATES - 4
--[AUTO LOG]--              INITIAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE
--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE *NOT* EQUAL
--[AUTO LOG]--              ADDING NEW EXAMPLES, ACCEPTING:0 0 0 0; REJECTING:
--[AUTO LOG]--              ADDITIONAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE
--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE *NOT* EQUAL
--[AUTO LOG]--              ADDING NEW EXAMPLES, ACCEPTING:0 0 0 1; REJECTING:
--[AUTO LOG]--              ADDITIONAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE
--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE EQUAL
```

- `test_minisat.sh`, `test_kissat.sh`, `test_nfa_minisat.sh`, and `test_qbf.sh` -- for testing the reduction using SAT and QBF solvers. The scripts run the same algorithms as the scripts for running the algorithm, but on top of that information about the reduction is noted and then printed onto the stdout. The running of the script may look like this:

```
./test_nfa_minisat.sh
Got info about the automaton:
      Reduction of states to: 2, size of the alphabet: 2
      Accepted words:0 0 0
      Rejected words:,0,1,0 0,0 1,0 0 1
--[AUTO LOG]--              INITIAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - UNSATISFIABLE, INCREASING NUMBER OF STATES - 3
--[AUTO LOG]--              INITIAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - UNSATISFIABLE, INCREASING NUMBER OF STATES - 4
--[AUTO LOG]--              INITIAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE
--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE *NOT* EQUAL
--[AUTO LOG]--              ADDING NEW EXAMPLES, ACCEPTING:0 0 0 0; REJECTING:
--[AUTO LOG]--              ADDITIONAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE
--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE *NOT* EQUAL
--[AUTO LOG]--              ADDING NEW EXAMPLES, ACCEPTING:0 0 0 1; REJECTING:
--[AUTO LOG]--              ADDITIONAL GENERATION DONE
--[SOLVER LOG]--            COMPUTATION DONE - SATISFIABLE
--[AUTO LOG]--              COMPARATION DONE, AUTOMATA ARE EQUAL

Iterations:                   5
Accept words:                 3
Reject words:                 6
Variables:                    2344
Clauses:                      16296
Get info time:                0.029
Generate clauses time:        3.634
Compare time:                 .127
Solver time:                  .184
```

## Contents of this repository
The contents of this repository include:
- `input_automata/` -- directory with the testing automata files in `.vtf` format
- `src/` -- directory with the source files
- `build/` -- directory with the build binary file and a testing script `test_all.sh`
- `solvers/` -- directory with the bash scripts for the reduction using SAT and QBF solvers with the solvers themselves
- `thesis/` -- directory with the thesis about this project
- `Makefile` -- used for the compilation of the program
- `README` -- this documentation