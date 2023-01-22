/* PrimeFiltering:
    This program filters an array of numbers and writes the prime numbers to a text file.
    It uses multiple threads to improve performance. The program reads in multiple test
    files containing a list of numbers, stores then in an array, and divides the array into
    approximatly equal batches. Each batch is processed by a seperate thread, which filters
    the prime numbers and writes them to output file. The program then prints the total
    number of prime numbers found.

    run the program using this command:
        gcc PrimeFiltering.c -pthread -o PrimeFiltering;
        ./PrimeFiltering.c 3 PrimeData1.txt PrimeData2.txt PrimeData3.txt;
        rm PrimeFiltering
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Defines a struct to hold the parameters for the threads.
typedef struct
{
    int *array;        // Pointer to the array of numbers.
    int batch;         // The batch number of the current thread. This is used mainly for testing purposes.
    int start;         // The start index for the current batch.
    int end;           // The end index for the current batch.
    int *isPrime;      // Pointer to the array for storing the results of the prime filtering.
    int PrimeCount;    // Counter for the number of prime numbers found in the current batch.
    FILE *output_file; // Pointer to the output file.

} Parameter;

// Function for filtering prime numbers in a given batch.
// Once a number is identified as Prime it'll be outputted in the FilteredPrimeNumbers.txt file.
void *is_prime(void *p)
{
    // Cast the void pointer to a Parameter pointer.
    Parameter *param = (Parameter *)p;
    // Initialize a variable to store wheter or not current number is prime.
    int was_it_prime = 0;
    // Loop through the nubers in the current batch
    for (int i = param->start; i < param->end; i++)
    {
        // Store the current number in the current batch.
        int number = param->array[i];
        // Check if the number in  seperate variable.
        if (number <= 1)
        {
            // If the number is less than equal to 1, it is not prime.
            was_it_prime = 0;
        }
        else
        {
            // If the number is greater than 1, assume it is prime.
            was_it_prime = 1;
            // Loop through the potential divisors of the number.
            for (int j = 2; j * j <= number; j++)
            {
                // If the number has a divisor of the number.
                if (number % j == 0)
                {
                    was_it_prime = 0;
                }
            }
        }
        // Increase the PrimeCount by the value of was_it_prime.
        param->PrimeCount += was_it_prime;
        // Store the result of the prime filtering in the isPrime array.
        param->isPrime[i] = was_it_prime;

        // Test to ensure the all thread are working as intended.
        // printf("Batch Number:\t %d\tcurrent Result =\t%d\tPrime count each batch %d\n", param->batch, param->isPrime[i], param->PrimeCount);

        // Write the prime number to the file if it is prime.
        if (param->isPrime[i] == 1)
        {
            fprintf(param->output_file, "%d\n", param->array[i]);
        }
    }
}

int main(int argc, char *argv[])
{
    // Test out array without input from external file.
    // int fakearray[] = {1, 65, 14, 7, 34, 41, 31};

    int batches = atoi(argv[1]);

    FILE *output_file = fopen("FilteredPrimeNumbers.txt", "w");

    // Check if the FilteredPrimeNumbers.txt failed to be created.
    if (output_file == NULL)
    {
        printf("Error opening output file\n");
        return 1;
    }

    // Allocate memory for the array of numbers using calloc
    // With this new method there isn't a need to get the number of lines in the text files before creating the array unlike in Task 1.
    int *numbers = (int *)calloc(1, sizeof(int));

    int num_count = 0;

    /*
        Lines 117 - 145: Reads in the numbers from the input files and stores them in an array called 'numbers'.
        As it reads in each number, it reallocates the 'numbers' array to increase its size of necessary and
        stores the new number at the end of the array. To keep track of the number of elements in the 'numbers' array
        a variable num_count gets incremented.
    */

    // Loop through each input file
    for (int i = 2; i < argc; i++)
    {

        // Open the input file
        FILE *input_file = fopen(argv[i], "r");
        if (input_file == NULL)
        {
            printf("Error opening input file %s\n", argv[i]);
            return 1;
        }

        // Read each number from the input file and add it to the array
        int num;
        while (fscanf(input_file, "%d", &num) == 1)
        {
            // Reallocate memory for the array if necessary
            if (num_count % 1 == 0)
            {
                numbers = realloc(numbers, (num_count + 1) * sizeof(int));
            }

            // Add the number to the array
            numbers[num_count] = num;
            num_count++;
        }

        // Close the input file
        fclose(input_file);
    }
    // If the amount of numbers found in input text files exeeds the amount of threads requested. An error message will appear.
    if (num_count >= batches)
    {

        /*
            Lines 156 - 161: Divide the 'numbers' array into smaller batches and creates a seperate thread for each batch to filter the prime numbers.
            First it calculates the size of each batch by dividing the total number of elements in the 'numbers' array by the number of batches.
            It also calculates the remainder of this division, which will be used to determine how many batches will have an extra element.
            It then initializes the 'start' and 'end' variable to mark the starting and ending indices of each batch.
        */
        int batch_size = num_count / batches;
        int remainder = num_count % batches;
        int start = 0;
        int end = 0;
        int index = 0;
        int *prime_filter_placeholder = (int *)calloc(num_count, sizeof(int));

        // Create array of threads and parameter structs
        Parameter *param = (Parameter *)calloc(batches, sizeof(Parameter));
        pthread_t *threads = (pthread_t *)calloc(batches, sizeof(pthread_t));

        /*
            Lines 175 - 199: A loop is created to iterates through each batch. For each iteration, it calculates the 'end'
            index by adding the 'batch_size' to the 'start' index. If there is a remainder, it increments the 'end'
            index by 1 and decrements the remainder by 1. It then fills the 'param' struct with hr values for the
            current batch and creates a new thread using the 'pthread_create' function, passing the 'param' struct
            as a parameter. Finally, it increments the 'start' index by the 'batch_size' to prepare for the next batch.
        */

        // Loop through each batch.
        for (int i = 0; i < batches; i++)
        {
            // End will be populated by the batchsize + start.
            end = start + batch_size;
            // Any remainder will increment the end value by 1 for the current parameter and decrease the remainder value by 1.
            if (remainder > 0)
            {
                remainder--;
                end++;
            }

            // Fill the parameter struct for current thread
            param[i].array = numbers;
            param[i].end = end;
            param[i].start = start;
            param[i].batch = index + 1;
            param[i].PrimeCount = 0;
            param[i].isPrime = prime_filter_placeholder;
            param[i].output_file = output_file;

            // Creates the thread.
            pthread_create(threads + index, NULL, is_prime, (void *)&param[index]);
            index++;

            // Update start row for next batch
            start = end;
        }

        // Wait for all threads to finish processing
        for (int i = 0; i < batches; i++)
        {
            pthread_join(threads[i], NULL);
        }

        int totalPrimeCount = 0;

        for (int i = 0; i < batches; i++)
        {
            totalPrimeCount += param[i].PrimeCount;
        }

        // Write the total prime numbers filtered into the output file at the last line.
        fprintf(output_file, "The total Number of Prime Numbers found is: %d\n", totalPrimeCount);

        // Close the output file
        fclose(output_file);

        /*
            Line 226 - 229: Frees the memory that was allocated for the 'numbers', 'PrimeFilterPlaceholder'
            ,'param', 'threads', and 'emptyarray' arrays using the 'free' function. It then returns 0 to
            indicate that the program has finished successfully.
        */

        // Freeing allocated memory.
        free(numbers);
        free(param);
        free(threads);
        free(prime_filter_placeholder);
    }
    else
    {
        printf("Can't have more threads that the amount of iterations. Please try again.\nIterations selected %d\tThreads:%d\n", num_count, batches);
        return 1;
    }

    return 0;
}