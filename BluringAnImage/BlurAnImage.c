/**
 * Blur an image: 
 * This program applies a blur filter to an image using multiple threads. It reads in an image file by using 'lodepng.h' header, splits 
 * the image into batches of rows, and creates a thread to process each seperated batch. Each thread applies the blur filter to its batch of rows 
 * by calculating the average value of a pixel (which was shown in the assignment task) and its surrounding pixels, and setting the value of the 
 * current pixel to the calculated average value. The program then writes the output image with the blur filter applied back to a file.
*/

// To run code:

//  gcc BlurAnImage.c -lm lodepng.c -lpthread -o BlurAnImage; 
//  ./BlurAnImage 300 selfie.png; 
//  rm BlurAnImage

#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"
#include <math.h>
#include <pthread.h>

// Struct to store information needed for each thread to apply blur filter
typedef struct
{
    int batch;                     // Number of batch the thread is processing
    int start;                     // Starting row of batch
    int end;                       // Ending row of batch
    unsigned char *image;          // Original image
    unsigned int width;            // Width of image
    unsigned char **blurred_image; // Blurred image
} Parameter;

/**
 * This function applies a blur filter to an image. It does this by considering the current pixel and its
 * surrounding pixels(a 3x3 grid), calculating the average value of those pixels, and setting the value of
 * the current pixel to the calculated average value.
 */
void *apply_blur_filter(void *p)
{
    Parameter *param = (Parameter *)p;

    // Initialise the outer loop with param->start and end with param->end to ensure the threads don't use unauthorized rows form other threads.
    for (int row = param->start; row < param->end; row++)
    {
        int col_pos = 0;
        // The inner loop, will loop through each pixel of the entire row.
        for (int col = 0; col < param->width * 4; col = col + 4)
        {
            /* These variables will be used to keep track of the sum of the red, green, and blue values of the surrounding pixels,
               and the number of surrounding pixels that have been considered.
            */
            int sumR = 0;
            int sumG = 0;
            int sumB = 0;
            int count = 0;
            // Within the i and j loop, loop through each row and column of pixels surrounding the current pixel.
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    /* For each surrounding pixel, check if it is within the bounds of the image. If it is, add its red, green,
                       and blue values to the corresponding sumR, sumG, and sumB variables, and increment count by 1.
                    */
                    if (row + i >= 0 && row + i < param->end && col / 4 + j >= 0 && col / 4 + j < param->width)
                    {
                        // Add values of surrounding pixels to sum
                        sumR += param->image[(row + i) * param->width * 4 + (col / 4 + j) * 4 + 0];
                        sumG += param->image[(row + i) * param->width * 4 + (col / 4 + j) * 4 + 1];
                        sumB += param->image[(row + i) * param->width * 4 + (col / 4 + j) * 4 + 2];
                        // For each pixel used the count will increment itself.
                        count++;
                    }
                }
            }
            /* After all surrounding pixels have been considered the following will occure: 
                * divide sumR by count to get the average red value
                * divide sumG by count to get the average green value 
                * divide sumB by count to get the average blue value.
                
                Set the value of the current pixel to the calculated average values, but leave the Alpha value the original value.

            */
            param->blurred_image[row][col] = sumR / count;
            param->blurred_image[row][col + 1] = sumG / count;
            param->blurred_image[row][col + 2] = sumB / count;
            param->blurred_image[row][col + 3] = param->image[row * param->width * 4 + col + 3];

            // print out the pixel values for testing purposes.
            //printf("Batch num: %d\tR: %d\tG:%d\tB:%d\n", param->batch, param->blurred_image[row][col_pos], param->blurred_image[row][col_pos + 1], param->blurred_image[row][col_pos + 2]);

            col_pos += 4; // Move to next pixel in row used for testing.

            //After the current pixel is 'Averaged', the loop will go to the next pixel.
        }
    }
}

