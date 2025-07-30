#ifndef BRIAN_BRAIN_ALGORITHM_HPP
#define BRIAN_BRAIN_ALGORITHM_HPP

#include "core/ast.hpp"

namespace brian_brain {
using namespace cellato::ast;

// Brian's Brain cell states
enum class bb_cell_state {
    off,
    on,
    dying
};

// Constants for states
using off = state_constant<bb_cell_state::off>;
using on  = state_constant<bb_cell_state::on>;
using dying = state_constant<bb_cell_state::dying>;

// Integer constants
using c_2 = constant<2>;

// Predicates for checking current state
using cell_is_off   = p<current_state, equals, off>;
using cell_is_on    = p<current_state, equals, on>;
using cell_is_dying = p<current_state, equals, dying>;

// Count neighbors that are "on" using Moore neighborhood
using on_count = count_neighbors<on, moore_8_neighbors>;
using has_two_on_neighbors = p<on_count, equals, c_2>;

// Brian's Brain rules:
// 1. On  -> Dying
// 2. Dying -> Off
// 3. Off -> On if exactly two on neighbors, else Off
using bb_algorithm =
    if_< cell_is_on >::then_<
        dying
    >::elif_< cell_is_dying >::then_<
        off
    >::else_<
        if_< has_two_on_neighbors >::then_<
            on
        >::else_<
            off
        >
    >;

} // namespace brian_brain

#endif // BRIAN_BRAIN_ALGORITHM_HPP
