#include "./reference_implementation.hpp"
#include <cuda_runtime.h>
#include "traversers/cuda_utils.cuh"

namespace brian_brain::reference {

__global__ void bb_kernel(const bb_cell_state* current, bb_cell_state* next,
                          int width, int height) {
    int x = blockIdx.x * blockDim.x + threadIdx.x + 1;
    int y = blockIdx.y * blockDim.y + threadIdx.y + 1;

    int idx = y * width + x;

    int on_neighbors =
        (current[(y - 1) * width + (x - 1)] == bb_cell_state::on) +
        (current[(y - 1) * width + x] == bb_cell_state::on) +
        (current[(y - 1) * width + (x + 1)] == bb_cell_state::on) +
        (current[y * width + (x - 1)] == bb_cell_state::on) +
        (current[y * width + (x + 1)] == bb_cell_state::on) +
        (current[(y + 1) * width + (x - 1)] == bb_cell_state::on) +
        (current[(y + 1) * width + x] == bb_cell_state::on) +
        (current[(y + 1) * width + (x + 1)] == bb_cell_state::on);

    bb_cell_state state = current[idx];

    if (state == bb_cell_state::on) {
        next[idx] = bb_cell_state::dying;
    } else if (state == bb_cell_state::dying) {
        next[idx] = bb_cell_state::off;
    } else {
        next[idx] = (on_neighbors == 2) ? bb_cell_state::on : bb_cell_state::off;
    }
}

void runner::run_kernel(int steps) {
    if (!d_current || !d_next) {
        init_cuda();
    }

    dim3 block_size(_block_size_x, _block_size_y);
    auto x_threads = _x_size - 2;
    auto y_threads = _y_size - 2;
    dim3 grid_dim((x_threads + block_size.x - 1) / block_size.x,
                  (y_threads + block_size.y - 1) / block_size.y);

    for (int i = 0; i < steps; ++i) {
        bb_kernel<<<grid_dim, block_size>>>(d_current, d_next, _x_size, _y_size);
        bb_cell_state* tmp = d_current;
        d_current = d_next;
        d_next = tmp;
    }

    CUCH(cudaDeviceSynchronize());
}

} // namespace brian_brain::reference
