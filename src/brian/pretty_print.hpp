#ifndef BRIANS_BRAIN_PRETTY_PRINT_HPP
#define BRIANS_BRAIN_PRETTY_PRINT_HPP

#include "memory/standard_grid.hpp"
#include "./algorithm.hpp"

namespace brian_brain {

using print_config = cellato::memory::grids::standard::print_config<bb_cell_state>;

struct bb_pretty_print {
    static print_config get_config() {
        return print_config()
            .with(bb_cell_state::off,   "\033[90m.\033[0m")   // Grey for off
            .with(bb_cell_state::on,    "\033[1;34m@\033[0m") // Blue for firing
            .with(bb_cell_state::dying, "\033[1;33m*\033[0m"); // Yellow for dying
    }
};

} // namespace brian_brain

#endif // BRIANS_BRAIN_PRETTY_PRINT_HPP
