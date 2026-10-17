#include "cmatrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static volatile sig_atomic_t status = 0;

void sig_handler(int signo);

int main()
{
    matrix_t A, B, C;
    int option;

    signal(status, sig_handler);

    matrix_construct(&A, 2, 2);
    matrix_construct(&B, 2, 2);
    matrix_construct(&C, 2, 2);

    while (status == 0)
    {
        printf("Choose an option:\n");
        printf("\t[1] Pick two matrices randomly.\n");
        printf("\t[2] Feed matrices manually.\n");
        printf("\t[0] Exit\n");

        do
        {
            printf("Your option: ");
        } while (scanf("%d", &option) < 0);

        if (option == 0)
        {
            raise(SIGINT);
            break;
        }
        else if (option == 1)
        {
            printf("Feeding matrices...\n");
            matrix_feed_random(&A);
            matrix_feed_random(&B);
        }
        else if (option == 2)
        {
            printf("A:\n");
            matrix_feed_stdin(&A);
            printf("\nB:\n");
            matrix_feed_stdin(&B);
        }
        else
        {
            printf("Bad input\n");
            continue;
        }

        printf("\nA:\n");
        matrix_peek_stdout(A);

        printf("\nB:\n");
        matrix_peek_stdout(B);

        printf("\nA * B:\n");
        matrix_mult(&C, A, B);
        matrix_peek_stdout(C);

        printf("\n");
    }

    matrix_destroy(&C);
    matrix_destroy(&B);
    matrix_destroy(&A);

    return 0;
}

void sig_handler(int signo)
{
    status = signo;
    fprintf(stdout, "\aCaught signal %d\n", signo);
}
