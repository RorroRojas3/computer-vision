#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAXLENGTH 256
#define NEWMAX 255
#define NEWMIN 0

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

/* CALCULATE AND OBTAIN ZERO-MEAN TEMPLATE */
int *zero_mean(unsigned char *template_image, int template_rows, int template_cols)
{
	// Variable Declaration Section
	int *zero_mean_template;
	int sum = 0;
	int c1 = 0;
	int mean = 0;
	
	zero_mean_template = (int *)calloc(template_rows * template_cols, sizeof(int));
	
	for (c1 = 0; c1 < (template_rows * template_cols); c1++)
	{
		sum += template_image[c1];
	}

	// Caculate mean of sum as well as allocating memory for template array
	mean = sum / (template_rows * template_cols);

	// Calculate zero-mean centered by subtracting the mean
	for (c1 = 0; c1 < (template_rows * template_cols); c1++)
	{
		zero_mean_template[c1] = template_image[c1] - mean;
	}
	
	return zero_mean_template;
}

/* CONVOLUTION OF ZERO-MEAN TEMPLATE AND INPUT PICTURE */
int *convolution(unsigned char *input_image, int *zero_mean_template, int image_rows, int image_cols, int template_rows, int template_cols)
{
	// Variable Declaration Section
	int row1, row2, col1, col2, index, index2, sum;
	int *convolution_image;
	row1 = row2 = col1 = col2 = index = index2 = sum = 0;

	// Allocate memory for convolution image
	convolution_image = (int *)calloc(image_rows * image_cols, sizeof(int));
	
	for (row1 = 7; row1 < (image_rows - 7); row1++)
	{
		for (col1 = 4; col1 < (image_cols - 4); col1++)
		{
			sum = 0;
			for(row2 = -7; row2 < (template_rows - 7); row2++)
			{
				for (col2 = -4; col2 < (template_cols - 4); col2++)
				{
					index = (image_cols * (row1 + row2)) + (col1 + col2);
					index2 = (template_cols * (row2 + 7)) + (col2 + 4);
					sum += input_image[index] * zero_mean_template[index2];
				}
			}
			index = (image_cols * row1) + col1;
			convolution_image[index] = sum;
		}
	}
	
	return convolution_image;
}

void find_min_and_max(int *min, int *max, int *convolution_image, int image_rows, int image_cols)
{
	int c1;
	*min = convolution_image[0];
	*max = convolution_image[0];
	for (c1 = 1; c1 < (image_rows * image_cols); c1++)
	{
		if (*min > convolution_image[c1])
		{
			*min = convolution_image[c1];
		}
		if (*max < convolution_image[c1])
		{
			*max = convolution_image[c1];
		}
	}
}

/* NORMALIZE OUTPUT IMAGE */
unsigned char *normalize(int *convolution_image, int image_rows, int image_cols, int new_max, int new_min, int max, int min)
{
	// Variable Declaration Section
	unsigned char *normalized_image;
	int c1;
	
	// Allocate memory
	normalized_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));
	
	for (c1 = 0; c1 < (image_rows * image_cols); c1++)
	{
		normalized_image[c1] = ((convolution_image[c1] - min)*(NEWMAX - NEWMIN)/(max-min)) + NEWMIN;
	}
	
	return normalized_image;
}

/* TRUTH TABLE CALCULATION */
void roc(unsigned char *normalized_image, int image_rows, int image_cols, char *file_name)
{
	// Variable Declaration Section
	FILE *file, *csv_file;
	int c1 = 0;
	int c2 = 0;
	int rows = 0; 
	int cols = 0;
	int row1, col1;
	int tp, fp, fn, tn;
	int threshold = 0;
	int index = 0;
	int found = 0;
	char current_character[2];
	char desired_character[2];
	unsigned char *temp_image;
	tp = fp = fn = tn = 0;
	strcpy(desired_character, "e");

	// Read in ground truth file
	file = fopen(file_name, "r");
	if (file == NULL)
	{
		printf("Error, could not read file\n");
		exit(1);
	}

	// Allocate memory for temporary image
	temp_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));

	// Create CSV file and write the header
	csv_file = fopen("Truth Table.csv", "w");
    fprintf(csv_file, "Threshold,TP,FP,FN,TN,TPR,FPR,PPV\n");
    
	for (c1 = 0; c1 < 256; c1 += 5)
	{
		threshold = c1;

		for (c2 = 0; c2 < (image_rows * image_cols); c2++)
		{
			if (normalized_image[c2] >= threshold)
			{
				temp_image[c2] = 255;
			}
			else
			{
				temp_image[c2] = 0;
			}
		}

		/*
		if (threshold == 205)
		{
			csv_file=fopen("205thresh.ppm","w");
			fprintf(csv_file,"P5 %d %d 255\n",image_cols,image_rows);
			fwrite(temp_image,image_cols*image_rows,1,csv_file);
			fclose(csv_file);
		}
		*/
	
		// Read character, row, and columns of current line	
		while((fscanf(file, "%s %d %d\n", current_character, &cols, &rows)) != EOF)
		{
			
			for (row1 = rows-7; row1 <= (rows + 7); row1++)
			{
				for (col1 = cols-4; col1 <= (cols + 4); col1++)
				{
					index = (row1 * image_cols) + col1;
					if (temp_image[index] == 255)
					{
						found = 1;
					}
				}
			}
		
			// Find TP, FP, FN, and TN
			if ((found == 1) && (strcmp(current_character, desired_character) == 0))
			{
				tp++;
			}
			if ((found == 1) && (strcmp(current_character, desired_character) != 0))
			{
				fp++;
			}
			if ((found == 0) && (strcmp(current_character, desired_character) == 0))
			{
				fn++;
			}
			if ((found == 0) && (strcmp(current_character, desired_character) != 0))
			{
				tn++;
			}
			found = 0;
		}
		// Write values to CSV file
		fprintf(csv_file, "%d,%d,%d,%d,%d,%.2f,%.2f,%.2f\n", threshold, 
		tp, fp, fn, tn, tp/(double)(tp +fn ),fp/(double)(fp+tn), fp/(double)(tp+fp));
		tp = fp = fn = tn = 0;
		rewind(file);
	}
	fclose(file);
	fclose(csv_file);
}

