#ifndef BRIAN_BRAIN_DATA_INIT_HPP
#define BRIAN_BRAIN_DATA_INIT_HPP

#include <vector>
#include <tuple>
#include "memory/grid_utils.hpp"
#include "experiments/run_params.hpp"
#include "./algorithm.hpp"

namespace brian_brain {

struct bb_random_init {
    static std::vector<bb_cell_state> init(cellato::run::run_params& params) {
        std::vector<bb_cell_state> initial_state(params.x_size * params.y_size);

        std::vector<std::tuple<bb_cell_state, double>> probabilities = {
            {bb_cell_state::off,   0.70},
            {bb_cell_state::on,    0.15},
            {bb_cell_state::dying, 0.15}
        };

        cellato::memory::grids::utils::generate_random_grid(
            initial_state,
            params.y_size, params.x_size,
            probabilities,
            params.seed
        );

        return initial_state;
    }
};

} // namespace brian_brain

#endif // BRIAN_BRAIN_DATA_INIT_HPP
