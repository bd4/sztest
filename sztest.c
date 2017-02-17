#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "sz.h"


double check_err(double *a, double *b, size_t n, double abs_bound);
void *malloc_or_die(size_t n);
unsigned char *test_roundtrip(double *data, unsigned int n,
                              double abs_bound,
                              int r5, int r4, int r3, int r2, int r1);


int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: sztest ROWS COLS ERR_EXP\n");
        exit(EXIT_FAILURE);
    }

    int rows = atoi(argv[1]);
    int cols = atoi(argv[2]);
    int err_exp = atoi(argv[3]);
    if (err_exp > 0)
        err_exp = -1 * err_exp;
    double abs_bound = pow(10, err_exp);
    size_t n = rows*cols;

    size_t data_size = n * sizeof(double);
    printf("size %dx%d, %zd bytes\n", rows, cols, data_size);

    double *rand_data = malloc_or_die(data_size);
    double *lin_data = malloc_or_die(data_size);
    double *const_data = malloc_or_die(data_size);

    double set_data[2][3] = {
        { 1.0, 383.2384, -77777777777 },
        { 68123.00, 0.007, 99.99999 },
    };
    double set_data2[2][3];

    srand48(2895720909174927L);

    //printf("sizeof = %zd\n", sizeof(const_data[0][0]));

    double v = 0.0;
    int ij = 0;
    for (int i=0; i<rows; i++) {
        for (int j=0; j<cols; j++) {
            ij = i * cols + j;
            rand_data[ij] = drand48() * 1024;
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

    printf("\n== RAND ==\n");
    unsigned char *rand_out =
        test_roundtrip(rand_data, n, abs_bound, 0, 0, 0, cols, rows);

    printf("\n== LIN ==\n");
    unsigned char *lin_out =
        test_roundtrip(lin_data, n, abs_bound, 0, 0, 0, cols, rows);

    printf("\n== CONST ==\n");
    unsigned char *const_out =
        test_roundtrip(const_data, n, abs_bound, 0, 0, 0, cols, rows);

    printf("\n== SET ==\n");
    unsigned char *set_out =
        test_roundtrip(&set_data[0][0], 2*3, abs_bound, 0, 0, 0, 3, 2);


    free(rand_data);
    free(lin_data);
    free(const_data);

    free(set_out);
    free(rand_out);
    free(lin_out);
    free(const_out);

    SZ_Finalize();
}


// returns negative if any individual error is greater than abs_bound, otherwise
// returns the average error.
double check_err(double *a, double *b, size_t n, double abs_bound) {
    double diff;
    double total_error = 0;
    for (int i=0; i<n; i++) {
        diff = fabs(a[i] - b[i]);
        if (diff > abs_bound) {
            return -1 * diff;
        }
        total_error += diff;
    }
    return total_error / n;
}


unsigned char *test_roundtrip(double *data, unsigned int n,
                              double abs_bound,
                              int r5, int r4, int r3, int r2, int r1) {
    int out_size;
    double *data2 = malloc_or_die(n * sizeof(double));
    unsigned char *out = SZ_compress_args(SZ_DOUBLE, data, &out_size,
                                          ABS, abs_bound, 0,
                                          r5, r4, r3, r2, r1);

    printf("out_size = %d (%0.4f)\n", out_size,
           (double)out_size / (n * sizeof(double)));
    printf("out = %p\n", out);
    printf("abs bound = %0.16f\n", abs_bound);

    SZ_decompress_args(SZ_DOUBLE, out, out_size, data2,
                       r5, r4, r3, r2, r1);
    if (memcmp(data, data2, n * sizeof(double)) != 0) {
        printf("roundtrip differs\n");
    }

    double avg_error = check_err(data, data2, n, abs_bound);
    if (avg_error < 0) {
        fprintf(stderr, "ERR: data error out of range: %0.16f\n", avg_error);
    } else {
        printf("avg error = %0.16f\n", avg_error);
    }

    free(data2);

    return out;
}


void *malloc_or_die(size_t data_size) {
    void *p = malloc(data_size);
    if (p == NULL) {
        fprintf(stderr, "malloc size %zd failed\n", data_size);
        exit(EXIT_FAILURE);
    }
    return p;
}
