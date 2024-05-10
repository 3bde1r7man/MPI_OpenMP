#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

double sqroot(double num)
{
    float x = num;
    float y = 1;
    float precision = 0.000001; // Desired precision

    while (x - y > precision)
    {
        x = (x + y) / 2;
        y = num / x;
    }

    return x;
}

int main()
{
    int n;
    printf("Write number of elements per process (n): ");
    scanf("%d", &n);
    int NumberOfThreads = omp_get_max_threads();
    double global_Sum = 0.0;

#pragma omp parallel num_threads(NumberOfThreads) reduction(+ : global_Sum)
    {
        int *PrivateArr = (int *)malloc(n * sizeof(int));
        double localSumOSqD = 0.0;
        double local_Sum = 0.0;

        // calculating local sum after generating random numbers

        srand(time(0) + omp_get_thread_num());
        for (int i = 0; i < n; i++)
        {
            PrivateArr[i] = rand() % 100; // Limit values to 0-99
            local_Sum += PrivateArr[i];
        }

        // Calculate local mean
        double localMean = local_Sum / n;

        // Calculate local sum of squared differences from the mean
        for (int i = 0; i < n; i++)
        {
            double diff = PrivateArr[i] - localMean;
            localSumOSqD += diff * diff;
        }

        // Add local sum of squared differences to global sum
        global_Sum += localSumOSqD;

        // Free the memory allocated for the private array
        free(PrivateArr);
    }
    // Calculate global mean and standard deviation
    double globalMean = global_Sum / (n * NumberOfThreads);
    double standardDeviation = sqroot(globalMean);
    printf("The Standard deviation: %.5f\n", standardDeviation);
    return 0;
}
