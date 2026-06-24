#ifndef BRIANS_BRAIN_ALGORITHM_HPP
#define BRIANS_BRAIN_ALGORITHM_HPP

#include "core/ast.hpp"

namespace brian_brain {
using namespace cellato::ast;

enum class bb_cell_state {
    off,
    on,
    dying
};

// State constants
using off  = state_constant<bb_cell_state::off>;
using on   = state_constant<bb_cell_state::on>;
using dying = state_constant<bb_cell_state::dying>;

// Integer constants
using c_2 = constant<2>;

// Predicates for cell state checks
using cell_is_off   = p<current_state, equals, off>;
using cell_is_on    = p<current_state, equals, on>;
using cell_is_dying = p<current_state, equals, dying>;

// Count "on" neighbors (Moore neighborhood)
using on_count = count_neighbors<on, moore_8_neighbors>;

// Predicate: exactly two "on" neighbors
using has_two_on_neighbors = p<on_count, equals, c_2>;

// Brian's Brain algorithm
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

} // namespace brian_brain

#endif // BRIANS_BRAIN_ALGORITHM_HPP
