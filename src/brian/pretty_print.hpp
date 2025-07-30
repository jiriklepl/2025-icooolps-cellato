#ifndef BRIAN_BRAIN_PRETTY_PRINT_HPP
#define BRIAN_BRAIN_PRETTY_PRINT_HPP

#include "memory/standard_grid.hpp"
#include "./algorithm.hpp"

namespace brian_brain {

using print_config = cellato::memory::grids::standard::print_config<bb_cell_state>;

struct bb_pretty_print {
    static print_config get_config() {
        return print_config()
            .with(bb_cell_state::off,   "\033[90m.\033[0m")   // grey for off
            .with(bb_cell_state::on,    "\033[1;36m@\033[0m") // cyan for on
            .with(bb_cell_state::dying, "\033[1;34m+\033[0m"); // blue for dying
    }
};

} // namespace brian_brain

#endif // BRIAN_BRAIN_PRETTY_PRINT_HPP
