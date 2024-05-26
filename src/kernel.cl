__kernel void add(__global const float* a, __global const float* b, __global float* result) {
    int id = get_global_id(0);
    result[id] = a[id] + b[id];
}