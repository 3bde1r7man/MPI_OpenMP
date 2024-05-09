#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int main() {
    int row, col, key;
    printf("Enter the number of rows: ");
    scanf("%d", &row);
    printf("Enter the number of columns: ");
    scanf("%d", &col);
    printf("Enter the key to search for: ");
    scanf("%d", &key);

    // Dynamically allocate a 2D array
    int** matrix = (int**) malloc(row * sizeof(int*));
    for (int i = 0; i < row; i++) {
        matrix[i] = (int*) malloc(col * sizeof(int));
    }

    // Initialize the matrix with random numbers
    srand(time(NULL));  // Seed for random number generation
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            matrix[i][j] = rand() % 10;  // Random numbers between 0 and 9
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }

    // Array to store indices of the key
    int max_hits = row * col;
    int* indxArr = (int*) malloc(2 * max_hits * sizeof(int));
    int count = 0;

    // Parallel search for the key
    #pragma omp parallel for shared(matrix, indxArr, count) // we can use collapse(2) to parallelize both loops for better performance
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < col; j++) {
            if (matrix[i][j] == key) {
                int tid = omp_get_thread_num();
                printf("Thread %d found key at index (%d, %d)\n", tid, i, j);

                // Use critical section to update shared resources safely
                #pragma omp critical
                {
                    indxArr[2 * count] = i;
                    indxArr[2 * count + 1] = j;
                    count++;
                }
            }
        }
    }

    // Check if the key was found and print results
    if (count == 0) {
        printf("-1\n");
        free(indxArr);
        indxArr = NULL;
    } else {
        printf("Key found at indices:\n");
        for (int i = 0; i < count; i++) {
            printf("(%d, %d)\n", indxArr[2 * i], indxArr[2 * i + 1]);
        }
    }

    // Free the allocated memory
    for (int i = 0; i < row; i++) {
        free(matrix[i]);
    }
    free(matrix);
    if (indxArr != NULL) {
        free(indxArr);
    }

    return 0;
}
