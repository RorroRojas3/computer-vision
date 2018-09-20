/*
	** This program reads bridge.ppm, a 512 x 512 PPM image.
	** It smooths it using a standard 3x3 mean filter.
	** The program also demonstrates how to time a piece of code.
	**
	** To compile, must link using -lrt  (man clock_gettime() function).
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])

{
    FILE		*fpt;
    unsigned char	*image;
    unsigned char	*smoothed;
    char		header[320];
    int		ROWS,COLS,BYTES;
    int		r,c,r2,c2,sum;
    struct timespec	tp1,tp2;

	/* read image */
    if ((fpt=fopen("bridge.ppm","rb")) == NULL)
    {
		printf("Unable to open bridge.ppm for reading\n");
		exit(0);
    }
    
    fscanf(fpt,"%s %d %d %d\n",header,&COLS,&ROWS,&BYTES);
    
    if (strcmp(header,"P5") != 0  ||  BYTES != 255)
    {
		printf("Not a greyscale 8-bit PPM image\n");
		exit(0);
    }
    
    image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
    header[0]=fgetc(fpt);	/* read white-space character that separates header */
    fread(image,1,COLS*ROWS,fpt);
    fclose(fpt);

	/* allocate memory for smoothed version of image */
    smoothed=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));

        /* query timer */
    clock_gettime(CLOCK_REALTIME,&tp1);
    printf("%ld %ld\n",(long int)tp1.tv_sec,tp1.tv_nsec);

        /* smooth image, skipping the border points */
    for (r=1; r<ROWS-1; r++)
    {
    	for (c=1; c<COLS-1; c++)
        {
            sum=0;
            for (r2=-1; r2<=1; r2++)
            {
            	for (c2=-1; c2<=1; c2++)
            	{
                	sum+=image[(r+r2)*COLS+(c+c2)];
                }
            }
            smoothed[r*COLS+c]=sum/9;
        }
    }

        /* query timer */
    clock_gettime(CLOCK_REALTIME,&tp2);
    printf("%ld %ld\n",(long int)tp2.tv_sec,tp2.tv_nsec);

        /* report how long it took to smooth */
    printf("%ld\n",tp2.tv_nsec-tp1.tv_nsec);

        /* write out smoothed image to see result */
    fpt=fopen("smoothed.ppm","w");
    fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
    fwrite(smoothed,COLS*ROWS,1,fpt);
    fclose(fpt);
}



