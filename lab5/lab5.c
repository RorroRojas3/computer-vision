// Library Declaration Section
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Definition Section
#define MAXLENGTH 256
#define MAXITERATION 30
#define NEWMIN 0
#define NEWMAX 255
#define SQUARE(x) ((x) * (x))

/* READS IN IMAGE */
unsigned char *read_in_image(int rows, int cols, FILE *image_file)
{
	// Variable Declaration Section
	unsigned char *image;

	image = (unsigned char *)calloc(rows * cols, sizeof(unsigned char));

	fread(image, sizeof(unsigned char), rows * cols, image_file);

	fclose(image_file);

	return image; 
}

/* CREATES AND SAVES FILE AS A PPM IMAGE */
void save_image(unsigned char *image, char *file_name, int rows, int cols)
{
	// Variable Declaration Section
	FILE *file;

	file = fopen(file_name, "w");
	fprintf(file, "P5 %d %d 255\n", cols, rows);
	fwrite(image, rows * cols, sizeof(unsigned char), file);
	fclose(file);
}

/* EXTRACTS INFORMATION FROM INITIAL CONTOUR TEXT FILE*/
void read_initial_countour(char *file_name, int **contour_rows, int **contour_cols, int *file_size)
{
	// Variable Declaration Section
	FILE *file;
	int i = 0;
	int cols, rows;
	char c;
	cols = rows = 0;
	*file_size = 0;
 
	// Obtains file length and rewinds it to the beginning
	file = fopen(file_name, "r");
	if (file == NULL)
	{
		printf("Error, could not read in initial contour text file\n");
		exit(1);
	}

	while((c = fgetc(file)) != EOF)
	{
		if (c == '\n')
		{
			*file_size += 1;
		}
	}
	rewind(file);

	// Allocates memory
	*contour_rows = calloc(*file_size, sizeof(int *));
	*contour_cols = calloc(*file_size, sizeof(int *));

	// Extracts the columns and rows for the initial countour file
	while((fscanf(file, "%d %d\n", &cols, &rows)) != EOF)
	{
		(*contour_rows)[i] = rows;
		(*contour_cols)[i] = cols;
		i++;
	}

	// Closes opened file
	fclose(file);
}

/* OUTPUTS INITIAL HAWK IMAGE WITH THE CONTOURS  */
void draw_initial_contour(unsigned char *image, int image_rows, int image_cols, int **contour_rows, int **contour_cols, int arr_length)
{
	// Variable Declaration Section	
	unsigned char *output_image;
	int rows, cols;
	int i = 0;

	output_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));
	
	// Copies original image to output image
	for (i = 0; i < (image_rows * image_cols); i++)
	{
		output_image[i] = image[i];
	}

	// Draw "+" on image
	for (i = 0; i < arr_length; i++)
	{
		rows = (*contour_rows)[i];
		cols = (*contour_cols)[i];

		// "|" on COLS
		output_image[(rows - 3)*image_cols + cols] = 0;
		output_image[(rows - 2)*image_cols + cols] = 0;
		output_image[(rows - 1)*image_cols + cols] = 0;
		output_image[(rows - 0)*image_cols + cols] = 0;
		output_image[(rows + 1)*image_cols + cols] = 0;
		output_image[(rows + 2)*image_cols + cols] = 0;
		output_image[(rows + 3)*image_cols + cols] = 0;
		
		// "-" on ROWS
		output_image[(rows * image_cols) + (cols - 3)] = 0;
		output_image[(rows * image_cols) + (cols - 2)] = 0;
		output_image[(rows * image_cols) + (cols - 1)] = 0;
		output_image[(rows * image_cols) + (cols - 0)] = 0;
		output_image[(rows * image_cols) + (cols + 1)] = 0;
		output_image[(rows * image_cols) + (cols + 2)] = 0;
		output_image[(rows * image_cols) + (cols + 3)] = 0;
	}
	
	// Saves image with initial contour labeled on them as a "+"
	save_image(output_image, "hawk_initial_contour.ppm", image_rows, image_cols);
	
	free(output_image);
}

/* CALCULATES THE MINIMUM AND MAXIMUM VALUE IN EACH PIXEL */
void find_min_and_max(int *convolution_image, int image_rows, int image_cols, int *min, int *max)
{
	int i;
	*min = convolution_image[0];
	*max = convolution_image[0];
	for (i = 1; i < (image_rows * image_cols); i++)
	{
		if (*min > convolution_image[i])
		{
			*min = convolution_image[i];
		}
		if (*max < convolution_image[i])
		{
			*max = convolution_image[i];
		}
	}
}

/* NORMALIZE INPUT IMAGE, RETURNS NORMALIZED IMAGE */
unsigned char *normalize(int *convolution_image, int image_rows, int image_cols, int new_min, int new_max, int min, int max)
{
	// Variable Declaration Section
	unsigned char *normalized_image;
	int i;
	
	// Allocate memory
	normalized_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));
	
	for (i = 0; i < (image_rows * image_cols); i++)
	{
		normalized_image[i] = ((convolution_image[i] - min)*(new_max - new_min)/(max-min)) + new_min;
	}
	
	return normalized_image;
}