int main(int argc, char *argv[])
{
	// Variable Declaration Section
	FILE *image_file, *template_file, *output_file;
	int IMAGE_ROWS, IMAGE_COLS, IMAGE_BYTES,TEMPLATE_ROWS, TEMPLATE_COLS, TEMPLATE_BYTES;
	char file_header[MAXLENGTH];
	unsigned char *template_image;
	unsigned char *input_image;
	unsigned char *normalized_image;
	int *zero_mean_template;
	int *convolution_image;
	int min, max;
	
	/* CHECK THAT USER ENTER THE CORRECT NUMBER OF PARAMETERS */
	if(argc != 4)
	{
		printf("Usage: ./executable image_file template_file ground_file\n");
		exit(1);
	}

	/* OPEN IMAGE AND TEMPLATE FILES AND OBTAIN THEIR INFORMATION */
	image_file = fopen(argv[1], "rb");
	if (image_file == NULL)
	{
		printf("Error, could not read image file\n");
		exit(1);
	}
	fscanf(image_file, "%s %d %d %d\n", file_header, &IMAGE_COLS, &IMAGE_ROWS, &IMAGE_BYTES);
	
    if ((strcmp(file_header, "P5") != 0) || (IMAGE_BYTES != 255))
    {
        printf("Error, not a greyscale 8-bit PPM image\n");
     	fclose(image_file);
        exit(1);
    }

	template_file = fopen(argv[2], "rb");
	if (template_file == NULL)
	{
		fclose(image_file);
		printf("Error, could not read template file\n");
		exit(1);
	}
	fscanf(template_file, "%s %d %d %d\n", file_header, &TEMPLATE_COLS, &TEMPLATE_ROWS, &TEMPLATE_BYTES);
    if ((strcmp(file_header, "P5") != 0) || (TEMPLATE_BYTES != 255))
    {
    	fclose(image_file);
    	fclose(template_file);
        printf("Error, not a greyscale 8-bit PPM image\n");
        exit(1);
    }

	/* ALLOCATE MEMORY AND READ IN INPUT AND TEMPLATE IMAGES */
	input_image = read_in_image(IMAGE_ROWS, IMAGE_COLS, file_header, image_file);
	template_image = read_in_image(TEMPLATE_ROWS, TEMPLATE_COLS, file_header, template_file);	
	
	/* CALCULATE AND OBTAIN ZERO-MEAN TEMPLATE */
	zero_mean_template = zero_mean(template_image, TEMPLATE_ROWS, TEMPLATE_COLS);
	
	/* CONVOLUTION OF ZERO-MEAN TEMPLATE AND INPUT PICTURE */
	convolution_image = convolution(input_image, zero_mean_template, IMAGE_ROWS, IMAGE_COLS, TEMPLATE_ROWS, TEMPLATE_COLS);

	/* CALCULATE MAX and MIN OF CONVOLUTED IMAGE */
	find_min_and_max(&min, &max, convolution_image, IMAGE_ROWS, IMAGE_COLS); 

	/* NORMALIZE OUTPUT IMAGE */
	normalized_image = normalize(convolution_image, IMAGE_ROWS, IMAGE_COLS, NEWMAX, NEWMIN, max, min);

	// Output Normalized Image
    output_file = fopen("normalized.ppm", "w");
    fprintf(output_file, "P5 %d %d 255\n", IMAGE_COLS, IMAGE_ROWS);
    fwrite(normalized_image, IMAGE_COLS * IMAGE_ROWS, sizeof(unsigned char), output_file);
    fclose(output_file);
	
	/* THRESHOLD OUTPUT IMAGE */
	roc(normalized_image, IMAGE_ROWS, IMAGE_COLS, argv[3]);

	return 0;
}
