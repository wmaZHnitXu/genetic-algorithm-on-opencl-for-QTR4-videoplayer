__kernel void mse_sum_between_two_matrices(__global int* A, __global int* B, __global int* result, int width, int height) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    float err = 0;

    if (x < width && y < height) {
        int index = y * width + x;
        int a_elem = A[index];
        int b_elem = B[index];
        int current_channels[4];
        int target_channels[4];
        
        current_channels[0] = (a_elem >> 24) & 0xFF;
        current_channels[1] = (a_elem >> 16) & 0xFF;
        current_channels[2] = (a_elem >> 8) & 0xFF;
        current_channels[3] = a_elem & 0xFF;
        
        target_channels[0] = (b_elem >> 24) & 0xFF;
        target_channels[1] = (b_elem >> 16) & 0xFF;
        target_channels[2] = (b_elem >> 8) & 0xFF;
        target_channels[3] = b_elem & 0xFF;

        int diff[4];
        
        for (int i = 0; i < 4; i++) {
            diff[i] = current_channels[i] - target_channels[i];
            diff[i] = diff[i] * diff[i];
        }

        err = sqrt((float)(diff[0] + diff[1] + diff[2] + diff[3]));
        atomic_add(result, (err * err) * 0.1f);
    }
}

uint rand(uint* seed) {
    uint hi = *seed / 127773;
    uint lo = *seed % 127773;
    uint x = 16807 * lo - 2836 * hi;
    if (x < 0) x += 0x7fffffff;
    return *seed = x;
}

float eval_rect(int x, int y, int w, int h, int* pcolor, __global int* current, __global int* target, int width) {

    int avg[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    /*
    Dymajy, tyt mojno bydet kak nibyd' optimizirovat' wtobi ne naxodit' sredniy cvet,
    a srazy witat' dispersijy cveta i kakto iz nee vi4lenyat' mse ny a poka v dva proxoda poly4aeca
    */

    int px_count = w * h;

    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            int index = i * width + j;
            int b_elem = target[index];

            avg[0] += ((b_elem >> 24) & 0xFF);
            avg[1] += ((b_elem >> 16) & 0xFF);
            avg[2] += ((b_elem >> 8) & 0xFF);
            avg[3] += (b_elem & 0xFF);
        }
    }

    int avg_channels[4];
    avg_channels[0] = 0xFF;
    for (int i = 1; i < 4; i++) {
        avg_channels[i] = avg[i] / px_count;
        pcolor[i-1] = avg_channels[i];
    }

    float prev_sum = 0.0f;
    float new_sum = 0.0f;
    
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            int index = i * width + j;
            int a_elem = current[index];
            int b_elem = target[index];

            int current_channels[4];
            int target_channels[4];
            
            current_channels[0] = (a_elem >> 24) & 0xFF;
            current_channels[1] = (a_elem >> 16) & 0xFF;
            current_channels[2] = (a_elem >> 8) & 0xFF;
            current_channels[3] = a_elem & 0xFF;
            
            target_channels[0] = (b_elem >> 24) & 0xFF;
            target_channels[1] = (b_elem >> 16) & 0xFF;
            target_channels[2] = (b_elem >> 8) & 0xFF;
            target_channels[3] = b_elem & 0xFF;

            int prev_diff[4];
            int new_diff[4];

            for (int k = 0; k < 4; k++) {
                prev_diff[k] = current_channels[k] - target_channels[k];
                new_diff[k] = avg_channels[k] - target_channels[k];
                
                prev_diff[k] = prev_diff[k] * prev_diff[k];
                new_diff[k] = new_diff[k] * new_diff[k];
            }

            float prev_err = (float)(prev_diff[0] + prev_diff[1] + prev_diff[2] + prev_diff[3]);
            float new_err = (float)(new_diff[0] + new_diff[1] + new_diff[2] + new_diff[3]);
            prev_sum += prev_err * 0.1f;
            new_sum += new_err * 0.1f;
        }
    }

    return prev_sum - new_sum;
}

