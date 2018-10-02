#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXLENGTH 256
#define EDGE 255
#define NOT_EDGE 0

/* READS IN IMAGE */
unsigned char *read_in_image(int rows, int cols, char file_header[], FILE *image_file)
{
	// Variable Declaration Section
	unsigned char *image;
	
	image = (unsigned char *)calloc(rows * cols, sizeof(unsigned char));
	
	//file_header[0] = fgetc(image_file);
	//printf("%s ", file_header);
	
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

/* DETERMINES IF CURRENT PIXEL OF IMAGE NEEDS TO BE ERASED/KEPT FROM THINNING IMAGE */
void get_transitions(unsigned char *image, int image_rows, int image_cols, int row, int col, int *mark_pixel, int *end_points, int *branch_points)
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
        next_pixel = image[index2];

        if ((current_pixel == EDGE) && (next_pixel == NOT_EDGE))
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
        if ((edge_neighbors >= 2) && (edge_neighbors <= 6))
        {
            if ((A == NOT_EDGE) || (B == NOT_EDGE) || ((C == NOT_EDGE) && (D == NOT_EDGE)))
            {
                *mark_pixel = 1;
            }
            else
            {
                *mark_pixel = 0;
            }
        }
        else
        {
            *mark_pixel = 0;
        }
    }
    else
    {
        *mark_pixel = 0;
    }

    if (edge_to_nonedge == 1)
    {
        *end_points = 1;
    }
    if (edge_to_nonedge > 2)
    {
        *branch_points = 2;
    }
}

/* THINNING OF ORIGINAL THRESHOLD IMAGE */
unsigned char *thinning(unsigned char *image, int image_rows, int image_cols)
{
	// Variable Declaration Section
	int c1, row, col;
	int is_pixel_marked = 0;
	int index = 0;
	int run_again  = 1;
    int end_points, branch_points;
	unsigned char *thinned_image;
	unsigned char *temp_image;
    int marked_pixels = 0;
    int ran_times = 0;
    end_points = branch_points = 0;

	// Allocate memory for thined image
	thinned_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));
	temp_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));

	// Copy original image to allocated thined image
	for (c1 = 0; c1 < (image_rows * image_cols); c1++)
	{
		thinned_image[c1] = image[c1];
		temp_image[c1] = image[c1];
	}

	// Thinning Algorithm
	while(run_again == 1)
	{
        run_again = 0;
        is_pixel_marked = 0;
        marked_pixels = 0;
        ran_times++;

		for (row = 1; row < (image_rows - 1); row++)
		{
			for (col = 1; col < (image_cols - 1); col++)
			{
                index = (row * image_cols) + col;
                if (thinned_image[index] == 255)
                {
                    is_pixel_marked = 0;
                    get_transitions(thinned_image, image_rows, image_cols, row, col, &is_pixel_marked, &end_points, &branch_points);
                    if (is_pixel_marked == 1)
                    {
                        index = (row * image_cols) + col;
                        temp_image[index] = 0;
                        run_again = 1;
                        marked_pixels += 1;
                    }
                }        
			}
		}

        printf("Pixels Marked on round %d: %d pixels\n", ran_times, marked_pixels);

        for (c1 = 0; c1 < (image_rows * image_cols); c1++)
		{
			thinned_image[c1] = temp_image[c1];
		}
	}
    return thinned_image;
}

/* RETURNS IMAGE DETAILING IF PIXEL IS END POINT OR BRANCH POINT */
unsigned char *get_end_and_branch_points(unsigned char *image, int image_rows, int image_cols)
{
    // Variable Declaration Section
    int row = 0;
    int c1 =0;
    int col = 0;
    int index = 0;
    int mark_pixel = 0;
    int end_points = 0;
    int branch_points = 0;
    int num_of_endp = 0;
    int num_of_branchp = 0;
    unsigned char *end_and_branch_point_image;
    unsigned char *thinned_with_points;

    // Allocate memory for image
    end_and_branch_point_image = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));
    thinned_with_points = (unsigned char *)calloc(image_rows * image_cols, sizeof(unsigned char));

    for (c1 = 0; c1 < (image_rows * image_cols); c1++)
    {
        thinned_with_points[c1] = image[c1];
    }

    for (row = 1; row < (image_rows - 1); row++)
    {
        for (col = 1; col < (image_cols - 1); col++)
        {
            index = (row * image_cols) + col;
            end_points = 0;
            branch_points = 0;
            if (image[index] == 255)
            {
                get_transitions(image, image_rows, image_cols, row, col, &mark_pixel, &end_points, &branch_points);
                if (end_points == 1)
                {
                    end_and_branch_point_image[index] = 50;
                    thinned_with_points[index] = 100;
                    num_of_endp += 1;
                }
                if (branch_points == 2)
                {
                    end_and_branch_point_image[index] = 150;
                    thinned_with_points[index] = 200;
                    num_of_branchp += 1;
                }
           }
        }
    }

    printf("Found Endpoints: %d | Found Brachpoints: %d\n", num_of_endp, num_of_branchp);

    save_image(end_and_branch_point_image, "endpoints and branchpoints.ppm", image_rows, image_cols);
    save_image(thinned_with_points, "thinned_with_points.ppm", image_rows, image_cols);

    return end_and_branch_point_image;
}

