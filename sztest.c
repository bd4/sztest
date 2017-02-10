#include <stdlib.h>
#include <stdio.h>

#include "sz.h"

#define ROWS 100
#define COLS 1000

#define ABS_BOUND  1e8


int check_err(double *a, double *b, size_t n, double abs_bound);


int main(int argc, char **argv) {
    double rand_data[ROWS][COLS];
    double lin_data[ROWS][COLS];
    double const_data[ROWS][COLS];

    double rand_data2[ROWS][COLS];
    double lin_data2[ROWS][COLS];
    double const_data2[ROWS][COLS];

    unsigned char *rand_out = NULL;
    unsigned char *lin_out = NULL;
    unsigned char *const_out = NULL;

    size_t n = ROWS*COLS;

    srand48(2895720909174927L);

    //printf("sizeof = %zd\n", sizeof(const_data[0][0]));

    double v = 0.0;
    for (int i=0; i<ROWS; i++) {
        for (int j=0; j<COLS; j++) {
            rand_data[i][j] = drand48() * 1000;
            lin_data[i][j] = v;
            const_data[i][j] = 1.0;
            v += 0.1;
            //printf("%d %d %p\n", i, j, &rand_data[i][j]);
        }
    }

    int rval = SZ_Init("sz.config");
    if (rval != 0) {
        fprintf(stderr, "Failed to load sz.config, exiting\n");
        exit(EXIT_FAILURE);
    }

    int rand_out_size;
    rand_out = SZ_compress_args(SZ_DOUBLE, &rand_data[0][0], &rand_out_size,
                                ABS, ABS_BOUND, 0,
                                0, 0, 0, COLS, ROWS);
    int lin_out_size;
    lin_out = SZ_compress_args(SZ_DOUBLE, &lin_data[0][0], &lin_out_size,
                               ABS, ABS_BOUND, 0,
                               0, 0, 0, COLS, ROWS);
    int const_out_size;
    const_out = SZ_compress_args(SZ_DOUBLE, &const_data[0][0], &const_out_size,
                                 ABS, ABS_BOUND, 0,
                                 0, 0, 0, COLS, ROWS);

    printf("rand_out_size = %d\n", rand_out_size);
    printf("lin_out_size = %d\n", lin_out_size);
    printf("const_out_size = %d\n", const_out_size);

    printf("rand_out = %p\n", rand_out);
    printf("lin_out = %p\n", lin_out);
    printf("const_out = %p\n", const_out);

    SZ_decompress_args(SZ_DOUBLE, rand_out, rand_out_size, &rand_data2[0][0],
                       0, 0, 0, COLS, ROWS);
    SZ_decompress_args(SZ_DOUBLE, lin_out, lin_out_size, &lin_data2[0][0],
                       0, 0, 0, COLS, ROWS);
    SZ_decompress_args(SZ_DOUBLE, const_out, const_out_size, &const_data2[0][0],
                       0, 0, 0, COLS, ROWS);

    if (memcmp(&rand_data[0][0], &rand_data2[0][0], n) != 0) {
        printf("rand_data roundtrip differs\n");
    }
    if (memcmp(&lin_data[0][0], &lin_data2[0][0], n) != 0) {
        printf("lin_data roundtrip differs\n");
    }
    if (memcmp(&const_data[0][0], &const_data2[0][0], n) != 0) {
        printf("const_data roundtrip differs\n");
    }

    if (!check_err(&rand_data[0][0], &rand_data2[0][0], n, ABS_BOUND)) {
        fprintf(stderr, "ERR: rand_data error out of range\n");
    }
    if (!check_err(&lin_data[0][0], &lin_data2[0][0], n, ABS_BOUND)) {
        fprintf(stderr, "ERR: lin_data error out of range\n");
    }
    if (!check_err(&const_data[0][0], &const_data2[0][0], n, ABS_BOUND)) {
        fprintf(stderr, "ERR: const_data error out of range\n");
    }

    if (rand_out != NULL)
        free(rand_out);
    if (lin_out != NULL)
        free(lin_out);
    if (const_out != NULL)
        free(const_out);

    SZ_Finalize();
}


int check_err(double *a, double *b, size_t n, double abs_bound) {
    double diff;
    for (int i=0; i<n; i++) {
        diff = a[i] - b[i];
        if (diff > abs_bound) {
            return 0;
        }
    }
    return 1;
}
