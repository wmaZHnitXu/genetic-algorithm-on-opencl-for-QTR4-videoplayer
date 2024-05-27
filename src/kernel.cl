__kernel void add(__global const int* a, __global const int* b, __global int* result) {
    int id = get_global_id(0);
    result[id] = a[id] + b[id];
}