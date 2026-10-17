#include "cmatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#define ESP 1e-12

extern int errorlevel;
extern char errormsg[100];

/**
 * Errorlevel list:
 *   2 = (warning) reconstructed unmathced matrix
 *   1 = (warning) data loss occured
 *   0 = clear
 *  -1 = (error) fatal error
 *  -2 = (error) invalid argument
 *  -3 = (error) operation not permitted
 */

int matrix_construct(matrix_t *M, int m, int n)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(CONSTRUCT) invalid matrix structure");
        return errorlevel;
    }

    if ((m <= 0) || (n <= 0))
    {
        errorlevel = -2;
        strcpy(errormsg, "(CONSTRUCT) non-positive arguments are not allowed");
        return errorlevel;
    }

    M->Grid = (double **)malloc(m * sizeof(double *));
    if (M->Grid == NULL)
    {
        errorlevel = -1;
        strcpy(errormsg, "(CONSTRUCT) memory allocation failed");
        return errorlevel;
    }

    for (int i = 0; i < m; i++)
    {
        M->Grid[i] = (double *)calloc(n, sizeof(double));
        if (M->Grid[i] == NULL)
        {
            matrix_destroy(M);
            errorlevel = -1;
            strcpy(errormsg, "(CONSTRUCT) memory allocation failed");
            return errorlevel;
        }
    }
    M->Rows = m;
    M->Columns = n;

    errorlevel = 0;
    return errorlevel;
}

int matrix_reconstruct(matrix_t *M, int m, int n)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(RECONSTRUCT) invalid matrix structure");
        return errorlevel;
    }

    if ((n <= 0) || (m <= 0))
    {
        errorlevel = -2;
        strcpy(errormsg, "(RECONSTRUCT) non-positive arguments are not allowed");
        return errorlevel;
    }

    int old_rows = (M->Grid == NULL) ? 0 : M->Rows;
    int old_cols = (M->Grid == NULL) ? 0 : M->Columns;

    double **new_grid = (double **)realloc(M->Grid, (size_t)m * sizeof(*new_grid));
    if (new_grid == NULL)
    {
        errorlevel = -1;
        strcpy(errormsg, "(RECONSTRUCT) memory reallocation failed");
        return errorlevel;
    }
    M->Grid = new_grid;

    for (int i = 0; i < m; i++)
    {
        if (i < old_rows)
        {
            double *row = (double *)realloc(M->Grid[i], (size_t)n * sizeof(**M->Grid));
            if (row == NULL)
            {
                errorlevel = -1;
                strcpy(errormsg, "(RECONSTRUCT) memory reallocation failed");
                return errorlevel;
            }
            M->Grid[i] = row;

            if (n > old_cols)
            {
                memset(M->Grid[i] + old_cols, 0, (size_t)(n - old_cols) * sizeof(**M->Grid));
            }
        }
        else
        {
            M->Grid[i] = (double *)calloc((size_t)n, sizeof(**M->Grid));
            if (M->Grid[i] == NULL)
            {
                errorlevel = -1;
                strcpy(errormsg, "(RECONSTRUCT) memory reallocation failed");
                return errorlevel;
            }
        }
    }

    for (int i = m; i < old_rows; i++)
    {
        free(M->Grid[i]);
    }

    M->Rows = m;
    M->Columns = n;

    errorlevel = 0;
    return errorlevel;
}

int matrix_cpy(matrix_t *A, matrix_t B)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (A == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(CPY) invalid matrix structures");
        return errorlevel;
    }

    if (B.Grid == NULL)
    {
        errorlevel = -3;
        strcpy(errormsg, "(CPY) uninitialized matrix operand");
        return errorlevel;
    }

    if ((A->Rows != B.Rows) || (A->Columns != B.Columns))
    {
        errorlevel = 2;
        strcpy(errormsg, "(CPY) reconstructed unmathced matrix operand");
        if (matrix_reconstruct(A, B.Rows, B.Columns) < 0)
        {
            strcpy(errormsg, "(CPY) reconstruction failed");
            return errorlevel;
        }
    }

    int m = A->Rows;    // or B->Rows
    int n = A->Columns; // or B->Columns
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            A->Grid[i][j] = B.Grid[i][j];

    return errorlevel;
}

