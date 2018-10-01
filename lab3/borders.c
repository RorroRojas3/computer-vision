#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLENGTH 256

unsigned char *read_in_image(int rows, int cols, char file_header[], FILE *image_file)
{
	// Variable Declaration Section
	unsigned char *image;
	
	image= (unsigned char *)calloc(rows * cols, sizeof(unsigned char));
	
	file_header[0] = fgetc(image_file);
	
	fread(image, sizeof(unsigned char), rows * cols, image_file);
	
	fclose(image_file);
	
	return image;
}

/* CREATE AND SAVE FILE AS PPM IMAGE */
void save_image(unsigned char *image, char *file_name, int rows, int cols)
{
    // Variable Declaration Section
    FILE * file;
    file = fopen(file_name, "w");
    fprintf(file, "P5 %d %d 255\n", cols, rows);
    fwrite(image, rows * cols, sizeof(unsigned char), file);
    fclose(file);
}

int main(int argc, char *argv[])
{
	FILE *image_file;
	FILE *ground_truth;
	int IMAGE_ROWS, IMAGE_COLS, IMAGE_BYTES;
	int c1, c2;
	int rows, cols;
	char letter[3];
	char file_header[MAXLENGTH];
	unsigned char *input_image;
	unsigned char *output_image;
	
	/* OPEN IMAGE AND MSF IMAGE FILES AND OBTEIN THEIR INFORMATION */
    image_file = fopen(argv[1], "rb");
    if (image_file == NULL)
    {
        printf("Error, could not read input image file\n");
        exit(1);
    }
    fscanf(image_file, "%s %d %d %d\n", file_header, &IMAGE_COLS, &IMAGE_ROWS, &IMAGE_BYTES);
    if ((strcmp(file_header, "P5") != 0) || (IMAGE_BYTES != 255))
    {
        printf("Error, not a greyscale 8-bit PPM image\n");
        fclose(image_file);
        exit(1);
    }
	
	/* ALLOCATE MEMORY AND READ IN INPUT IMAGE */
    input_image = read_in_image(IMAGE_ROWS, IMAGE_COLS, file_header, image_file);
	
	output_image = (unsigned char *)calloc(IMAGE_ROWS * IMAGE_COLS, sizeof(unsigned char));
	
	for (c1 = 0; c1 < (IMAGE_ROWS * IMAGE_COLS); c1++)
	{
		output_image[c1] = input_image[c1];
	}
	
	ground_truth = fopen(argv[2], "r");
	if (ground_truth == NULL)
	{
		printf("Error, could not read Ground truth text file\n");
		exit(1);
	}
	
	int index = 0;
	
	while((fscanf(ground_truth, "%s %d %d\n", letter, &cols, &rows)) != EOF)
	{
		c2 = rows - 7;
		for (c1 = cols - 4; c1 <= (cols + 4); c1++)
		{
			index = (c2 * IMAGE_COLS) + c1;
			output_image[index] = 150;
		}
		
		c2 = cols + 4;
		for (c1 = rows - 7; c1 <= (rows + 7); c1++)
		{
			index = (c1 * IMAGE_COLS) + c2;
			output_image[index] = 150;
		}
		
		c2 = rows + 7;
		for (c1 = cols + 4; c1 >= (cols - 4); c1--)
		{
			index = (c2 * IMAGE_COLS) + c1;
			output_image[index] = 150;
		}
		
		c2 = cols - 4;
		for (c1 = rows + 7; c1 >= (rows - 7); c1--)
		{
			index = (c1 * IMAGE_COLS) + c2;
			output_image[index] = 150;
		} 
	}
	
	save_image(output_image, "thinned_with_squares.ppm", IMAGE_ROWS, IMAGE_COLS);
	

	return 0;
}