__kernel void evaluate_rect(int x, int y, int w, int h, __global int* current_matrix, __global int* target_matrix, __global int* result, __global int* c,
 int width, int height) {
    int color[3];
    
    float res = eval_rect(x, y, w, h, color, current_matrix, target_matrix, width);

    for (int i = 0; i < 3; i++) {
        c[i] = color[i];
    }
    atomic_xchg(result, res);
}

__kernel void calculate_next_rect(__global int* current_matrix, __global int* target_matrix, __global int* result, __global int* rands,
 int width, int height, int random_seed, int num) {
    
    int x = get_global_id(0);
    int y = get_global_id(1);

    int seed = rands[(random_seed + y * width + x) % 64] + y * width + x;

    int rect[8];
    if (num <= 0) {
        rect[0] = rand(&seed) % 256; //x
        rect[1] = rand(&seed) % 256; //y
        rect[2] = 256 - rect[0] + 1; //w constraint
        rect[3] = 256 - rect[1] + 1; //h constraint
        rect[2] = rand(&seed) % rect[2]; //w
        rect[3] = rand(&seed) % rect[3]; //h
    }
    else {
        rect[0] = rand(&seed) % 256; //x
        rect[1] = rand(&seed) % 256; //y
        rect[2] = 1 + rand(&seed) % 8;
        rect[3] = 1 + rand(&seed) % 8;
        rect[2] = rect[2] + rect[0] <= 256 ? rect[2] : 256 - rect[0];
        rect[3] = rect[3] + rect[1] <= 256 ? rect[3] : 256 - rect[1];
    }

    float avg[4];

    float score = eval_rect(rect[0], rect[1], rect[2], rect[3], &(rect[4]), current_matrix, target_matrix, width);
    int iscore = (int)(score * 1.0f);
    rect[7] = iscore;

    for (int i = 0; i < 8; i++) {
        int offset = i * 8;
        int old = atomic_max(&result[offset + 7], iscore);
        if (old < iscore) {
            iscore = 0;
            for (int j = 0; j < 7; j++) {
                atomic_xchg(&result[offset + j], rect[j]);
                
            }
        }
    }

    int mutation_steps = 25;
    for (int i = 0; i < mutation_steps; i++) {
        int mut_rect_id = get_global_id(0) % 8;
        int offset = mut_rect_id * 8;

        //mutation
        for (int j = 0; j < 2; j++) {
            rect[j] = result[offset + j] + rand(&seed) % 8 - 4;
            rect[j] = rect[j] >= 0 ? rect[j] : 0;
            rect[j] = rect[j] < 256 ? rect[j] : 255;
        }

        for (int j = 2; j < 4; j++) {
            rect[j] = result[offset + j] + rand(&seed) % 8 - 4;
            rect[j] = rect[j] > 0 ? rect[j] : 1;
            rect[j] = rect[j] + rect[j - 2] <= 256 ? rect[j] : 256 - rect[j - 2];
        }

        score = eval_rect(rect[0], rect[1], rect[2], rect[3], &(rect[4]), current_matrix, target_matrix, width);
        iscore = (int)(score * 1.0f);
        rect[7] = iscore;

        for (int i = 0; i < 8; i++) {
            int offset = i * 8;
            barrier(CLK_GLOBAL_MEM_FENCE);
            int old = atomic_max(&result[offset + 7], iscore);
            if (old < iscore) {
                iscore = 0;
                for (int j = 0; j < 7; j++) {
                    atomic_xchg(&result[offset + j], rect[j]);
                    
                }
            }
        }

    }
}