int matrix_destroy(matrix_t *M)
{
    if (M == NULL)
        return 0;

    if (M->Grid != NULL)
    {
        for (int i = 0; i < M->Rows; i++)
            free(M->Grid[i]);
        free(M->Grid);
    }

    M->Rows = 0;
    M->Columns = 0;

    errorlevel = 0;
    return 0;
}

int matrix_feed_random(matrix_t *M)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(FEED) invalid matrix structure");
        return errorlevel;
    }

    if (M->Grid == NULL)
    {
        errorlevel = -3;
        strcpy(errormsg, "(FEED) uninitialized matrix");
        return errorlevel;
    }

    for (int i = 0; i < M->Rows; i++)
    {
        for (int j = 0; j < M->Columns; j++)
            M->Grid[i][j] = ((double)rand() / (double)RAND_MAX);
    }

    errorlevel = 0;
    return errorlevel;
}

int matrix_feed_stdin(matrix_t *M)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(FEED) invalid matrix structure");
        return errorlevel;
    }

    if (M->Grid == NULL)
    {
        errorlevel = -3;
        strcpy(errormsg, "(FEED) uninitialized matrix");
        return errorlevel;
    }

    for (int i = 0; i < M->Rows; i++)
    {
        for (int j = 0; j < M->Columns; j++)
        {
            printf("M[%d][%d] = ", i + 1, j + 1);
            scanf("%lf", &(M->Grid[i][j]));
        }
    }

    errorlevel = 0;
    return errorlevel;
}

int matrix_feed_manual(matrix_t *M, int count, ...)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(FEED) invalid matrix structure");
        return errorlevel;
    }

    if (M->Grid == NULL)
    {
        errorlevel = -3;
        strcpy(errormsg, "(FEED) uninitialized matrix");
        return errorlevel;
    }

    va_list ap;
    int i = 0, j = 0;

    va_start(ap, count);
    for (int k = 0; k < count; k++)
    {
        M->Grid[i][j] = (double)va_arg(ap, double);
        j++;
        if (j == M->Columns)
        {
            j = 0;
            i++;
        }
    }
    va_end(ap);

    return errorlevel;
}

int matrix_peek_stdout(matrix_t M)
{
    int p = 0;

    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M.Grid == NULL)
    {
        errorlevel = -3;
        strcpy(errormsg, "(FEED) uninitialized matrix");
        return errorlevel;
    }

    for (int i = 0; i < M.Rows; i++)
    {
        for (int j = 0; j < M.Columns; j++)
        {
            p = fprintf(stdout, "%lf ", M.Grid[i][j]);
            if (p <= 0)
            {
                errorlevel = -1;
                strcpy(errormsg, "(PEEK) output failed");
                return errorlevel;
            }
        }
        p = fprintf(stdout, "\n");
        if (p <= 0)
        {
            errorlevel = -1;
            strcpy(errormsg, "(PEEK) output failed");
            return errorlevel;
        }
    }

    errorlevel = 0;
    return 0;
}

int matrix_add(matrix_t *C, matrix_t A, matrix_t B)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (C == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(ADD) invalid matrix struccture");
        return errorlevel;
    }

    if ((A.Rows != B.Rows) || (A.Columns != B.Columns))
    {
        errorlevel = -3;
        strcpy(errormsg, "(ADD) unmatched matrix operands");
        return errorlevel;
    }

    int m = A.Rows;    // or B.Rows
    int n = A.Columns; // or B.Columns

    if (C->Grid == NULL)
    {
        errorlevel = 2;
        strcpy(errormsg, "(ADD) reconstructed unmatched destination operand");

        if (matrix_reconstruct(C, m, n) < 0)
            return errorlevel;
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
            C->Grid[i][j] = A.Grid[i][j] + B.Grid[i][j];
    }

    return errorlevel;
}

