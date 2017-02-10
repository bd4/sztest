#include <stdlib.h>
#include <stdio.h>

#include "sz.h"

#define ROWS 8
#define COLS 8

#define ABS_BOUND  1e6


int check_err(double *a, double *b, size_t n, double abs_bound);
void *malloc_or_die(size_t n);
int test_roundtrip(double *data, unsigned int data_size, double abs_bound,
                   int r5, int r4, int r3, int r2, int r1);

int main(int argc, char **argv) {
    size_t n = ROWS*COLS;

    double *rand_data = malloc_or_die(n * sizeof(double));
    double *lin_data = malloc_or_die(n * sizeof(double));
    double *const_data = malloc_or_die(n * sizeof(double));

    double set_data[2][3] = {
        { 1.0, 383.2384, -77777777777 },
        { 68123.00, 0.007, 99.99999 },
    };
    double set_data2[2][3];

    srand48(2895720909174927L);

    //printf("sizeof = %zd\n", sizeof(const_data[0][0]));

    double v = 0.0;
    int ij = 0;
    for (int i=0; i<ROWS; i++) {
        for (int j=0; j<COLS; j++) {
            ij = i * COLS + j;
            rand_data[ij] = drand48() * 1000;
            lin_data[ij] = v;
            const_data[ij] = 1.0;
            v += 0.1;
            //printf("%d %d %p\n", i, j, &rand_data[i][j]);
        }
    }

    int rval = SZ_Init("sz.config");
    if (rval != 0) {
        fprintf(stderr, "Failed to load sz.config, exiting\n");
        exit(EXIT_FAILURE);
    }

    printf("== SET ==\n");
    test_roundtrip(&set_data[0][0], 2*3, ABS_BOUND, 0, 0, 0, 3, 2);
    printf("\n== RAND ==\n");
    test_roundtrip(rand_data, n, ABS_BOUND, 0, 0, 0, COLS, ROWS);
    printf("\n== LIN ==\n");
    test_roundtrip(lin_data, n, ABS_BOUND, 0, 0, 0, COLS, ROWS);
    printf("\n== CONST ==\n");
    test_roundtrip(const_data, n, ABS_BOUND, 0, 0, 0, COLS, ROWS);

    free(rand_data);
    free(lin_data);
    free(const_data);

    SZ_Finalize();
}


int check_err(double *a, double *b, size_t n, double abs_bound) {
    double diff;
    for (int i=0; i<n; i++) {
        diff = a[i] - b[i];
        if (diff > abs_bound || diff < -abs_bound) {
            return 0;
        }
    }
    return 1;
}


int test_roundtrip(double *data, unsigned int data_size, double abs_bound,
                   int r5, int r4, int r3, int r2, int r1) {
    int out_size;
    double *data2 = malloc_or_die(data_size * sizeof(double));
    unsigned char *out = SZ_compress_args(SZ_DOUBLE, data, &out_size,
                                          ABS, abs_bound, 0,
                                          r5, r4, r3, r2, r1);

    printf("out_size = %d\n", out_size);
    printf("out = %p\n", out);

    SZ_decompress_args(SZ_DOUBLE, out, out_size, data2,
                       r5, r4, r3, r2, r1);
    if (memcmp(data, data2, data_size) != 0) {
        printf("roundtrip differs\n");
    }

    if (!check_err(data, data2, data_size, abs_bound)) {
        fprintf(stderr, "ERR: data error out of range\n");
    }

    free(out);
    free(data2);
}


void *malloc_or_die(size_t n) {
    void *p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "malloc %zd failed\n", n);
        exit(EXIT_FAILURE);
    }
    return p;
}
