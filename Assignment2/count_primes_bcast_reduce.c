#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <stdbool.h>

bool isPrime(int n) 
{
    if (n == 1) {
        return false;
    }
    
    if( n == 2 || n == 3 ){
       return true;
    }
    
    if (!(n % 2) || !(n % 3)) {
        return false;
    }

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) {
            return false;
        }
    }
    return true;
}

void countPrimes(int start, int end,int* primes,int* count) {
    *count = 0;
    for (int i = start; i <= end; i++) {
        if (isPrime(i)) {
           primes[(*count)++]=i;
        }
    }
}

int main() {

    int rank, size;
    int x, y, r;
    double startTime, endTime;
    
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Master process (rank 0)
    if (rank == 0) {
        do 
        {
            printf("Enter Lower bound (x > 0): ");
            fflush(stdout);
            scanf("%d", &x);
            
            
            printf("Enter Upper bound (y > x): ");
            fflush(stdout);
            scanf("%d", &y);
            
            printf("\n");
          
            
            if (x <= 0) printf("x must be positive\n");
            if (y <= x) printf("y must be greater than x\n");
            
        } while (x <= 0 || y <= x);

        r = (y - x + 1) / (size - 1);
        
        printf("Searching primes in [%d, %d] (inclusive) ......\n\n", x, y);
        fflush(stdout);
        
        startTime = MPI_Wtime();
    }
    
    MPI_Bcast(&x, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&y, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&r, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int localCount = 0;    
    int* localPrimes = NULL;
    
    // Worker processes (rank > 0)
    if (rank != 0) {
        int totalNumbers = y - x + 1;
        int remainder = totalNumbers % (size - 1);
        
        int start, end;

        if (rank <= remainder) {
            start = x + (rank - 1) * (r + 1); // Workers get r+1 Numbers
            end = start + r;

        } 
        else {
           //Remaining Workers get exactly 'r' Numbers
            start = x + remainder * (r + 1) + (rank - 1 - remainder) * r;
            end = start + r - 1;
        }

        if (end > y) end = y;
        
        localPrimes = (int*)malloc((end- start + 1) * sizeof(int));
        countPrimes(start, end,localPrimes,&localCount);
        
        printf("P#%d [%d-%d] found %d primes numbers in my partition : [", 
               rank, start , end , localCount);
               
        for(int i = 0 ; i < localCount ;i++){
           printf("%d",localPrimes[i]);
           if(i!=localCount-1){
             printf(",");
           }
        }
        printf("]\n");
        fflush(stdout);
        
        free(localPrimes);
    }
    
    int totalCount;
    MPI_Reduce(&localCount, &totalCount, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        endTime = MPI_Wtime();
        printf("\nMaster: total primes between %d and %d: %d.\n", 
               x, y, totalCount);
               
        printf("Execution time: %.6f seconds\n\n", endTime - startTime);
        fflush(stdout);
    }
    

    MPI_Finalize();
    
    return 0;
}
