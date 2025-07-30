#include "./reference_implementation.hpp"
#include <cuda_runtime.h>
#include "traversers/cuda_utils.cuh"

namespace brian_brain::reference {

__global__ void brian_kernel(const bb_cell_state* current, bb_cell_state* next,
                             int width, int height) {
    int x = blockIdx.x * blockDim.x + threadIdx.x + 1;
    int y = blockIdx.y * blockDim.y + threadIdx.y + 1;

    const int idx = y * width + x;

    bb_cell_state cell_state = current[idx];
    bb_cell_state next_state = cell_state;

    if (cell_state == bb_cell_state::on) {
        next_state = bb_cell_state::dying;
    } else if (cell_state == bb_cell_state::dying) {
        next_state = bb_cell_state::off;
    } else {
        int on_neighbors =
            (current[(y - 1) * width + (x - 1)] == bb_cell_state::on) +
            (current[(y - 1) * width +  x    ] == bb_cell_state::on) +
            (current[(y - 1) * width + (x + 1)] == bb_cell_state::on) +
            (current[ y      * width + (x - 1)] == bb_cell_state::on) +
            (current[ y      * width + (x + 1)] == bb_cell_state::on) +
            (current[(y + 1) * width + (x - 1)] == bb_cell_state::on) +
            (current[(y + 1) * width +  x    ] == bb_cell_state::on) +
            (current[(y + 1) * width + (x + 1)] == bb_cell_state::on);
        if (on_neighbors == 2) {
            next_state = bb_cell_state::on;
        } else {
            next_state = bb_cell_state::off;
        }
    }

    next[idx] = next_state;
}

void runner::run_kernel(int steps) {
    if (!d_current || !d_next) {
        init_cuda();
    }

    dim3 block_size(_block_size_x, _block_size_y);

    auto _x_size_threads = _x_size - 2;
    auto _y_size_threads = _y_size - 2;

    dim3 grid_dim((_x_size_threads + block_size.x - 1) / block_size.x,
                 (_y_size_threads + block_size.y - 1) / block_size.y);

    for (int i = 0; i < steps; i++) {
        brian_kernel<<<grid_dim, block_size>>>(d_current, d_next, _x_size, _y_size);
        bb_cell_state* temp = d_current;
        d_current = d_next;
        d_next = temp;
    }

    CUCH(cudaDeviceSynchronize());
}

} // namespace brian_brain::reference