int main(int argc, char *argv[])
{
    // Check if correct number of arguments are provided, if not an error message is printed.
    if (argc != 3)
    {
        printf("Usage: ./program_name input_image.png num_threads\n");
        return EXIT_FAILURE;
    }
    // Convert number of threads input to int
    int num_threads = atoi(argv[1]);

    // Load image into a 1D array of pixels
    char *filename = argv[2];
    unsigned int error;
    unsigned char *image;
    unsigned int width, height;
    error = lodepng_decode32_file(&image, &width, &height, filename);
    // Error checking for issues related to the decode32 function.
    if (error)
    {
        printf("Error %d: %s\n", error, lodepng_error_text(error));
        return EXIT_FAILURE;
    }
    // Error checking to ensure the threads amount won't be higher than the height of the image.
    else if (num_threads > height)
    {
        printf("Error cannot request more threads than height of image.\n");
        return EXIT_FAILURE;
    }
    else
    {
        // Create a new 2D array to store the blurred image. This is used for multi threads.
        unsigned char **blurred_image = malloc(height * sizeof(unsigned char *));
        for (int i = 0; i < height; i++)
        {
            blurred_image[i] = malloc(width * 4 * sizeof(unsigned char));
        }

        // Create a new 2D array to store the complete blurred image. This is used to reconstruct the image from each threads.
        unsigned char **complete_blurred_image = malloc(height * sizeof(unsigned char *));

        for (int i = 0; i < height; i++)
        {
            complete_blurred_image[i] = malloc(width * 4 * sizeof(unsigned char));
        }

        // Calculate size of each batch and leftover rows
        int batch_size = height / num_threads;
        int extra_rows = height % num_threads;

        // Create array of threads and parameter structs
        pthread_t *thread = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
        Parameter *param = (Parameter *)malloc(num_threads * sizeof(Parameter));

        int start = 0;
        int end_row = 0;
        int index = 0;

        // Algorithm used for Task 2 & Task 3.
        for (int i = 0; i < num_threads; i++)
        {
            // Calculate end_row for current batch. This is populated by batch_size + start
            end_row = start + batch_size;

            // Any remainder will increment the end value by 1 for the current parameter and decrease the remainder value by 1.
            if (extra_rows > 0)
            {
                end_row++;
                extra_rows--;
            }

            // Fill in parameter struct for current thread
            param[i].start = start;
            param[i].end = end_row;
            param[i].batch = i + 1;
            param[i].image = image;
            param[i].width = width;
            param[i].blurred_image = blurred_image;

            // Create thread and apply blur filter to designated batch of image
            pthread_create(thread + i, NULL, apply_blur_filter, (void *)&param[i]);

            // Update start row for next batch
            start = end_row;
        }

        // Wait for all threads to finish processing
        for (int i = 0; i < num_threads; i++)
        {
            pthread_join(thread[i], NULL);
        }

        // Combine batches of blurred image into a single 2D array
        for (int i = 0; i < num_threads; i++)
        {
            for (int row = param[i].start; row < param[i].end; row++)
            {
                for (int col = 0; col < width * 4; col++)
                {
                    complete_blurred_image[row][col] = param[i].blurred_image[row][col];
                }
            }
        }

        // Convert 2d array back to 1 d array as decoding complete_blurred_image as a 2d array will warp the image.
        unsigned char *complete_blurred_image_flat = malloc(height * width * 4 * sizeof(unsigned char));
        int pos = 0;
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width * 4; j++)
            {
                complete_blurred_image_flat[pos] = complete_blurred_image[i][j];
                pos++;
            }
        }

        // Print details of the image for testing the encode function to ensure that it captures the pixels and gives me a better perspective of the objective.
        // printf("Original image: width: %d height: %d\n", width, height);
        // for (unsigned int row = 0; row < height; row++)
        // {
        //     for (unsigned int col = 0; col < width * 4; col = col + 4)
        //     {
        //         printf("Pixel at (%d, %d): R = %d, G = %d, B = %d, A = %d\n", row, col / 4, image[row * width * 4 + col], image[row * width * 4 + col + 1], image[row * width * 4 + col + 2], image[row * width * 4 + col + 3]);
        //     }
        // }
        // Print details of blurred image pixels for testing the math and using libre to compare pixel values.
        // printf("Blurred image:\n");
        // for (unsigned int row = 0; row < height; row++)
        // {
        //     for (unsigned int col = 0; col < width * 4; col = col + 4)
        //     {
        //         printf("Pixel at (%d, %d): R = %d, G = %d, B = %d, A = %d\n", row, col / 4, complete_blurred_image[row][col], complete_blurred_image[row][col + 1], complete_blurred_image[row][col + 2], complete_blurred_image[row][col + 3]);
        //     }
        // }
        /// Encode and save the blurred image
        error = lodepng_encode32_file("blurred.png", complete_blurred_image_flat, width, height);
        if (error)
        {
            printf("Error %d: %s\n", error, lodepng_error_text(error));
            return EXIT_FAILURE;
        }

        // Freeing allocated memory
        free(thread);
        free(param);
        // Freeing the 2nd dimenenstion of the array in a 2d array.
        for (int i = 0; i < height; i++)
        {
            free(blurred_image[i]);
            free(complete_blurred_image[i]);
        }
        free(image);
        free(blurred_image);
        free(complete_blurred_image);
        return 0;
    }
}