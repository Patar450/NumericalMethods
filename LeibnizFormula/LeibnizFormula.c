/*LeibnizFormula:
This program calculates Pi using the Leibniz formula. It reads in user inputs from the command line and creates threads to perform the calculations.
The program gets the number of iterations and number of batches( for multi threading) and stores them in variables.

To run code:
    gcc LeibnizFormula.c -pthread -lm -o LeibnizFormula;
    ./LeibnizFormula 9999 4000;
    rm LeibnizFormula
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct
{
    int *array;
    int batch;
    double pi;
    int start;
    int end;
} Parameter;

/* The leibnizFormula function calculates the sum of the Leibniz formula applied to each element in an array within a specified range.
   It then stores the result in the pi member of a parameter struct.
   For testing purposes I added printf for the batch number, start and end points, index, and value of each element in the array to gain better understanding of each process for each batch.
*/

void *leibnizFormula(void *p)
{
    Parameter *param = (Parameter *)p;
    double term = 0, sum = 0;

    /* Applying Leibniz Formula */
    for (int i = param->start; i < param->end; i++)
    {
        // printf("Batch number: %d\n", param->batch);
        // printf("Start:%d\tEnd:%d\tIndex:%d\tValue:%d\n", param->start, param->end, param->batch, param->array[i]);
        term = pow(-1, i) / (2 * i + 1);
        sum += term;
    }
    int holder = param->batch;
    param->pi += 4 * sum;
}

int main(int argc, char **argv)
{
    // Getting the user input from terminal
    int numOfIterations = atoi(argv[1]);
    int batches = atoi(argv[2]);

    // for loop to check if the user requested more threds than there are iterations.
    // If the number of threads is high then the terminal will print out an error message.
    if (numOfIterations > batches)
    {
        // Creating an array to fill it with iterations from 0 - numOfIterations
        int *array = (int *)calloc(numOfIterations, sizeof(int));

        for (int i = 0; i < numOfIterations; i++)
        {
            array[i] = i;
        }

        // This section ensure that all threads will use 'equal' amount of processes.
        // This is done by getting the amount of threads / total amount of iterations.
        // First we needs to find what each thread will process each section of iterations equally.
        int batch_size = numOfIterations / batches;
        // If there is an odd number, each one will be added from batch 0 onwards untill the amonut in remainder becomes 0.
        int remainder = numOfIterations % batches;

        // Creating the foundation of variable that will be manupuated to store sections of the array we created earlier on.
        int start = 0;
        int end = 0;
        int index = 0;

        // Struct that will store all the required variables.
        Parameter *param = (Parameter *)calloc(batches, sizeof(Parameter));
        // Creating threads by the amount of batches the user entered.
        pthread_t *threads = (pthread_t *)calloc(batches, sizeof(pthread_t));

        /*  
            Lines: 90 - 111: This loop iterates through each batch. For each iteration, it calculates the 'end' for the current batch by adding the 'batch_size' to the 'start'.
            If there is a remainder, it increments the 'end' by 1 and decrements the 'remainder' by 1.
            It then assigns the 'array', 'end', 'start', and 'batch' number to a parameter struct and creates a thread using the leibnizFormula function and the parameter struct as input.
            The 'start' for the next iteration is then updated to the 'end' point of the current iteration and the 'index' is incremented.
        */

        // Loop through each batch.
        for (int i = 0; i < batches; i++)
        {
            // End will be populated by the batchsize + start.
            end = start + batch_size;
            // any remainder will increment the end value by 1 for the current parameter and decrease the remainder value by 1.
            if (remainder > 0)
            {
                remainder--;
                end++;
            }

            // Fill the parameter
            param[i].array = array;
            param[i].end = end;
            param[i].start = start;
            param[i].batch = index;

            // Call the thread.
            pthread_create(threads + i, NULL, leibnizFormula, (void *)&param[i]);
            index++;
            start = end;
        }

        for (int i = 0; i < batches; i++)
        {
            pthread_join(threads[i], NULL);
        }

        double truestPi = 0;
        for (int i = 0; i < batches; i++)
        {
            truestPi += param[i].pi;
        }

        //Prints the closest Pi number as specified by the iterations from user.
        printf("Pi is: %.5f\n", truestPi);

        // Freeing allocated memory.
        free(param);
        free(threads);
        free(array);
    }
    else
    {
        printf("Can't have more threads that the amount of iterations. Please try again.\nIterations selected %d\tThreads:%d\n", numOfIterations, batches);
    }
    return 0;
}
