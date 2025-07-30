#ifndef BRIANS_BRAIN_ALGORITHM_HPP
#define BRIANS_BRAIN_ALGORITHM_HPP

#include "core/ast.hpp"

namespace brians_brain {
using namespace cellato::ast;

// Define states for Brian's Brain
// OFF (0), ON (1), DYING (2)
enum class bb_cell_state {
    off,
    on,
    dying
};

// Constants for each state
using off   = state_constant<bb_cell_state::off>;
using on    = state_constant<bb_cell_state::on>;
using dying = state_constant<bb_cell_state::dying>;

// Integer constant for neighbor count
using c_2 = constant<2>;

// Predicates to check current cell state
using cell_is_off   = p<current_state, equals, off>;
using cell_is_on    = p<current_state, equals, on>;
using cell_is_dying = p<current_state, equals, dying>;

// Count ON neighbors in Moore neighborhood
using on_neighbors = count_neighbors<on, moore_8_neighbors>;

// Predicate: exactly two ON neighbors
using has_two_on_neighbors = p<on_neighbors, equals, c_2>;

// Brian's Brain update rule
// 1. ON -> DYING
// 2. DYING -> OFF
// 3. OFF with exactly two ON neighbors -> ON
//    otherwise remains OFF
using bb_algorithm =
    if_< cell_is_on >::then_<
        dying
    >::elif_< cell_is_dying >::then_<
        off
    >::else_< // cell_is_off
        if_< has_two_on_neighbors >::then_<
            on
        >::else_<
            off
        >
    >;

} // namespace brians_brain

#endif // BRIANS_BRAIN_ALGORITHM_HPP
