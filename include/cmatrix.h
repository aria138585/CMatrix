#ifndef _CMATRIX_H_
#define _CMATRIX_H_

typedef struct
{
    int Rows;
    int Columns;
    double **Grid;
} matrix_t;

int errorlevel = 0;
char errormsg[100];

int matrix_construct(matrix_t *M, int m, int n);
int matrix_reconstruct(matrix_t *M, int m, int n);
int matrix_cpy(matrix_t *A, matrix_t B);
int matrix_destroy(matrix_t *M);

int matrix_feed_random(matrix_t *M);
int matrix_feed_stdin(matrix_t *M);
int matrix_feed_manual(matrix_t *M, int count, ...);
int matrix_peek_stdout(matrix_t M);

double matrix_get_element(matrix_t M, int i, int j);
double *matrrix_get_row(matrix_t *M, int i);
double *matrix_get_column(matrix_t *M, int j);

int matrix_add(matrix_t *C, matrix_t A, matrix_t B);
int matrix_sub(matrix_t *C, matrix_t A, matrix_t B);
int matrix_mult(matrix_t *C, matrix_t A, matrix_t B);
int matrix_scale(matrix_t *M, double r);

int matrix_swap_rows(matrix_t *M, int row1, int row2);
int matrix_scale_row(matrix_t *M, int row, double c);
int matrix_sum_rows(matrix_t *M, int row3, int row1, int row2, double c1, double c2);
int matrix_row_echelon(matrix_t *D, matrix_t M);

int matrix_cmp(matrix_t A, matrix_t B);

int matrix_get_errorlevel();
const char *matrix_get_errormsg();
void matrix_perror(const char *msg);

#endif
