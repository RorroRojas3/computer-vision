#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLENGTH 256
#define EDGE 255
#define NOT_EDGE 0

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

/* THRESHOLD ORIGINAL IMAGE AT VALUE OF 128 */
unsigned char *original_image_threshold(unsigned char *original_image, int original_image_rows, int original_image_cols)
{
    // Variable Declaration Section
    int c1;
    int threshold = 128;
    unsigned char *output_threshold_image;

    // Allocate memory for temporary image
    output_threshold_image = (unsigned char *)calloc(original_image_rows * original_image_cols, sizeof(unsigned char));

    for (c1 = 0; c1 < (original_image_rows * original_image_cols); c1++)
    {
        if (original_image[c1] < threshold)
        {
            output_threshold_image[c1] = 255;
        }
        else
        {
            output_threshold_image[c1] = 0;
        }
    }
    
    return output_threshold_image;
}

int mark_pixel(unsigned char *image, int image_rows, int image_cols, int row, int col)
{
	// Variable Declaration Section
	int A, B, C, D;
	int c1;
	int index = 0;
	int index2 = 0;
	int edge_to_nonedge = 0;
	int edge_neighbors = 0;
    int current_pixel = 0;
    int next_pixel = 0;
    A = B = C = D = 0;

	// Up Row clockwise
	for (c1 = (col - 1); c1 <= col; c1++)
    {
        index = ((row - 1) * image_cols) + c1;
        current_pixel = image[index];
        index2 = ((row - 1) * image_cols) + (c1 + 1);
        next_pixel = image[index2];

        if ((current_pixel == EDGE) && (next_pixel == NOT_EDGE))
        {
            edge_to_nonedge++;
        }
        if ((current_pixel == NOT_EDGE) && (next_pixel == EDGE))
        {
            edge_to_nonedge++;
        }
        if (current_pixel == EDGE)
        {
            edge_neighbors++;
        }
    }

    // Right-Down Row Clock-wise
    for (c1 = (row - 1); c1 <= row; c1++)
    {
        index = (c1 * image_cols) + (col + 1);
        current_pixel = image[index];
        index2 = ((c1 + 1) * image_cols) + (col + 1);
        next_pixel = image[index2];

        if ((current_pixel == EDGE) && (next_pixel == NOT_EDGE))
        {
            edge_to_nonedge++;
        }
        if ((current_pixel == NOT_EDGE) && (next_pixel == EDGE))
        {
            edge_to_nonedge++;
        }
        if (current_pixel == EDGE)
        {
            edge_neighbors++;
        }
    }

    // Bottom-Left Row Clock-Wise
    for (c1 = (col + 1); c1 > (col - 1); c1--)
    {
        index = ((row + 1) * image_cols) + c1;
        current_pixel = image[index];
        index2 = ((row + 1) * image_cols) + (c1 - 1);
        next_pixel = image[index];

        if ((current_pixel == EDGE) && (next_pixel == NOT_EDGE))
        {
            edge_to_nonedge++;
        }
        if ((current_pixel == NOT_EDGE) && (next_pixel == EDGE))
        {
            edge_to_nonedge++;
        }
        if (current_pixel == EDGE)
        {
            edge_neighbors++;
        }
    }

    // Left-Up Row Clock-wise
    for (c1 = (row + 1); c1 > (row - 1); c1--)
    {
        index = (c1 * image_cols) + (col - 1);
        current_pixel = image[index];
        index2 = ((c1 - 1) * image_cols) + (col - 1);
        next_pixel = image[index2];

        if ((current_pixel == EDGE) && (next_pixel == NOT_EDGE))
        {
            edge_to_nonedge++;
        }
        if ((current_pixel == NOT_EDGE) && (next_pixel == EDGE))
        {
            edge_to_nonedge++;
        }
        if (current_pixel == EDGE)
        {
            edge_neighbors++;
        }
    }

    A = image[((row - 1) * image_cols) + col];
    B = image[(row * image_cols) + (col + 1)];
    C = image[(row * image_cols) + (col - 1)];
    D = image[((row + 1) * image_cols) + col];

    if (edge_to_nonedge == 1)
    {
        printf("HERE\n");
        if ((edge_neighbors >= 3) && (edge_neighbors <= 7))
        {
            if ((A == NOT_EDGE) || (B == NOT_EDGE) || ((C == NOT_EDGE) && (D == NOT_EDGE)))
            {
                return 1;
            }
        }
    }

	return 0;
}

void thinning(unsigned char *image, int image_rows, int image_cols)
{
	// Variable Declaration Section
	int c1, row, col;
	int is_pixel_marked = 0;
	int index = 0;
	int run_again  = 1;
	unsigned char *thined_image;
	unsigned char *temp_image;

	// Allocate memory for thined image
	thined_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));
	temp_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));

	// Copy original image to allocated thined image
	for (c1 = 0; c1 < (image_rows * image_cols); c1++)
	{
		thined_image[c1] = image[c1];
		temp_image[c1] = image[c1];
	}

	// Thinning Algorithm
	while(run_again == 1)
	{
        run_again = 0;
        is_pixel_marked = 0;

		for (row = 1; row < (image_rows - 1); row++)
		{
			for (col = 1; col < (image_cols - 1); col++)
			{
				is_pixel_marked = mark_pixel(thined_image, image_rows, image_cols, row, col);
                if (is_pixel_marked == 1)
				{
					index = (row * image_cols) + col;
					temp_image[index] = 0;
                    run_again = 1;
				}
			}
		}

        for (c1 = 0; c1 < (image_rows * image_cols); c1++)
		{
			thined_image[c1] = temp_image[c1];
		}
	}

    save_image(thined_image, "thined_image.ppm", image_rows, image_cols);
    
}