__kernel void calculate_next_256_rects(__global int* current_matrix, __global int* target_matrix, __global int* result, __global int* rect_buffer,
                                       int width, int height, int random_seed) {

    int x = get_global_id(0);
    int y = get_global_id(1);
    int gid = get_global_id(0) + get_global_id(1) * get_global_size(0);

    int num = 1;
    int seed = random_seed + y * width + x;


    for (int l = 0; l < 256; l++) {

        int rect[8];
        if (num <= 0) {
            rect[0] = rand(&seed) % width; // x
            rect[1] = rand(&seed) % height; // y
            rect[2] = width - rect[0] + 1; // w constraint
            rect[3] = height - rect[1] + 1; // h constraint
            rect[2] = rand(&seed) % rect[2]; // w
            rect[3] = rand(&seed) % rect[3]; // h
        }
        else {
            rect[0] = rand(&seed) % width; // x
            rect[1] = rand(&seed) % height; // y
            rect[2] = 1 + rand(&seed) % 8;
            rect[3] = 1 + rand(&seed) % 8;
            rect[2] = rect[2] + rect[0] <= width ? rect[2] : width - rect[0];
            rect[3] = rect[3] + rect[1] <= height ? rect[3] : height - rect[1];
        }

        float avg[4];
        float score = eval_rect(rect[0], rect[1], rect[2], rect[3], &(rect[4]), current_matrix, target_matrix, width);
        int iscore = (int)score;
        rect[7] = iscore;

        for (int i = 0; i < 8; i++) {
            int offset = i * 8;
            int old = atomic_max(&result[offset + 7], iscore);
            if (old < iscore) {
                iscore = 0;
                for (int j = 0; j < 7; j++) {
                    atomic_xchg(&result[offset + j], rect[j]);
                    
                }
            }
        }

        barrier(CLK_LOCAL_MEM_FENCE);
        
        int mutation_steps = 5;
        for (int i = 0; i < mutation_steps; i++) {
            int mut_rect_id = get_global_id(0) % 8;
            int offset = mut_rect_id * 8;

            // mutation
            rect[0] = result[offset + 0] + rand(&seed) % 8 - 4;
            rect[0] = rect[0] >= 0 ? rect[0] : 0;
            rect[0] = rect[0] < width ? rect[0] : width - 1;

            rect[1] = result[offset + 1] + rand(&seed) % 8 - 4;
            rect[1] = rect[1] >= 0 ? rect[1] : 0;
            rect[1] = rect[1] < height ? rect[1] : height - 1;

            rect[2] = result[offset + 2] + rand(&seed) % 8 - 4;
            rect[2] = rect[2] > 0 ? rect[2] : 1;
            rect[2] = rect[2] + rect[0] <= width ? rect[2] : width - rect[0];

            rect[3] = result[offset + 3] + rand(&seed) % 8 - 4;
            rect[3] = rect[3] > 0 ? rect[3] : 1;
            rect[3] = rect[3] + rect[1] <= height ? rect[3] : height - rect[1];

            score = eval_rect(rect[0], rect[1], rect[2], rect[3], &(rect[4]), current_matrix, target_matrix, width);
            iscore = (int)(score);
            rect[7] = iscore;

            
            for (int i = 0; i < 8; i++) {
                int offset = i * 8;
                int old = atomic_max(&result[offset + 7], iscore);
                if (old < iscore) {
                    iscore = 0;
                    for (int j = 0; j < 7; j++) {
                        atomic_xchg(&result[offset + j], rect[j]);
                        
                    }
                }
            }

        }

        barrier(CLK_GLOBAL_MEM_FENCE);

        if (x == 0 && y == 0) {
            int xr = result[0];
            int yr = result[1];
            int wr = result[2];
            int hr = result[3];

            int color = 0xFF000000;
            color |= (result[4] & 0xFF) << 16;
            color |= (result[5] & 0xFF) << 8;
            color |= result[6] & 0xFF;

            for (int i = yr; i < yr + hr; i++) {
                for (int j = xr; j < xr + wr; j++) {
                    int index = i * width + j;
                    atomic_xchg(&current_matrix[index], color);
                }
            }

            for (int i = 0; i < 8; i++) {
                atomic_xchg(&rect_buffer[l * 8 + i], result[i]);
            }

            for (int i = 0; i < 64; i++) {
                result[i] = 0;
            }
        }

        barrier(CLK_GLOBAL_MEM_FENCE);
    }
}
