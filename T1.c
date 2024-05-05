// Here are the rules for the election process:
// 1. There are C candidates (numbered from 1 to C), and V voters.
// 2. The election process consists of up to 2 rounds. 
// All candidates compete in the first round. 
// If a candidate receives more than 50% of the votes, he wins, otherwise another round takes place,
// in which only the top 2 candidates compete for the presidency,
// the candidate who receives more votes than his opponent wins and becomes the new president.
// 3. The voters' preferences are the same in both rounds, 
// and each voter must vote exactly once in each round for a currently competing candidate according to his preferences.
// Given the preferences lists, you need to write a program to announce which candidate will win and in which round.


// You must use files so firstly generate by the code file which contains the voters' preferences, 
// and the format of the file must be number of candidates in the first line and number of voters in the second line, 
// and then followed by voters' preferences equal to number of voters.


// Note: when running the program the file must not be loaded by one process but every running process should loads its part.


// Run Steps you must follow: When run the program prompt the user to generate file like above of voter's preferences or
// to calculate the result and if the user choose the second option enter the filename as input.


// The Output: Print to the console the steps happening in every process and print which candidate will win the elections
// and in which round. And if there are 2 rounds identity that also and show the percentage of every candidate per each round.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>


int main(int argc, char *argv[])
{
    int rank, size, tag = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int C, V;
    if(rank == 0){
        printf("Enter 1 to generate file or 2 to calculate the result: ");
        int choice;
        scanf("%d", &choice);
        if(choice == 1){
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
            char filename[100];
            scanf("%s", filename);
            FILE *file = fopen(filename, "r");
            fscanf(file, "%d\n%d\n", &C, &V);
            fclose(file);
        }
    }
    MPI_Bcast(&C, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&V, 1, MPI_INT, 0, MPI_COMM_WORLD);
    printf("Process %d: C = %d, V = %d\n", rank, C, V);

    MPI_Finalize();
}