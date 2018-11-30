#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define MAXLENGTH 256
#define THRESHOLD 137

// RETURNS PPM IMAGE
unsigned char *read_in_image(int rows, int cols, FILE *image_file)
{
	// VARIABLE DECLARATION SECTION
	unsigned char *image;

	image = (unsigned char *)calloc(rows * cols, sizeof(unsigned char));

	fread(image, sizeof(unsigned char), rows * cols, image_file);

	fclose(image_file);

	return image;
}

// CREATE AND SAVE FILE AS PPM IMAGE 
void save_image(unsigned char *image, char *file_name, int rows, int cols)
{
    // VARIABLE DECLARATION SECTION
    FILE * file;
    file = fopen(file_name, "w");
    fprintf(file, "P5 %d %d 255\n", cols, rows);
    fwrite(image, rows * cols, sizeof(unsigned char), file);
    fclose(file);
}

// THRESHOLD IMAGE TO GET RID OF BACKGROUND 
unsigned char *threshold_image(int rows, int cols, unsigned char *image)
{
	// VARIABLE DECLARATION SECTION
	int i;
	unsigned char *output_image;

	// ALLOCATE MEMORY
	output_image = (unsigned char *)calloc(rows * cols, sizeof(unsigned char));

	// THRESHOLD IMAGE 
	for (i = 0; i < (rows * cols); i++)
	{
		if (image[i] < THRESHOLD)
		{
			output_image[i] = 255;
		}
	}

	save_image(output_image, "thresholded.ppm", rows, cols);

	return output_image;
}

// CALCULATES THE X,Y, AND Z COORDINATES OF IMAGE
void calc_points(unsigned char *image, unsigned char *threshold_image, int rows, int cols, double **X, double **Y, double **Z)
{
	// VARIABLE DECLARATION SECTION
	int i, j;
	double x_angle, y_angle, distance;
	double cp[7];
	double slant_correction;
	int index = 0;

	// ALLOCATE MEMORY
	*X = calloc(rows * cols, sizeof(double *));
	*Y = calloc(rows * cols, sizeof(double *));
	*Z = calloc(rows * cols, sizeof(double *));

	// COORDINATES ALGORITHM
	cp[0]=1220.7;		/* horizontal mirror angular velocity in rpm */
    cp[1]=32.0;		/* scan time per single pixel in microseconds */
    cp[2]=(cols/2)-0.5;		/* middle value of columns */
    cp[3]=1220.7/192.0;	/* vertical mirror angular velocity in rpm */
    cp[4]=6.14;		/* scan time (with retrace) per line in milliseconds */
    cp[5]=(rows/2)-0.5;		/* middle value of rows */
    cp[6]=10.0;		/* standoff distance in range units (3.66cm per r.u.) */

    cp[0]=cp[0]*3.1415927/30.0;	/* convert rpm to rad/sec */
    cp[3]=cp[3]*3.1415927/30.0;	/* convert rpm to rad/sec */
    cp[0]=2.0*cp[0];		/* beam ang. vel. is twice mirror ang. vel. */
    cp[3]=2.0*cp[3];		/* beam ang. vel. is twice mirror ang. vel. */
    cp[1]/=1000000.0;		/* units are microseconds : 10^-6 */
    cp[4]/=1000.0;			/* units are milliseconds : 10^-3 */

	for (i = 0; i < rows; i++)
	{
		for (j = 0; j < cols; j++)
		{
			slant_correction = cp[3] * cp[1] * ((double)j - cp[2]);
			x_angle = cp[0] * cp[1] * ((double)j - cp[2]);
			y_angle = (cp[3] * cp[4] * (cp[5] - (double)i)) + (slant_correction * -1);	/*  + slant correction */
			index = (i * cols) + j;
			distance = (double)image[index] + cp[6];
			(*Z)[index] = sqrt((distance * distance) / (1.0 + (tan(x_angle) * tan(x_angle)) + (tan(y_angle) *tan(y_angle))));
			(*X)[index] = tan(x_angle) * (*Z)[index];
			(*Y)[index] = tan(y_angle) * (*Z)[index];
		}
	}
}


int main(int argc, char *argv[])
{
	// VARIABLE DECLARATION SECTION
	FILE *image_file;
	int IMAGE_ROWS, IMAGE_COLS, IMAGE_BYTES;
	char file_header[MAXLENGTH];
	unsigned char *input_image, *thresholded_image;
	double *X, *Y, *Z;

	if (argc != 2)
	{
		printf("Usage: ./executable image_file\n");
		exit(1);
	}

	image_file = fopen(argv[1], "rb");
	if (image_file == NULL)
	{
		printf("Error, could not read PPM image file\n");
		exit(1);
	}
	fscanf(image_file, "%s %d %d %d\n", file_header, &IMAGE_COLS, &IMAGE_ROWS, &IMAGE_BYTES);
	if ((strcmp(file_header, "P5") != 0) || (IMAGE_BYTES != 255))
	{
		printf("Error, not a grey-scale 8-bit PPM image\n");
		fclose(image_file);
		exit(1);
	}

	/* ALLOCATES MEMORY AND READS IN INPUT IMAGE */
	input_image = read_in_image(IMAGE_ROWS, IMAGE_COLS, image_file);

	/* THRESHOLD IMAGE */
	thresholded_image = threshold_image(IMAGE_ROWS, IMAGE_COLS, input_image);

	/* CALCULATES 3D POINTS */
	calc_points(input_image, thresholded_image, IMAGE_ROWS, IMAGE_COLS, &X, &Y, &Z);

	printf("%lf %lf %lf\n", X[0], Y[0], Z[0]);
	printf("%lf %lf %lf\n", X[1], Y[1], Z[1]);


	return 0;
}
