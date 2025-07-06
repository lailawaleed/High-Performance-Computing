#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void encrypt(char* text, int length) 
{
    for(int i = 0; i < length; i++) 
    {
        if(text[i] >= 'a' && text[i] <= 'z') 
        {
            text[i] = 'a' + (text[i] - 'a' + 3) % 26;
        }
        else if(text[i] >= 'A' && text[i] <= 'Z') 
{
            text[i] = 'A' + (text[i] - 'A' + 3) % 26;
        }
    }
}

void decrypt(char* text, int length) 
{
    for(int i = 0; i < length; i++) 
    {
        if(text[i] >= 'a' && text[i] <= 'z') 
        {
            text[i] = 'a' + (text[i] - 'a' - 3 + 26) % 26;
        }
        else if(text[i] >= 'A' && text[i] <= 'Z') 
        {
            text[i] = 'A' + (text[i] - 'A' - 3 + 26) % 26;
        }
    }
}

int main() {
    int rank, numOfProcesses;
    int textLength, localLength;
    char *text = NULL, *localText = NULL;
    int mode;      // 0 encrypt, 1 decrypt
    int inputMode; // 0 console, 1 file
    
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numOfProcesses);
    
    if(rank == 0) 
    {
        printf("Enter input mode (0 for console, 1 for file): ");
        fflush(stdout);
        scanf("%d", &inputMode);
        
        printf("Enter mode (0 for encrypt, 1 for decrypt): ");
        fflush(stdout);
        scanf("%d", &mode);
        
        //CASE 1 : Console input
        if(inputMode == 0) 
        {
            printf("Enter the Text: ");
            fflush(stdout);
            text = (char *)malloc(1000 * sizeof(char));
            
            scanf(" %[^\n]s", text);  
            
            textLength = strlen(text);
        }
        
        //CASE 2 : File input
        else 
        {
            FILE *file;
            char filename[100];
            printf("Enter filename: ");
            fflush(stdout);
            scanf("%s", filename);
            
            file = fopen(filename, "r");
            if (file == NULL) {
                printf("Error opening file.\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
            
            fseek(file, 0, SEEK_END);
            textLength = ftell(file);
            fseek(file, 0, SEEK_SET);
            
            text = (char *)malloc((textLength + 1) * sizeof(char));
            fread(text, sizeof(char), textLength, file);
            text[textLength] = '\0';
            fclose(file);
        }
        
        int slaveCount = numOfProcesses - 1;
        int defaultSlaveSize = textLength / slaveCount;
        int remainder = textLength % slaveCount;
        int start = 0;
        
        // Send mode to all slaves
        for(int dest = 1; dest <= slaveCount; dest++) 
        {
            MPI_Send(&mode, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
        }
        
        // Send partitions to slaves
        start = 0;
        for(int dest = 1; dest <= slaveCount; dest++) 
        {
            int slaveSize = defaultSlaveSize + (dest <= remainder ? 1 : 0);
            MPI_Send(&slaveSize, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
            MPI_Send(&text[start], slaveSize, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
            start += slaveSize;
        }
        
        // Receive results from slaves
        start = 0;
        for(int src = 1; src <= slaveCount; src++) 
        {
            int slaveSize = defaultSlaveSize + (src <= remainder ? 1 : 0);
            MPI_Recv(&text[start], slaveSize, MPI_CHAR, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            start += slaveSize;
        }
        
        printf("Result is %s\n", text);
        free(text);
    }
    
    // Slave processes
    else 
    {
        MPI_Recv(&mode, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&localLength, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        localText = (char *)malloc((localLength + 1) * sizeof(char));
        MPI_Recv(localText, localLength, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        localText[localLength] = '\0';
        
        if(mode == 0) 
        {
            encrypt(localText, localLength);
        }
        else 
        {
            decrypt(localText, localLength);
        }
        
        MPI_Send(localText, localLength, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
        free(localText);
    }
    
    MPI_Finalize();
    return 0;
}
