/*	Name: Rodrigo Ignacio Rojas Garcia
	Lab #7
*/

// Library Declaration Section
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

// Define Declaration Section
#define MAXLENGTH 256
#define ACC_THRESHOLD 128
#define GYRO_THRESHOLD 128
#define WINDOW_SIZE 50

void read_text_file(char *file_name, int *file_size, double **time, double **accX, double **accY,
					double **accZ, double **pitch, double **roll, double **yaw)
{
	// VARIABLE DECLARATION SECTION
	FILE *file;
	int i = 0;
	double d1, d2, d3, d4, d5, d6, d7;
	char c;
	char line[MAXLENGTH];
	*file_size = 0;
 
	// OBTAINS FILE LENGTH AND REWINDS IT TO BEGINNING
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

	fgets(line, sizeof(line), file);

	// ALLOCATES MEMORY
	*time = calloc(*file_size, sizeof(double *));
	*accX = calloc(*file_size, sizeof(double *));
	*accY = calloc(*file_size, sizeof(double *));
	*accZ = calloc(*file_size, sizeof(double *));
	*pitch = calloc(*file_size, sizeof(double *));
	*roll = calloc(*file_size, sizeof(double *));
	*yaw = calloc(*file_size, sizeof(double *));
	

	// EXTRACTS TDATA FROM TEXT FILE	
	while((fscanf(file, "%lf %lf %lf %lf %lf %lf %lf\n", &d1, &d2, &d3, &d4, &d5, &d6, &d7)) != EOF)
	{
		(*time)[i] = d1;
		(*accX)[i] = d2;
		(*accY)[i] = d3;
		(*accZ)[i] = d4;
		(*pitch)[i] = d5;
		(*roll)[i] = d6;
		(*yaw)[i] = d7;
		i++;
	}
	fclose(file);

	// CREATE CSV FILE
	file = fopen("data-csv.csv", "w");
	fprintf(file, "Time[s],Acceletarion-X[m/s],Acceletarion-Y[m/s],Acceleration-Z[m/s],Pitch,Roll,Yaw\n");
	for (i = 0; i < *file_size; i++)
	{
		fprintf(file, "%lf,%lf,%lf,%lf,%lf,%lf,%lf\n", (*time)[i], (*accX)[i], (*accY)[i], (*accZ)[i], (*pitch)[i], (*roll)[i], (*yaw)[i]);
	}
	fclose(file);
}

// SMOOTH DATA POINTS
void smooth_data(int *arr_length, double **data, double **smoothed_data)
{
	// VARIABLE DECLARATION SECTION
	int i, j;
	double sum;
	
	*smoothed_data = calloc(*arr_length, sizeof(double *));

	for (i = 0; i < WINDOW_SIZE; i++)
	{
		(*smoothed_data)[i] = (*data)[i];
	}

	for (i = WINDOW_SIZE - 1; i < *arr_length; i++)
	{
		sum = 0;
		for (j = 1; j < WINDOW_SIZE; j++)
		{
			sum += (*data)[i - j];
		}
		(*smoothed_data)[i] = (sum + (*data)[i]) / WINDOW_SIZE;
	}

}

// CALCULATE VARIANCE
double calc_variance(double **data, int *arr_length, int index)
{
	// VARIABLE DECLARATION SECTION
	int i;
	int window = 0;
	double mean = 0;
	double variance = 0;

	// HANDLES ERROR WHEN WINDOW SIZE IS TOO BIG 
	if (index + WINDOW_SIZE < *arr_length)
	{
		window = WINDOW_SIZE;
	}
	else
	{
		window = abs(*arr_length - index);
	}

	// CALCULATE THE SUM OF DATA SET
	for (i = index; i < (index + window); i++)
	{
		variance += (*data)[i];
	}
	
	// USED SUM CALCULATED TO OBTAIN MEAN OF DATA SET
	mean = variance / window;
	variance = 0;

	// SUBTRACT MEAN FOR THE DATA SET AND SUM RESULTS
	for (i = index; i < (index + window); i++)
	{
		variance += pow((*data)[i] - mean, 2);
	}
	
	// CALCULATE VARIANCE OF EACH DATA SET 
	variance = variance / (window - 1);

	return variance;
}

int main(int argc, char *argv[])
{
	/* VARIABLE DECLARATION SECTION */
	int file_size;
	int i, j;
	double *time, *accX, *accY, *accZ, *pitch, *roll, *yaw;
	double variance;
	double *smooth_accX, *smooth_accY, *smooth_accZ, *smooth_pitch, *smooth_roll, *smooth_yaw;

	if (argc != 2)
	{
		printf("Usage: ./executable text_file.txt\n");
		exit(1);
	}

	/* EXTRACT DATA FROM TEXT FILE */ 
	read_text_file(argv[1], &file_size, &time, &accX, &accY, &accZ, &pitch, &roll, &yaw);

	/* SMOOTH EXTRACTED DATA */
	smooth_data(&file_size, &accX, &smooth_accX);

	/* CALCULATE VARIANCE */
	j = 0;
	for (i = 0; i < 20; i++)
	{
		variance = calc_variance(&smooth_accX, &file_size, j);
		j += WINDOW_SIZE;
		printf("%lf\n", variance);
	}

	return 0;
}
