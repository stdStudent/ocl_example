__kernel void mul_kernel(
    __global double *matrix1_in,
    __global double *matrix2_in,
    __global double *matrix_out,
    int size,
    int singleThread
) {
    int start = 0;
    int end = size * size;

    // If not singleThread, only compute for the current work item
    if (!singleThread) {
        start = get_global_id(0);
        end = start + 1;
    }

    for (int id = start; id < end; ++id) {
        double value = 0;
        int row, col;

        row = id / size;
        col = id % size;
        for (int i = 0; i < size; ++i)
            value += matrix2_in[(i * size) + col] * matrix1_in[(row * size) + i];

        matrix_out[id] = value;
    }
}
