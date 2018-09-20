#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define HEADER_MAX 320

int main(int argc, char *argv[])
{
    // Variable Declaration Section
    FILE *file;
    unsigned char *input_image;
    unsigned char *output_image;
    float *temp_output;
    char image_header[HEADER_MAX];
    int ROWS, COLS, BYTES;
    int row1, col1;
    struct timespec start_time, end_time;
    int index = 0;
    float sum = 0;
    int previous_pixel = 0;
    int current_pixel = 0;
    long int total_time = 0;
    long int average_time = 0;
    int storage_row;

    // Read image
    file = fopen("bridge.ppm", "rb");
    if (file == NULL)
    {
        printf("Error, unable to open image file\n");
        exit(1);
    }

    // Read in header of image
    fscanf(file, "%s %d %d %d\n", image_header, &COLS, &ROWS, &BYTES);

    // Check that image is a binary grey-scale image
    if ((strcmp(image_header, "P5") != 0) || (BYTES != 255))
    {
        printf("Error, not a greyscale 8-bit PPM image\n");
        exit(1);
    }

    // Allocate memory for input image
    input_image = (unsigned char *)calloc(ROWS * COLS, sizeof(unsigned char));

    image_header[0] = fgetc(file);

    fread(input_image, sizeof(char), ROWS * COLS, file);
    fclose(file);

    // Allocate memory for temporary output and smoothed image
    temp_output = (float *)calloc(ROWS * COLS, sizeof(float));
    output_image = (unsigned char *)calloc(ROWS * COLS, sizeof(unsigned char));

    int i;
    for (i = 0; i < 10; i++)
    {
        // Start time
        clock_gettime(CLOCK_REALTIME, &start_time);
        printf("Starting time: %ld s, %ld ns\n", (long int)start_time.tv_sec, start_time.tv_nsec);
        
        // Smoothe image - Sliding Window Algorithm
        // Columns part
        for (row1 = 0; row1 < ROWS; row1++)
        {
            sum = 0;
            for (col1 = 0; col1 < COLS; col1++)
            {
                if (col1 < 7)
                {
                    index = (row1 * COLS) + col1;
                    sum += input_image[index];
                    if (col1 == 6)
                    {
                        index = (row1 * COLS) + 3;
                        temp_output[index] = sum;
                    }
                }
                else
                {
                    current_pixel = input_image[(row1 * COLS) + col1];
                    previous_pixel = input_image[(row1 * COLS) + (col1 - 7)];
                    sum  = current_pixel + (sum - previous_pixel);
                    index = (row1 * COLS) + (col1 - 3);
                    temp_output[index] = sum;
                }
            }
        }

        // Row part
        for (col1 = 0; col1 < COLS; col1++)
        {
            sum  = 0;
            storage_row = 3;
            for (row1  = 0; row1 < ROWS; row1++)
            {
                if (row1 < 7)
                {
                    index = (row1 * COLS) + col1;
                    sum += temp_output[index];
                    if (row1 == 6)
                    {
                        index = (storage_row * COLS) + col1;
                        output_image[index] = (int) sum / 49.0;
                        storage_row++;
                    }
                }
                else
                {
                    current_pixel = temp_output[(row1 * COLS) + col1];
                    previous_pixel = temp_output[((row1 - 7) * COLS) + col1];
                    sum = current_pixel + (sum - previous_pixel);
                    index = (storage_row * COLS) + col1;
                    output_image[index] = (int) sum / 49.0;
                    storage_row++;
                }
            }
        }

        // Stop time 
        clock_gettime(CLOCK_REALTIME, &end_time);
        printf("End time: %ld s, %ld ns\n", (long int)end_time.tv_sec, end_time.tv_nsec);

        total_time += end_time.tv_nsec - start_time.tv_nsec;

        printf("Total time: %ld ns\n", total_time);
        printf("\n");
    }

    average_time = total_time / 10;
    printf("Average time (ran 10 times): %ld ns\n", average_time);

    // Create and write smoothed image file
    file = fopen("pic3.ppm", "w");
    fprintf(file, "P5 %d %d 255\n", COLS, ROWS);
    fwrite(output_image, COLS * ROWS, sizeof(char), file);
    fclose(file);

    return 0;
}