void roc(unsigned char *threshold_image, unsigned char *msf_image, int threshold_rows, int threshold_cols, int msf_rows, int msf_cols, char *file_name)
{
    // Variable Declaration Section
    FILE *file;
    int c1, c2;
    int rows, cols;
    int row1, col1;
    int tp, fp, fn, tn;
    int threshold;
    int index;
    int found;
    char current_character[2];
    char desired_character[2];
    unsigned char *temp_image;
    rows = cols = tp = fp = fn = tn = threshold = index = found = 0;
    
    strcpy(desired_character, "e");

    // Read in Ground Truth text file
    file = fopen(file_name, "r");
    if (file == NULL)
    {
        printf("Error, could not read Ground Truth text file\n");
        exit(1);
    }

    // Allocate memory for temporary image
    temp_image = (unsigned char *)calloc(msf_rows * msf_cols, sizeof(unsigned char));

    /* THRESHOLD IMAGE BASED ON THRESHOLD */
    for (c1 = 0; c1 < 256; c1 += 5)
    {
        threshold = c1;

        for (c2 = 0; c2 < (msf_rows * msf_cols); c2++)
        {
            if (msf_image[c2] >= threshold)
            {
                temp_image[c2] = 255;
            }
            else
            {
                temp_image[c2] = 0;
            }
        }
    }

    /* EXTRACT DATA REGARDING CHARACTER, COLUMNS, AND ROWS FROM GROUND TRUTH TEXT FILE */
    while((fscanf(file, "%s %d %d\n", current_character, &cols, &rows)) != EOF)
    {
        for (row1 = rows - 7; row1 <= (rows + 7); row1++)
        {
            for (col1 = cols - 4; col1 <= (cols + 4); col1++)
            {
                index = (row1 * msf_cols) + col1;
                if (temp_image[index] == 255)
                {
                    found = 1;
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    // Variable Declaration Section
    FILE *image_file, *template_file, *msf_file;
    int IMAGE_ROWS, IMAGE_COLS, IMAGE_BYTES, TEMPLATE_ROWS, TEMPLATE_COLS, TEMPLATE_BYTES, MSF_ROWS, MSF_COLS, MSF_BYTES;
    char file_header[MAXLENGTH];
    unsigned char *template_image;
    unsigned char *input_image;
    unsigned char *msf_image;
    unsigned char *original_image_with_threshold;

    /* CHECKT HAT THE USER ENTER THE CORRECT NUMBER OF PARAMETERS */
    if (argc != 5)
    {
        printf("Usage: ./executable image_file template_file ground_truth_file\n");
        exit(1);
    }

    /* OPEN IMAGE, TEMPLATE, AND MSF IMAGE FILES AND OBTEIN THEIR INFORMATION */
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

    template_file = fopen(argv[2], "rb");
    if (template_file == NULL)
    {
        fclose(image_file);
        printf("Error, could not read template image file\n");
        exit(1);
    }
    fscanf(template_file, "%s %d %d %d", file_header, &TEMPLATE_COLS, &TEMPLATE_ROWS, &TEMPLATE_BYTES);
    if ((strcmp(file_header, "P5") != 0) || (TEMPLATE_BYTES != 255))
    {
        fclose(image_file);
        fclose(template_file);
        printf("Error, not a greyscale 8-bit PPM image\n");
        exit(1);
    }

	msf_file = fopen(argv[4], "rb");
	if (msf_file == NULL)
	{
		fclose(image_file);
		fclose(template_file);
		printf("Error, could not read MSF image file\n");
		exit(1);
	}
	fscanf(msf_file, "%s %d %d %d", file_header, &MSF_COLS, &MSF_ROWS, &MSF_BYTES);
	if ((strcmp(file_header, "P5") != 0) || (MSF_BYTES != 255))
	{
		fclose(image_file);
		fclose(template_file);
		fclose(msf_file);
		printf("Error, not a greyscale 8-bit PPM image\n");
		exit(1);
	}

    /* ALLOCATE MEMORY AND READ IN INPUT AND TEMPLATE IMAGES */
    input_image = read_in_image(IMAGE_ROWS, IMAGE_COLS, file_header, image_file);
	template_image = read_in_image(TEMPLATE_ROWS, TEMPLATE_COLS, file_header, template_file);
	msf_image = read_in_image(MSF_ROWS, MSF_COLS, file_header, msf_file);

    /* THRESHOLD ORIGINAL IMAGE AT VALUE 128 */
    original_image_with_threshold = original_image_threshold(input_image, IMAGE_ROWS, IMAGE_COLS);
    thinning(original_image_with_threshold, IMAGE_ROWS, IMAGE_COLS);
    save_image(original_image_with_threshold, "original_128_threshold.ppm", IMAGE_ROWS, IMAGE_COLS);
    
    return 0;
}
