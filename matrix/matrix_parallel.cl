__kernel void mul_kernel(
    __global double *matrix1_in,
    __global double *matrix2_in,
    __global double *matrix_out,
    int size
) {
    // The kernel allows for a more parallelizable implementation,
    // where each entry of the resulting matrix is calculated by a single kernel
    int id = get_global_id(0);
    double value = 0;
    int row, col;

    // The kernel assigns a row of the first operand and a column of the second operand that is computed
    // (multiplying its entries and adding them together) to achieve the final result for that specific entry
    // in the resulting matrix
    row = id / size;
    col = id % size;
    for (int i = 0; i < size; ++i)
        value += matrix2_in[(i * size) + col] * matrix1_in[(row * size) + i];

    matrix_out[id] = value;
}