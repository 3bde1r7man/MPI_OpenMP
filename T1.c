#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void getVotesR1(int rank, int start, int end, int C, int V, int *votes, int (*pref)[C]){ // get votes for the first round
    for(int i = start; i < end; i++){
        int indx = pref[i-2][0];
        votes[indx-1]++;
    }
}

void getVotesR2(int rank, int start, int end, int C, int V, int *votes, int (*pref)[C], int maxIndex, int secondMaxIndex){ // get votes for the second round
    for(int i = start; i < end; i++){
        for(int j = 0; j < C; j++){
            int indx = pref[i-2][j];
            if(indx == maxIndex || indx == secondMaxIndex){
                votes[indx-1]++;
                break;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int rank, size, tag = 0;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int C, V;
    char filename[100];
    if(rank == 0){
        printf("Enter 1 to generate file or 2 to calculate the result: ");
        int choice;
        scanf("%d", &choice);
        if(choice == 1){
            strcat(filename, "voters.txt");
            FILE *file = fopen("voters.txt", "w");
            printf("Enter number of candidates: ");
            scanf("%d", &C);
            printf("Enter number of voters: ");
            scanf("%d", &V);
            fprintf(file, "%d\n%d\n", C, V);
            for(int i = 0; i < V; i++){
                printf("Enter voter %d preferences: ", i+1);
                for(int j = 0; j < C; j++){
                    int x;
                    scanf("%d", &x);
                    fprintf(file, "%d ", x);
                }
                fprintf(file, "\n");
            }
            fclose(file);
        }else{
            printf("Enter the filename: ");
            scanf("%s", filename);
            FILE *file = fopen(filename, "r");
            if(file == NULL){
                printf("File not found\n");
                MPI_Finalize();
                return 0;
            }
            fclose(file);
        }
    }
    MPI_Bcast(filename, 100, MPI_CHAR, 0, MPI_COMM_WORLD);
    FILE *file = fopen(filename, "r");
    fscanf(file, "%d\n%d\n", &C, &V);
    int votes[C];
    int pref[V][C];
    int (*Pp)[C] = pref;
    memset(votes, 0, sizeof(votes));
    memset(pref, 0, sizeof(pref));
    // make each process read its part of the file
    int p = V / size;
    int start = 0;
    int end = 0;
    if(p == 0){ 
        if(rank > V - 1){
            start = 0;
            end = 0;
        }else{
            start = rank + 2;
            end = rank + 3;
        }
    }else{
        start = rank * p + 2;
        end = (rank == size - 1) ? V + 2 : start + p ;
    }

    fseek(file, 0, SEEK_SET); // go to the beginning of the file
    for(int i = 0; i < V + 2; i++){
        if(i < 2){
            int temp;
            fscanf(file, "%d", &temp);
            continue;
        }
        if(i < start){
            for(int j = 0; j < C; j++){ // skip the preferences of the voters that are not in the range of the process
                int x;
                fscanf(file, "%d", &x);
            }
        }else if(i >= start && i < end){ // read the preferences of the voters that are in the range of the process
            for(int j = 0; j < C; j++){
                fscanf(file, "%d", &pref[i-2][j]);
            }
        }else{ // skip the preferences of the voters that are not in the range of the process
            break;
        }
    }
    fclose(file);
    getVotesR1(rank, start, end, C, V, votes, Pp); // get votes for the first round

    int totalVotes[C];
    MPI_Reduce(votes, totalVotes, C, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    int maxVotes = -1, maxIndex = -1, secondMaxVotes = -1, secondMaxIndex = -1;
    if(rank == 0){  
        for(int i = 0; i < C; i++){
            if(totalVotes[i] > maxVotes){
                secondMaxVotes = maxVotes;
                secondMaxIndex = maxIndex;
                maxVotes = totalVotes[i];
                maxIndex = i;
            }else if(totalVotes[i] > secondMaxVotes){
                secondMaxVotes = totalVotes[i];
                secondMaxIndex = i;
            }
        }
        if(maxVotes > (double) V/2){
            printf("Candidate %d wins the elections in the first round with %d votes\n", maxIndex+1, maxVotes);
        }else{
            printf("Candidate %d and Candidate %d will compete in the second round\n", maxIndex+1, secondMaxIndex+1);
        }
    }
    MPI_Bcast(&maxIndex, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&secondMaxIndex, 1, MPI_INT, 0, MPI_COMM_WORLD);
    if(!(maxVotes > (double) V/2)){
        memset(votes, 0, sizeof(votes));
        getVotesR2(rank, start, end, C, V, votes, Pp, maxIndex+1, secondMaxIndex+1);
        MPI_Reduce(votes, totalVotes, C, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(rank == 0){
            int maxVotes = 0, maxIndex = 0;
            for(int i = 0; i < C; i++){
                if(totalVotes[i] > maxVotes){
                    maxVotes = totalVotes[i];
                    maxIndex = i;
                }
            }
            printf("Candidate %d wins the elections in the second round with %d votes\n", maxIndex+1, maxVotes);
        }
    }
    MPI_Finalize();
    return 0;
}