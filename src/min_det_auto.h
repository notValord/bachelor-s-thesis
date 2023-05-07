//
/**
* Project name: Effective reduction of Finite Automata
* Author: Veronika Molnárová
* Date: 06.05.2023
* Subject: Bachelor's thesis - 1st part
*/

#ifndef BAKALARKA_MIN_DET_AUTO_H
#define BAKALARKA_MIN_DET_AUTO_H

#include "det_auto.h"

class power_element;
class min_auto;

/***
 * Creates a minimal DFA through Hopcroft's algorithm
 * @param nfa           the initial NFA
 * @return              a newly created instance of minimal DFA
 */
std::shared_ptr <det_auto> det_n_min(const std::shared_ptr <det_auto>& nfa);

/***
 * Creates a minimal DFA through Brzozowski's algorithm
 * @param nfa           the initial NFA
 * @return              a newly created instance of minimal DFA
 */
std::shared_ptr <det_auto> brzozowski(const std::shared_ptr <det_auto>& nfa);

/***
 * Creates a minimal DFA from the given DFA through basic Hopcroft's algorithm by spliting the final
 *  and non-final partitions
 */
void minimal_dfa(const std::shared_ptr <min_auto>&);

/***
 * Checks whether there is a closure with the same behaviour as the adding power element, if so the adding element is
 *  added to the closure, else a new closure is creates in the power_set with the element in it
 * @param adding            power element that is being checked an added
 * @param power_set         the set of closures defining the splitting
 */
void insert_pow_set(const std::shared_ptr <power_element>& adding,
                    std::vector <std::shared_ptr<power_element>>& power_set);

/***
 * Adds every element in the closure to a help table where it is given to which closure the element belongs to
 * @param help_table        help table for future optimization
 * @param previous          the old closure set
 */
void init_power_hash(std::vector <ptr_state_vector>& help_table,
                     const std::vector <std::shared_ptr<power_element>>& previous);

/// Checks whether two vectors of pointers to power element are equal
bool is_eq(std::vector <std::shared_ptr <power_element>>&, std::vector <std::shared_ptr <power_element>>&);

/***
 * Class for the power element for the Hopcroft's algorithm for finding the minimal DFA,
 *  represent the closures used
 */
class power_element{
private:
    /** Set of states of the closure */
    ptr_state_vector same_set;
    /** Behaviour of the element against different closures*/
    std::vector <ptr_state_vector> transition;

public:
    /***
     * Operator defining the equivalence of two power_elements
     * @param first         first element
     * @param second        second element
     * @return              true if the elements have the same set of states, else false is returned
     */
    friend bool operator==(const power_element& first, const power_element& second){
        if (first.same_set == second.same_set){
            return true;
        }
        return false;
    }

    /***
     * Operator defining the difference of two power_elements
     * @param first         first element
     * @param second        second element
     * @return              true if the elements have different set of states, else false is returned
     */
    friend bool operator!=(const power_element& first, const power_element& second){
        if (first.same_set != second.same_set){
            return true;
        }
        return false;
    }

    /***
     * Constructor  - creates an element with from a single state and its behaviour against the partitions
     * @param state         the single state of power element
     * @param trans         the behaviour of the state
     */
    power_element(const std::shared_ptr <auto_state>& state, std::vector<ptr_state_vector>& trans);

    /***
     * Constructor  - creates only a set of states without defining their behaviour
     * @param set           the set of states of the power element
     */
    explicit power_element(ptr_state_vector set);

    /***
     * Constructor  - creates an empty element
     */
    power_element();

    /***
     * Getter of the set of states of the power element
     * @return              the same_set attribute with the states of the element
     */
    ptr_state_vector& get_set();
    /***
     * Getter of the transitions defining the behaviour of the states agains partitions
     * @return              transitions attribute with the behaviour of the power element
     */
    std::vector<ptr_state_vector>& get_trans();

    /***
     * Adds a new state to the element
     * @param state         state to be added
     */
    void add_state(const std::shared_ptr <auto_state>& state);
    /***
     * Setter of the transitions of the element with the behaviour
     * @param new_trans         new transitions that the attribute will be set to
     */
    void set_trans(const std::vector<ptr_state_vector>& new_trans);
};

/***
 * Class for the minimal DFA defining the methods required for finding the minimal equivalent,
 * inherits from the deterministic automaton
 */
class min_auto : public det_auto {
public:
    /***
     * Constructor  - initializes every attribute with the values from the given base automaton
     * Is used for converting!!!
     * @param base          the base automaton with the values for the automaton
     */
    min_auto(det_auto& base);

    /***
     * Creates the minimal automaton based on the provided power_set with the closures defining the new states of the automaton
     * @param power_set         set of closures representing the new states of the automaton
     */
    void min_power(const std::vector <std::shared_ptr<power_element>>& power_set);
    /***
     * Initializes the power set with the final and non-final closures
     * @param pow_set           set of power elements to be initializes
     */
    void init_power_set(std::vector <std::shared_ptr<power_element>>& pow_set);
    /***
     * A single iteration of the splitting of the closures for minimal DFA
     * @param previous          old set of closures that is being split
     * @param next              newly creates set of closures representing the new iteration of the closures
     */
    void split_power_state(std::vector <std::shared_ptr <power_element>>& previous,
                           std::vector <std::shared_ptr <power_element>>& next);
};

#endif //BAKALARKA_MIN_DET_AUTO_H