int matrix_sub(matrix_t *C, matrix_t A, matrix_t B)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (C == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(SUB) invalid matrix struccture");
        return errorlevel;
    }

    if ((A.Rows != B.Rows) || (A.Columns != B.Columns))
    {
        errorlevel = -3;
        strcpy(errormsg, "(SUB) unmatched matrix operands");
        return errorlevel;
    }

    int m = A.Rows;    // or B.Rows
    int n = A.Columns; // or B.Columns

    if (C->Grid == NULL)
    {
        errorlevel = 2;
        strcpy(errormsg, "(ADD) reconstructed unmatched destination operand");

        if (matrix_reconstruct(C, m, n) < 0)
            return errorlevel;
    }

    matrix_scale(&B, -1.0);
    matrix_add(C, A, B);

    errorlevel = 0;
    return errorlevel;
}

int matrix_mult(matrix_t *C, matrix_t A, matrix_t B)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (C == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(MULT) invalid matrix struccture");
        return errorlevel;
    }

    if (A.Columns != B.Rows)
    {
        errorlevel = -3;
        strcpy(errormsg, "(MULT) unmatched matrix operands");
        return errorlevel;
    }

    int m = A.Rows;
    int n = B.Columns;
    int p = A.Columns; // or B.Rows

    if (C->Grid == NULL)
    {
        errorlevel = 2;
        strcpy(errormsg, "(ADD) reconstructed unmatched destination operand");

        if (matrix_reconstruct(C, m, n) < 0)
            return errorlevel;
    }

    for (int i = 0; i < m; i++)
    {
        for (int j = 0; j < n; j++)
        {
            C->Grid[i][j] = 0.0;
            for (int k = 0; k < p; k++)
                C->Grid[i][j] += A.Grid[i][k] * B.Grid[k][j];
        }
    }

    errorlevel = 0;
    return errorlevel;
}

int matrix_scale(matrix_t *M, double r)
{
    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(SCALE) invalid matrix structure");
        return errorlevel;
    }

    if (M->Grid == NULL)
    {
        errorlevel = -3;
        strcpy(errormsg, "(SCALE) uninitialized matrix");
        return errorlevel;
    }

    for (int i = 0; i < M->Rows; i++)
        for (int j = 0; j < M->Columns; j++)
            M->Grid[i][j] *= r;

    errorlevel = 0;
    return errorlevel;
}

int matrix_swap_rows(matrix_t *M, int row1, int row2)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(SWAP) invalid matrix structure");
        return errorlevel;
    }

    if (M->Grid == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(SWAP) uninitialized matrix operand");
        return errorlevel;
    }

    if ((row1 < 0) || (row2 < 0))
    {
        errorlevel = -3;
        strcpy(errormsg, "(SWAP) invalid row indecies");
        return errorlevel;
    }
    else if ((row1 >= M->Rows) || (row2 >= M->Rows))
    {
        errorlevel = -3;
        strcpy(errormsg, "(SWAP) non-existing rows");
        return errorlevel;
    }
    else if (row1 == row2)
    {
        errorlevel = 0;
        strcpy(errormsg, "Clear");
        return 0;
    }

    for (int i = 0; i < M->Columns; i++)
    {
        double tmp = M->Grid[row1][i];
        M->Grid[row1][i] = M->Grid[row2][i];
        M->Grid[row2][i] = tmp;
    }

    errorlevel = 0;
    return 0;
}

int matrix_scale_row(matrix_t *M, int row, double c)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(RSCALE) invalid matrix structure");
        return errorlevel;
    }

    if (M->Grid == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(RSCALE) uninitialized matrix operand");
        return errorlevel;
    }

    if ((row < 0) || (row >= M->Rows))
    {
        errorlevel = -3;
        strcpy(errormsg, "(RSCALE) invalid or non-existing row indecies");
        return errorlevel;
    }

    if (c == 0.0)
    {
        errorlevel = 1;
        strcpy(errormsg, "(RSCALE) zero scaling caused data loss in selected row");
    }

    for (int i = 0; i < M->Columns; i++)
        M->Grid[row][i] *= c;

    return errorlevel;
}

