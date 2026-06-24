#ifndef BRIAN_BRAIN_CONFIG_HPP
#define BRIAN_BRAIN_CONFIG_HPP

#include "./algorithm.hpp"
#include "./pretty_print.hpp"
#include "./data_init.hpp"
#include "./reference_implementation.hpp"
#include "memory/state_dictionary.hpp"

namespace brian_brain {

struct config {
    static constexpr auto name = "brians-brain";

    using algorithm = bb_algorithm;

    using cell_state = bb_cell_state;
    using state_dictionary = cellato::memory::grids::state_dictionary<
        cell_state::off, cell_state::on, cell_state::dying>;

    using pretty_print = bb_pretty_print;

    using reference_implementation = reference::runner;

    struct input {
        using random = bb_random_init;
    };
};

} // namespace brian_brain

#endif // BRIAN_BRAIN_CONFIG_HPP
