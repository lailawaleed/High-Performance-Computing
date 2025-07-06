#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>

#define CYAN "\033[1;36m"
#define BROWN "\033[1;33m"
#define RESET "\033[0m"

// Function to compute local sum using OpenMP
long long computeLocalSum(int *chunk, int chunkSize, int rank) {
    long long localSum = 0;
    #pragma omp parallel for reduction(+:localSum)
    for (int i = 0; i < chunkSize; i++) {
        int threadId = omp_get_thread_num();
        printf(" [Process %2d] Thread %2d: Adding element %d(value:%d)\n",rank,threadId,i,chunk[i]);
        localSum += chunk[i];
    }
    return localSum;
}

int main() {
    MPI_Init(NULL, NULL);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n;
    int *array = NULL;
    long long globalSum = 0;

    if (rank == 0) {
        // Root process inputs array size and 
        printf(CYAN "==== Array Sum with MPI and OpenMP ===="RESET "\n");
        printf("Enter the size of the array: ");
        fflush(stdout);
        scanf("%d", &n);

        // Allocate and initialize array 
        array = (int *)malloc(n * sizeof(int));
        printf("\n"CYAN "----- Input Array ----- "RESET "\n");
        printf("Enter elements of the array: ");
        fflush(stdout);
        for (int i = 0; i < n; i++) {
            scanf("%d", &array[i]);
        }
        printf("\n");
    }

    // Broadcast array size to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute chunk size for each process (equal distribution)
    int chunkSize = n / size;
    int remainder = n % size; // Extra elements to be handled by root

    // Allocate memory for local chunk
    int *chunk = (int *)malloc(chunkSize * sizeof(int));

    // Scatter array chunks to all processes
    MPI_Scatter(array, chunkSize, MPI_INT, chunk, chunkSize, MPI_INT, 0, MPI_COMM_WORLD);

    // Compute local sum using OpenMP
    long long localSum = computeLocalSum(chunk, chunkSize,rank);

    // Root process handles remaining elements (if any)
    if (rank == 0 && remainder > 0) {
        // Compute sum of remaining elements (indices: chunk_size * size to n-1)
        int *remainderChunk = array + chunkSize * size;
        printf("\n" CYAN "----- Process 0 Remainder -----" RESET "\n");
        printf("  Size: %d, Elements: ", remainder);
        for (int i = 0; i < remainder; i++) {
            printf("%d ", remainderChunk[i]);
        }
        printf("\n");
        printf(CYAN "----- Computing Remainder Sum -----" RESET "\n");
        long long remainderSum = computeLocalSum(remainderChunk, remainder, rank);
        printf("\n [Process  0] Remainder sum: %lld\n", remainderSum);
        localSum += remainderSum;
    }

    // Reduce local sums to global sum
    MPI_Reduce(&localSum, &globalSum, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    // Root process prints result
    if (rank == 0) {
        printf("\n"BROWN "===== Global Sum: %lld =====" RESET"\n\n", globalSum);
    }

    // Clean up
    if (rank == 0) {
        free(array);
    }
    free(chunk);

    MPI_Finalize();
    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int rank, size;
    int scalar, vector_size;
    int *vector = NULL;
    int *final_result = NULL;
    int *local_data = NULL;
    int local_size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        printf("Enter vector size: ");
        scanf("%d", &vector_size);

        vector = malloc(vector_size * sizeof(int));
        printf("Enter scalar: ");
        scanf("%d", &scalar);

        printf("Enter vector elements:\n");
        for (int i = 0; i < vector_size; i++) {
            scanf("%d", &vector[i]);
        }

        final_result = malloc(vector_size * sizeof(int));
    }

    // Broadcast scalar and vector size to all processes
    MPI_Bcast(&scalar, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&vector_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int base = vector_size / size;
    int rem = vector_size % size;

    // Determine local size for this rank
    local_size = base + (rank < rem ? 1 : 0);
    local_data = malloc(local_size * sizeof(int));

    if (rank == 0) {
        // Manual send to others
        int offset = 0;

        for (int i = 1; i < size; i++) {
            int send_count = base + (i < rem ? 1 : 0);
            offset += base + (i - 1 < rem ? 1 : 0);  // update offset before sending
            MPI_Send(&vector[offset], send_count, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        // Copy local chunk for root (rank 0)
        for (int i = 0; i < local_size; i++) {
            local_data[i] = vector[i];
        }
    } else {
        MPI_Recv(local_data, local_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // Multiply each element by the scalar
    for (int i = 0; i < local_size; i++) {
        local_data[i] *= scalar;
    }

    // Root gathers the results
    if (rank == 0) {
        int offset = 0;
        // Copy root's result
        for (int i = 0; i < local_size; i++) {
            final_result[i] = local_data[i];
        }
        offset = local_size;

        for (int i = 1; i < size; i++) {
            int recv_count = base + (i < rem ? 1 : 0);
            MPI_Recv(&final_result[offset], recv_count, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            offset += recv_count;
        }

        // Print the final result
        printf("\nFinal result: (");
        for (int i = 0; i < vector_size; i++) {
            printf("%d", final_result[i]);
            if (i != vector_size - 1) printf(", ");
        }
        printf(")\n");

        free(vector);
        free(final_result);
    } else {
        MPI_Send(local_data, local_size, MPI_INT, 0, 1, MPI_COMM_WORLD);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
