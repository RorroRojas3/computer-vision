// Author: Rodrigo Ignacio Rojas Garcia
// Lab #: 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HEADER_MAX 320

int main(int argc, char *argv[])
{
    FILE *file;
    unsigned char *input_image;
    unsigned char *smoothed_image;
    char image_header[HEADER_MAX];
    int ROWS, COLS, BYTES;
    int row1, row2, col1, col2;
    struct timespec start_time, end_time;
    int index = 0;
    int sum = 0;
    long int total_time = 0;
    long int average_time = 0;

    // Read in image
    file = fopen("bridge.ppm", "rb");
    if (file == NULL)
    {
        printf("Error, unable to open image file\n");
        exit(1);
    }

    // Read in header
    fscanf(file, "%s %d %d %d\n", image_header, &COLS, &ROWS, &BYTES);
    
    // Check that image is binary grey-scale image
    if ((strcmp(image_header, "P5") != 0) || (BYTES != 255))
    {
        printf("Error, not a greyscale 8-bit PPM image\n");
        exit(1);
    }

    // Allocate memory for image
    input_image = (unsigned char *)calloc(ROWS * COLS, sizeof(unsigned char));

    image_header[0] = fgetc(file);

    fread(input_image, sizeof(char), ROWS * COLS, file);

    fclose(file);

    // Allocate memory for smoothed image
    smoothed_image = (unsigned char *)calloc(ROWS * COLS, sizeof(unsigned char));
    
    int i;
    for (i = 0; i < 10; i++)
    {

        // Start Time
        clock_gettime(CLOCK_REALTIME, &start_time);
        printf("Starting time: %ld s, %ld ns\n", (long int)start_time.tv_sec, start_time.tv_nsec);

        // Smooth image - 2D Convolution Algorithm
        for (row1 = 3; row1 < (ROWS - 3); row1++)
        {
            for (col1 = 3; col1 < (COLS - 3); col1++)
            {
                sum = 0;
                for (row2 = -3; row2 < 4; row2++)
                {
                    for (col2 = -3; col2 < 4; col2++)
                    {

                        index = (row1 + row2) * COLS + (col1 + col2);
                        sum += input_image[index];
                    }
                }
                index = (row1 * COLS) + col1;
                smoothed_image[index] = (int)sum / 49;
            }
        }

        // Stop time
        clock_gettime(CLOCK_REALTIME, &end_time);
        printf("End time: %ld s, %ld ns\n", (long int)end_time.tv_sec, end_time.tv_nsec);

        total_time =  total_time + abs((end_time.tv_nsec - start_time.tv_nsec));
        printf("Total time: %ld ns\n", total_time);
        printf("\n");
    }

    average_time = total_time / 10;
    printf("Average time (ran 10 times): %ld ns\n", average_time);
    
    // Create and write smoothed image file
    file = fopen("pic1.ppm", "w");
    fprintf(file, "P5 %d %d 255\n", COLS, ROWS);
    fwrite(smoothed_image, COLS * ROWS, sizeof(char), file);
    fclose(file);

    return 0;
}
