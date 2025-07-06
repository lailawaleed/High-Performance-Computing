#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CYAN "\033[1;36m"
#define BROWN "\033[1;33m"
#define RESET "\033[0m"


double computeStdDev(int n,int *array){
    double mean=0.0, sumSquares = 0.0, variance = 0.0;

    // Compute sum for mean in parallel
    long long sum = 0;
    printf(CYAN "----- Computing Sum for Mean -----"RESET" \n");
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < n; i++) {
        int threadId = omp_get_thread_num();
        printf("  Thread %d computing element %d(value: %d)\n",threadId,i,array[i]);
        sum += array[i];
    }
    mean = (double)sum / n;
    printf("\nMean: %.6lf\n",mean);

    // Compute sum of squared differences in parallel
    printf(CYAN "----- Computing Sum for Squared Differences -----"RESET" \n");
    #pragma omp parallel for reduction(+:sumSquares)
    for (int i = 0; i < n; i++) {
        int threadId = omp_get_thread_num();
        double diff = array[i] - mean;
        sumSquares += diff * diff;
        printf("  Thread %d computing Squared diff for element %d(value: %.6lf)\n",threadId,i,diff*diff);
    }
    variance = sumSquares / n;
    printf("\nVariance: %.6lf\n",variance);

    //Return standard deviation
    return sqrt(variance);
}

int main(){
    int n;
    printf(CYAN "==== Standard Deviation Computation ===="RESET "\n");
    printf("Enter the size of the array (n): ");
    fflush(stdout);
    scanf("%d", &n);

    // Allocate memory for the array
    int *arr = (int *)malloc(n * sizeof(int));

     printf(CYAN "----- Input Array ----- "RESET "\n");
    // Input array elements
    printf("Enter %d integers: ",n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }

    // Compute standard deviation
    printf(CYAN "============================================= Computing Standard Deviation ==========================================="RESET "\n");
    double stdDev = computeStdDev(n, arr);
    printf("\n" BROWN "==== Standard Deviation: %.6lf ===="RESET "\n\n",stdDev);

    // Free allocated memory
    free(arr);
    
    return 0;
}


