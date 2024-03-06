__kernel void vec_sum_kernel(
    __global double *vector1_in,
    __global double *vector2_in,
    __global double *vector_out,
    int size
) {
    // Each thread gets its own ID
    int id = get_global_id(0);

    // Check if the ID is within the bounds of the vector size
    if (id < size) {
        // Sum the corresponding elements of the two vectors
        vector_out[id] = vector1_in[id] + vector2_in[id];
    }
}