/* OUTPUTS NORMALIZED SOBEL IMAGE AND RETURNS UN-NORMALIZED SOBEL IMAGE */
int *sobel_edge_detector(unsigned char *image, int image_rows, int image_cols)
{
	// Variable Declaration Section
	int *convolution_image;
	unsigned char *normalized_image;
	int i, j, rows, cols;
	int index1 = 0;
	int index2 = 0;
	int x = 0;
	int y = 0;
	int min = 0;
	int max = 0;

	// X and Y KERNELS
	int g_x[9] = 	{-1, 0, 1, 
					-2, 0, 2, 
					-1, 0, 1};

	int g_y[9] = 	{-1, -2, -1
					, 0, 0, 0, 
					1, 2, 1};

	// Allocate memory for image
	convolution_image = (int *)calloc(image_rows * image_cols, sizeof(int));

	// Copy original image
	for (i = 0; i < (image_rows * image_cols); i++)
	{
		convolution_image[i] = image[i];
	}

	for (rows = 1; rows < (image_rows - 1); rows++)
	{
		for (cols = 1; cols < (image_cols - 1); cols++)
		{
			x = 0;
			y = 0;
			for (i = -1; i < 2; i++)
			{
				for (j = -1; j < 2; j++)
				{
					index1 = (image_cols * (rows + i)) + (cols + j);
					index2 = 3*(i + 1) + (j + 1);
					x += (image[index1] * g_x[index2]);
					y += (image[index1] * g_y[index2]);
				}
			}
			index1 = (image_cols * rows) + cols;
			convolution_image[index1] = (int)sqrt(SQUARE(x) + SQUARE(y));
		}
	}

	find_min_and_max(convolution_image, image_rows, image_cols, &min, &max);
	
	normalized_image = normalize(convolution_image, image_rows, image_cols, NEWMIN, NEWMAX, min, max);

	save_image(normalized_image, "hawk_sobel_image.ppm", image_rows, image_cols);

	free(normalized_image);

	return convolution_image;
}

/* ACTIVE CONTOUR ALGORITHM APPLIED TO ORIGINAL IMAGE */
void active_contour(unsigned char *image, int image_rows, int image_cols, int **contour_rows, int **contour_cols, int arr_length)
{
	// Variable Declaration Section
	unsigned char *output_image;
	int i, j, k, rows, cols;
	int index = 0;
	int average_distance_x = 0;
	int average_distance_y = 0;
	int *first_window;
	int *second_window;

	
	// Allocate memory for image
	output_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));
	first_window = (int *)calloc(49, sizeof(int));
	second_window = (int *)calloc(49, sizeof(int));
	
	// Copy original image
	for (i = 0; i < (image_rows * image_cols); i++)
	{
		output_image[i] = image[i];
	}

	for (i = 0; i < arr_length; i++)
	{
		rows = (*contour_rows)[0];
		cols = (*contour_cols)[0];
		index = 0;

		// FIRST INTERNAL ENERGY CALCULATED 
		for (j = rows - 3; j <= (rows + 3); j++)
		{
			for (k = (cols - 3); k <= (cols + 3); k++)
			{
				if ((i + 1) < arr_length)
				{
					first_window[index] = SQUARE((k - (*contour_cols)[i + 1])) + SQUARE((j - (*contour_rows)[i + 1])); 
				}
				else
				{
					first_window[index] = SQUARE((k - (*contour_cols)[0])) + SQUARE((j - (*contour_rows)[0])); 
				}
				/*if (i == 0)
				{
					printf("%d ", first_window[index]);
				}*/
				index++;
			}
			/*
			if (i == 0)
			{
				printf("\n");
			}*/
		}

		// SECOND INTERNAL ENERGY
		if ((i + 1) < arr_length)
		{
			average_distance_x += cols;
			average_distance_y += rows;
		}
		//printf("\n");
	}

	average_distance_x /= arr_length;
	average_distance_y /= arr_length;
	printf("X: %d, Y: %d\n", average_distance_x, average_distance_y);

	free(first_window);
	free(second_window);
	free(output_image);
	
}

int main(int argc, char *argv[])
{
	// Variable Declaration Section
	FILE *image_file;
	int IMAGE_ROWS, IMAGE_COLS, IMAGE_BYTES;
	char file_header[MAXLENGTH];
	unsigned char *input_image;
	int *sobel_image;
	int *contour_rows, *contour_cols;		
	int file_size;

	/* CHECKS THAT USER ENTERED THE CORRECT NUMBER OF PARAMETERS */
	if (argc != 3)
	{
		printf("Usage: ./executable image_file.ppm initial_contour_file.txt");
		exit(1);
	}

	/* OPEN IMAGE */
	image_file = fopen(argv[1], "rb");
	if (image_file == NULL)
	{
		printf("Error, could not read input image\n");
		exit(1);
	}
	fscanf(image_file, "%s %d %d %d\n", file_header, &IMAGE_COLS, &IMAGE_ROWS, &IMAGE_BYTES);
	if ((strcmp(file_header, "P5") != 0) || (IMAGE_BYTES !=  255))
	{
		printf("Error, not a greyscale 8-bit PPM image\n");
		fclose(image_file);
		exit(1);
	}

	/* ALLOCATE MEMORY AND READ IN INPUT IMAGE */
	input_image = read_in_image(IMAGE_ROWS, IMAGE_COLS, image_file);

	/* EXTRACT INFORMATION FROM INITIAL CONTOUR TEXT FILE */
	read_initial_countour(argv[2], &contour_rows, &contour_cols, &file_size);
	
	/* DRAW INITIAL CONTOUR "+" ON INPUT IMAGE */
	draw_initial_contour(input_image, IMAGE_ROWS, IMAGE_COLS, &contour_rows, &contour_cols, file_size);
	
	/* UN-NORMALIZED SOBEL IMAGE */
	sobel_image = sobel_edge_detector(input_image, IMAGE_ROWS, IMAGE_COLS);
	
	/* CONTOUR ALGORITHM */
	active_contour(input_image, IMAGE_ROWS, IMAGE_COLS, &contour_rows, &contour_cols, file_size);
	
	/* FREE ALLOCATED MEMORY */
	free(input_image);
	free(sobel_image);
	free(contour_rows);
	free(contour_cols);
	
	return 0;
}