int matrix_sum_rows(matrix_t *M, int row3, int row1, int row2, double c1, double c2)
{
    if (errorlevel < 0)
    {
        matrix_perror(NULL);
        return errorlevel;
    }

    if (M == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(RSUM) invalid matrix structure");
        return errorlevel;
    }

    if (M->Grid == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(RSUM) uninitialized matrix operand");
        return errorlevel;
    }

    if ((row1 < 0) || (row2 < 0) || (row3 < 0))
    {
        errorlevel = -3;
        strcpy(errormsg, "(RSUM) invalid row indecies");
        return errorlevel;
    }
    else if ((row1 >= M->Rows) || (row2 >= M->Rows) || (row3 >= M->Rows))
    {
        errorlevel = -3;
        strcpy(errormsg, "(RSUM) non-existing rows");
        return errorlevel;
    }

    if ((c1 == 0.0) && (c2 == 0.0))
    {
        errorlevel = 1;
        strcpy(errormsg, "(RSUM) substitution of zero rows, caused data loss in the destination row");
    }

    for (int i = 0; i < M->Columns; i++)
    {
        double tmp = M->Grid[row3][i];
        tmp = (c1 * M->Grid[row1][i]) + (c2 * M->Grid[row2][i]);
        M->Grid[row3][i] = tmp;
    }

    return errorlevel;
}

int matrix_row_echelon(matrix_t *D, matrix_t M)
{
    if (M.Grid == NULL)
    {
        errorlevel = -3;
        strcpy(errormsg, "(RECH) uninitialized matrix operand");
        return errorlevel;
    }

    if (D == NULL)
    {
        errorlevel = -2;
        strcpy(errormsg, "(RECH) invalid matrix structure");
        return errorlevel;
    }

    if ((D->Grid == NULL) || (D->Rows != M.Rows) || (D->Columns != M.Columns))
    {
        errorlevel = 2;
        strcpy(errormsg, "(RECH) reconstructed unmathced matrix operand");
        if (matrix_reconstruct(D, M.Rows, M.Columns) < 0)
        {
            strcpy(errormsg, "(RECH) reconstruction failed");
            return errorlevel;
        }
    }
    matrix_cpy(D, M);

    int lead = 0;
    for (int col = 0; col < M.Columns && lead < M.Rows; col++)
    {
        int p = lead;
        for (int i = lead + 1; i < M.Rows; i++)
            if (fabs(D->Grid[i][col]) > fabs(D->Grid[p][col]))
                p = i;

        if (fabs(D->Grid[p][col]) < ESP)
            continue;

        if (p != lead)
            matrix_swap_rows(D, lead, p);

        for (int i = lead + 1; i < M.Rows; i++)
        {
            double m = D->Grid[i][col] / D->Grid[lead][col];
            if (fabs(m) < ESP)
                continue;
            matrix_sum_rows(D, i, i, lead, 1.0, -m);
            D->Grid[i][col] = 0.0;
        }
        lead++;
    }

    return errorlevel;
}

int matrix_cmp(matrix_t A, matrix_t B)
{
    if ((A.Rows != B.Rows) || (A.Columns != B.Columns))
    {
        return 0;
    }
    int m = A.Rows;    // or B.Rows
    int n = A.Columns; // or B.Columns

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (A.Grid[i][j] != B.Grid[i][j])
                return 0;

    return 1;
}

int matrix_get_errorlevel()
{
    return errorlevel;
}

const char *matrix_get_errormsg()
{
    return errormsg;
}

void matrix_perror(const char *msg)
{
    if (errorlevel == 0)
        strcpy(errormsg, "Clear");

    if (msg == NULL)
        fprintf(stderr, "%s\n", errormsg);
    else
        fprintf(stderr, "%s: %s\n", errormsg, msg);
}
