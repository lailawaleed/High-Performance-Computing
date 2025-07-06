#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define CYAN "\033[1;36m"
#define BROWN "\033[1;33m"
#define RESET "\033[0m"

void matrixVectorMultiplication(int **A, int *v, int *r, int N) {
    #pragma omp parallel for
    for (int i = 0; i < N; i++) {
        int threadId = omp_get_thread_num();
        printf("  Thread %d computing row %d\n",threadId,i);
        r[i] = 0;
        for (int j = 0; j < N; j++) {
            r[i] += A[i][j] * v[j];
        }
    }
}

int main(){
    int n;
    printf(CYAN "==== Matrix-Vector Multiplication ===="RESET "\n");
    printf("Enter the size of the matrix (n): ");
    fflush(stdout);
    scanf("%d", &n);

    // Allocate memory for matrix A, vector v, and result r
    int **A = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++) {
        A[i] = (int *)malloc(n * sizeof(int));
    }
    int *v=(int *)malloc(n * sizeof(int));
    int *r=(int *)malloc(n * sizeof(int));

    //Input matrix A 
    printf("\n"CYAN "----- Input Matrix A (%d x %d) ----- "RESET "\n",n,n);
    printf("Enter elements (row by row):\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &A[i][j]);
        }
    }

    //Input vector v
    printf("\n"CYAN "----- Input Vector V (size %d) ----- "RESET "\n",n);
    printf("Enter elements: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &v[i]);
    }
    printf("\n");

    // Perform matrix-vector multiplication
    printf("\n"CYAN "----- Computing Multiplication ----- "RESET "\n");
    matrixVectorMultiplication(A, v, r, n);

    // Print the result vector r
    printf("\n"BROWN "===== Computing Multiplication ===== "RESET "\n");
    printf("Result vector r:\n");
    for (int i = 0; i < n; i++) {
        printf("%d ", r[i]);
    }
    printf("\n\n");

    // Free allocated memory
    for (int i = 0; i < n; i++) {
        free(A[i]);
    }
    free(A);
    free(v);
    free(r);
    
    return 0;
}
