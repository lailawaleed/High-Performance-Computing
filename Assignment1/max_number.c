#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>

int main()
{
    int rank, numOfProcesses;
    int sizeOfArray, *array = NULL;
    int globalMaxValue, globalMaxIndex;
    int localArraySize, *localArray = NULL;
    int localMaxValue, localMaxIndex;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);

    // Master process (rank 0)
    if(rank == 0)
    {
        printf("Hello from master process.\n");
        printf("Number of slave processes is %d\n\n", numOfProcesses - 1);
        printf("Please enter size of array...\n");
        fflush(stdout);
        scanf("%d", &sizeOfArray);
        printf("\n");

        // Allocate and fill the array
        array = (int*)malloc(sizeOfArray * sizeof(int));

        printf("Please enter array elements...\n");
        fflush(stdout);
        for(int i = 0; i < sizeOfArray; i++)
        {
            scanf("%d", &array[i]);
        }
        printf("\n");

        // Calculate how to distribute array among slaves
        int slaveCount = numOfProcesses - 1;
        // Ensure we don't have more slaves than array elements
        int activeSlaves = (slaveCount > sizeOfArray) ? sizeOfArray : slaveCount;
        int defaultSlaveSize = (activeSlaves > 0) ? sizeOfArray / activeSlaves : 0;
        int remainder = (activeSlaves > 0) ? sizeOfArray % activeSlaves : 0;
        int start = 0;

        // Send array portions to each active slave
        for(int dest = 1; dest <= slaveCount; dest++)
        {
            if(dest <= activeSlaves)
            {
                int slaveSize = defaultSlaveSize + (dest <= remainder ? 1 : 0);
                MPI_Send(&slaveSize, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                MPI_Send(&array[start], slaveSize, MPI_INT, dest, 0, MPI_COMM_WORLD);
                start += slaveSize;
            }
            else
            {
                // Tell extra slaves they have no work
                int slaveSize = 0;
                MPI_Send(&slaveSize, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            }
        }

        // Initialize with minimum possible integer value
        globalMaxValue = INT_MIN;
        globalMaxIndex = -1;

        // Receive results from active slaves only
        for(int src = 1; src <= activeSlaves; src++)
        {
            int maxSlaveValue, maxSlaveIndex;
            MPI_Recv(&maxSlaveValue, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&maxSlaveIndex, 1, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // Calculate the global index
            int offset = 0;
            for(int i = 1; i < src; i++)
            {
                int slaveSize = defaultSlaveSize + (i <= remainder ? 1 : 0);
                offset += slaveSize;
            }
            int globalIndex = offset + maxSlaveIndex;

            if(maxSlaveValue > globalMaxValue || globalMaxIndex == -1)
            {
                globalMaxValue = maxSlaveValue;
                globalMaxIndex = globalIndex;
            }
        }

        printf("Master process announce the final max which is %d and its index is %d.\n\n",
               globalMaxValue, globalMaxIndex);
        printf("Thanks for using our program \n");
        free(array);
    }

    // Slave processes (rank > 0)
    else
    {
        MPI_Recv(&localArraySize, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(localArraySize > 0)
        {
            localArray = (int *)malloc(localArraySize * sizeof(int));
            MPI_Recv(localArray, localArraySize, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            localMaxValue = localArray[0];
            localMaxIndex = 0;
            for(int i = 1; i < localArraySize; i++)
            {
                if(localArray[i] > localMaxValue)
                {
                    localMaxValue = localArray[i];
                    localMaxIndex = i;
                }
            }

            printf("Hello from slave#%d Max number in my partition is %d and index is %d.\n",
                   rank, localMaxValue, localMaxIndex);
            fflush(stdout);

            // Send Result back to Master
            MPI_Send(&localMaxValue, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&localMaxIndex, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            free(localArray);
        }
    }

    MPI_Finalize();
    return 0;
}
