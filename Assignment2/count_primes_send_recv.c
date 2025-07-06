#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
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
int main(){

    int rank, numProcesses;
    int x, y, r;
    double startTime, endTime;
    
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    // Master process
    if (rank == 0) {
        do {
            printf("Enter lower bound (x > 0): ");
            fflush(stdout);
            scanf("%d", &x);
            
            printf("Enter upper bound (y > x): ");
            fflush(stdout);
            scanf("%d", &y);
            
            printf("\n");
            
            if (x <= 0) fprintf(stderr, "x must be positive\n");
            if (y <= x) fprintf(stderr, "y must be greater than x\n");
        } while (x <= 0 || y <= x);

        startTime = MPI_Wtime();
        r = (y - x + 1) / (numProcesses - 1);  
        
        printf("Searching primes in [%d, %d] (inclusive) ......\n\n", x, y);
        fflush(stdout);

        //Send x, y, r to each worker
        for (int worker = 1; worker < numProcesses; worker++) 
        {
            MPI_Send(&x, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
            MPI_Send(&y, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
            MPI_Send(&r, 1, MPI_INT, worker, 0, MPI_COMM_WORLD);
        }
    }
    
    else 
    {  // Worker processes
        MPI_Recv(&x, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&y, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&r, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        int totalNumbers = y - x + 1;
        int remainder = totalNumbers % (numProcesses - 1);
        int start, end;
        int localCount = 0;    
        int* localPrimes = NULL;

        if (rank <= remainder) {
        // Workers get r+1 Numbers
            start = x + (rank - 1) * (r + 1);
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
        
        MPI_Send(&localCount, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        free(localPrimes);
    }

    // Master collects results
    if (rank == 0) {
        int totalPrimes = 0;
        for (int worker = 1; worker < numProcesses; worker++) {
            int workerCount = 0;
            MPI_Recv(&workerCount, 1, MPI_INT, worker, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            totalPrimes += workerCount;
        }

        endTime = MPI_Wtime();
        printf("\nTotal primes between %d and %d: %d\n", x, y, totalPrimes);
        printf("Execution time: %.6f seconds\n", endTime - startTime);
    }

    MPI_Finalize();
    return 0;
}
