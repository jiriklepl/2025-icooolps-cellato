#ifndef BRIANS_BRAIN_REFERENCE_IMPLEMENTATION_HPP
#define BRIANS_BRAIN_REFERENCE_IMPLEMENTATION_HPP

#include <vector>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include "./algorithm.hpp"
#include "experiments/run_params.hpp"
#include "traversers/cuda_utils.cuh"

namespace brian_brain::reference {

struct runner {
    void init(const bb_cell_state* grid,
              const cellato::run::run_params& params = cellato::run::run_params()) {
        _x_size = params.x_size;
        _y_size = params.y_size;
        _block_size_x = params.cuda_block_size_x;
        _block_size_y = params.cuda_block_size_y;
        _current_grid.resize(_x_size * _y_size);
        _next_grid.resize(_x_size * _y_size);

        if (params.device == "CUDA") {
            if ((_x_size - 2) % _block_size_x != 0 || (_y_size - 2) % _block_size_y != 0) {
                std::cerr << "Grid size must be divisible by block size.\n";
                throw std::runtime_error("Invalid grid size for CUDA traverser.");
            }
        }

        if (grid) {
            for (std::size_t i = 0; i < _x_size * _y_size; ++i) {
                _current_grid[i] = grid[i];
                _next_grid[i] = grid[i];
            }
        }
    }

    void init_cuda() {
        const size_t grid_size = _x_size * _y_size * sizeof(bb_cell_state);
        CUCH(cudaMalloc(&d_current, grid_size));
        CUCH(cudaMalloc(&d_next, grid_size));
        CUCH(cudaMemcpy(d_current, _current_grid.data(), grid_size, cudaMemcpyHostToDevice));
    }

    void run(int steps) {
        for (int step = 0; step < steps; ++step) {
            for (std::size_t y = 1; y < _y_size - 1; ++y) {
                for (std::size_t x = 1; x < _x_size - 1; ++x) {
                    int on_neighbors =
                        (_current_grid[(y - 1) * _x_size + (x - 1)] == bb_cell_state::on) +
                        (_current_grid[(y - 1) * _x_size +  x     ] == bb_cell_state::on) +
                        (_current_grid[(y - 1) * _x_size + (x + 1)] == bb_cell_state::on) +
                        (_current_grid[ y      * _x_size + (x - 1)] == bb_cell_state::on) +
                        (_current_grid[ y      * _x_size + (x + 1)] == bb_cell_state::on) +
                        (_current_grid[(y + 1) * _x_size + (x - 1)] == bb_cell_state::on) +
                        (_current_grid[(y + 1) * _x_size +  x     ] == bb_cell_state::on) +
                        (_current_grid[(y + 1) * _x_size + (x + 1)] == bb_cell_state::on);

                    bb_cell_state current = _current_grid[y * _x_size + x];
                    bb_cell_state next;

                    if (current == bb_cell_state::on) {
                        next = bb_cell_state::dying;
                    } else if (current == bb_cell_state::dying) {
                        next = bb_cell_state::off;
                    } else {
                        if (on_neighbors == 2) {
                            next = bb_cell_state::on;
                        } else {
                            next = bb_cell_state::off;
                        }
                    }

                    _next_grid[y * _x_size + x] = next;
                }
            }
            _current_grid.swap(_next_grid);
        }
    }

    void run_on_cuda(int steps) {
        if (!d_current || !d_next) {
            init_cuda();
        }
        run_kernel(steps);
    }

    std::vector<bb_cell_state> fetch_result() {
        if (d_current) {
            const size_t grid_size = _x_size * _y_size * sizeof(bb_cell_state);
            CUCH(cudaMemcpy(_current_grid.data(), d_current, grid_size, cudaMemcpyDeviceToHost));
            CUCH(cudaFree(d_current));
            CUCH(cudaFree(d_next));
            d_current = nullptr;
            d_next = nullptr;
        }
        return _current_grid;
    }

    ~runner() {
        if (d_current) {
            cudaFree(d_current);
            d_current = nullptr;
        }
        if (d_next) {
            cudaFree(d_next);
            d_next = nullptr;
        }
    }

private:
    std::size_t _x_size{}, _y_size{};
    int _block_size_x = 16;
    int _block_size_y = 16;
    std::vector<bb_cell_state> _current_grid;
    std::vector<bb_cell_state> _next_grid;

    bb_cell_state* d_current = nullptr;
    bb_cell_state* d_next = nullptr;

    void run_kernel(int steps);
};

} // namespace brian_brain::reference

#endif // BRIANS_BRAIN_REFERENCE_IMPLEMENTATION_HPP
