// Library Declaration Section
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Definition Section
#define MAXLENGTH 256

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
	*contour_rows = (int **)calloc(*file_size, sizeof(int *));
	*contour_cols = (int **)calloc(*file_size, sizeof(int *));

	// Extracts the columns and rows for the initial countour file
	while((fscanf(file, "%d %d\n", &cols, &rows)) != EOF)
	{
		(*contour_cols)[i] = cols;
		(*contour_rows)[i] = rows;
		i++;
	}

	// Closes opened file
	fclose(file);
}

void draw_initial_contour(unsigned char *image, int image_rows, int image_cols)
{
	
}


int main(int argc, char *argv[])
{
	// Variable Declaration Section
	FILE *image_file;
	int IMAGE_ROWS, IMAGE_COLS, IMAGE_BYTES;
	char file_header[MAXLENGTH];
	unsigned char *input_image;
	int *contour_rows, *contour_cols;
	int i;
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

	return 0;
}