void roc(unsigned char *msf_image, unsigned char *end_and_branch_point_image, int msf_rows, int msf_cols, int end_rows, int end_cols, char *file_name)
{
    // Variable Declaration Section
    FILE *file, *csv_file;
    int c1, c2;
    int rows, cols;
    int row1, col1;
    int tp, fp, fn, tn;
    int threshold;
    int index;
    int found;
    int end_points = 0;
    int branch_points = 0;
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

    // Create CSV file and write the header
	csv_file = fopen("Truth Table.csv", "w");
    fprintf(csv_file, "Threshold,TP,FP,FN,TN,TPR,FPR,PPV\n");

    /* THRESHOLD IMAGE BASED ON THRESHOLD */
    for (c1 = 0; c1 <= 250; c1 += 5)
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
        
        /* EXTRACT DATA REGARDING CHARACTER, COLUMNS, AND ROWS FROM GROUND TRUTH TEXT FILE */
		while((fscanf(file, "%s %d %d\n", current_character, &cols, &rows)) != EOF)
		{
            end_points = 0;
            branch_points = 0;
            found = 0;
            //cols = cols - 1;

		    for (row1 = rows - 7; row1 <= (rows + 7); row1++)
		    {
		        for (col1 = cols - 4; col1 <= (cols + 4); col1++)
		        {
		            index = (row1 * msf_cols) + col1;
		            if (temp_image[index] == 255)
		            {
		                found = 1;
		            }
                    if (end_and_branch_point_image[index] == 50)
                    {
                        end_points += 1;
                    }
                    if (end_and_branch_point_image[index] == 150)
                    {
                        branch_points += 1;
                    }
		        }
		    }

   
            if ((found == 1) && (end_points == 1) && (branch_points == 1))
            {
                found = 1;
            }
            else
            {
                found = 0;
            }


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
    FILE *image_file, *msf_file; 
    int IMAGE_ROWS, IMAGE_COLS, IMAGE_BYTES, MSF_ROWS, MSF_COLS, MSF_BYTES;
    char file_header[MAXLENGTH];
   // unsigned char *template_image;
    unsigned char *input_image;
    unsigned char *msf_image;
    unsigned char *original_image_with_threshold;
    unsigned char *thinned_image;
    unsigned char *end_and_branch_points_image;

    /* CHECKT HAT THE USER ENTER THE CORRECT NUMBER OF PARAMETERS */
    if (argc != 4)
    {
        printf("Usage: ./executable image_file msf_file ground_truth_file\n");
        exit(1);
    }

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

    msf_file = fopen(argv[2], "rb");
    if (msf_file == NULL)
    {
        printf("Error, could not read MSF image\n");
        exit(1);
    }
    fscanf(msf_file, "%s %d %d %d\n", file_header, &MSF_COLS, &MSF_ROWS, &MSF_BYTES);
    if ((strcmp(file_header, "P5") != 0) || (MSF_BYTES != 255))
    {
        printf("Error, not a greyscale 8-bit PPM image\n");
        fclose(image_file);
        fclose(msf_file);
        exit(1);
    }

    /* ALLOCATE MEMORY AND READ IN INPUT IMAGE */
    input_image = read_in_image(IMAGE_ROWS, IMAGE_COLS, file_header, image_file);
    msf_image = read_in_image(MSF_ROWS, MSF_COLS, file_header, msf_file);

    /* THRESHOLD ORIGINAL IMAGE AT VALUE 128 */
    original_image_with_threshold = original_image_threshold(input_image, IMAGE_ROWS, IMAGE_COLS);

    /* THIN THRESHOLD IMAGE */
    thinned_image = thinning(original_image_with_threshold, IMAGE_ROWS, IMAGE_COLS);

    /* END AND BRANCHPOINT IMAGE */
    end_and_branch_points_image = get_end_and_branch_points(thinned_image, IMAGE_ROWS, IMAGE_COLS);
    
    /* CALCULATE ROC */
    roc(msf_image, end_and_branch_points_image, MSF_ROWS, MSF_COLS, IMAGE_ROWS, IMAGE_COLS, argv[3]);

    /* SAVE IMAGES */
    save_image(original_image_with_threshold, "threshold_at128.ppm", IMAGE_ROWS, IMAGE_COLS);
    save_image(thinned_image, "thinned_image.ppm", IMAGE_ROWS, IMAGE_COLS);
    
    return 0;
}
