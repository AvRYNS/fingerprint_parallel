int __sum(__local float *values) {
    const int local_id = get_local_id(0);
    const int group_size = get_local_size(0);

    barrier(CLK_LOCAL_MEM_FENCE);

    // parallel reduction
    for (unsigned int stride = group_size >> 1; stride > 0; stride >>= 1) {
        if (local_id < stride) {
            values[local_id] += values[local_id + stride];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    return values[0];
}

/**
 * @brief Calculate sum of elements in a work group.
 */
__kernel void sum(__global uchar *input, __global float *output,
                  __local float *tmp, int inputSize) {
    int global_id = get_global_id(0);
    int local_id = get_local_id(0);
    int group_id = get_group_id(0);

    tmp[local_id] = global_id < inputSize ? input[global_id] : 0;

    float result = __sum(tmp);

    // write to global mem
    if (local_id == 0) {
        output[group_id] = result;
    }
}

/**
 * @brief Calculate sum of elements^2 in a work group.
 */
__kernel void squareSum(__global uchar *input, __global float *output,
                        __local float *tmp, int inputSize) {
    int global_id = get_global_id(0);
    int local_id = get_local_id(0);
    int group_id = get_group_id(0);

    float val = input[global_id];
    val *= val;

    tmp[local_id] = global_id < inputSize ? val : 0;

    float result = __sum(tmp);

    // write to global mem
    if (local_id == 0) {
        output[group_id] = result;
    }
}