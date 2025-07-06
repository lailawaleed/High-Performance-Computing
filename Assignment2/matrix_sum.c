#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int rank, size;
    int N = 4, M = 4; // Default matrix dimensions
    int *A = NULL, *B = NULL, *C = NULL; // Matrices as 1D arrays
    int *local_A, *local_B, *local_C; // Local chunks for each process
    int base_rows, local_size, remaining_rows;
    MPI_Status status;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Master process: Get matrix dimensions
    if (rank == 0) {
        printf("Enter the size of matrix NxM : ");
        fflush(stdout);
        scanf("%d", &N);
        scanf("%d", &M);
    }

    // Broadcast N and M to all processes
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate base rows per process and remaining rows
    base_rows = N / size; // Integer division for base rows
    remaining_rows = N % size; // Remaining rows for master
    local_size = base_rows * M; // Size of chunk for each process

    // Allocate local arrays for all processes
    local_A = (int *)malloc(local_size * sizeof(int));
    local_B = (int *)malloc(local_size * sizeof(int));
    local_C = (int *)malloc(local_size * sizeof(int));

    // Master process: Allocate and input matrices A and B
    if (rank == 0) {
        A = (int *)malloc(N * M * sizeof(int));
        B = (int *)malloc(N * M * sizeof(int));
        C = (int *)malloc(N * M * sizeof(int));

        // Input matrix A
        printf("\nEnter values in Matrix a :\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                scanf("%d", &A[i * M + j]);
            }
        }

        // Input matrix B
        printf("\nEnter values in matrix b :\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                scanf("%d", &B[i * M + j]);
            }
        }
    }

    // Scatter matrices A and B using MPI_Scatter
    MPI_Scatter(A, local_size, MPI_INT, local_A, local_size, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(B, local_size, MPI_INT, local_B, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Each process computes its portion of C = A + B
    for (int i = 0; i < local_size; i++) {
        local_C[i] = local_A[i] + local_B[i];
    }

    // Gather results from all processes to C in master using MPI_Gather
    MPI_Gather(local_C, local_size, MPI_INT, C, local_size, MPI_INT, 0, MPI_COMM_WORLD);

    // Master process: Handle remaining rows and print result
    if (rank == 0) {
        // Compute remaining rows (if any)
        for (int i = base_rows * size; i < N; i++) {
            for (int j = 0; j < M; j++) {
                C[i * M + j] = A[i * M + j] + B[i * M + j];
            }
        }

        // Print result matrix C
        printf("\nThe resulted matrix c is :\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                printf("%4d ", C[i * M + j]);
            }
            printf("\n");
        }

        // Free master matrices
        free(A);
        free(B);
        free(C);
    }

    // Free allocated memory
    free(local_A);
    free(local_B);
    free(local_C);

    // Finalize MPI
    MPI_Finalize();
    return 0;
}
