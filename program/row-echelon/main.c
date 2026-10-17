#include "cmatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile sig_atomic_t status = 0;

void signalExit(int signo);

int main()
{
    matrix_t A = {0};
    matrix_t B = {0};
    int m, n;

    printf("Type your desired dimensions.\n");
    do
    {
        printf("Number of rows: ");
    } while ((scanf("%d", &m) <= 0) || (m <= 0));
    do
    {
        printf("Number of columns: ");
    } while ((scanf("%d", &n) <= 0) || (n <= 0));

    matrix_construct(&A, m, n);
    matrix_construct(&B, m, n);

    signal(SIGINT, signalExit);

    while (status == 0)
    {
        int resp = 0;
        int ready = 0;

        printf("Choose an option:\n");
        printf("\t[1] Size/resize chosen matrix\n");
        printf("\t[2] Generate a random matrix and show row-echelon form\n");
        printf("\t[3] Construct a user-defined matrix and show row-echelon form\n");
        printf("\t[0] Exit\n");
        do
        {
            printf("Your option: ");
        } while (scanf("%d", &resp) <= 0);

        if (resp == 1)
        {
            printf("\nType your desired dimensions.\n");
            do
            {
                printf("Number of rows: ");
            } while (scanf("%d", &m) <= 0);
            do
            {
                printf("Number of columns: ");
            } while (scanf("%d", &n) <= 0);

            matrix_reconstruct(&A, m, n);
            matrix_reconstruct(&B, m, n);
        }
        else if (resp == 2)
        {
            matrix_feed_random(&A);
            printf("\nA:\n");
            matrix_peek_stdout(A);
            ready = 1;
        }
        else if (resp == 3)
        {
            printf("\nType your desired values\n");
            matrix_feed_stdin(&A);
            printf("\nA:\n");
            matrix_peek_stdout(A);
            ready = 1;
        }
        else if (resp == 0)
        {
            break;
        }
        else
        {
            printf("\nBad option");
            continue;
        }

        if (ready)
        {
            matrix_row_echelon(&B, A);

            printf("\nRow-echelon equivalent:\n");
            matrix_peek_stdout(B);
        }

        printf("\n");
    }

    matrix_destroy(&B);
    matrix_destroy(&A);

    return 0;
}

void signalExit(int signo)
{
    status = signo;
    fprintf(stderr, "\a\nCaught error %d\n", signo);
}
