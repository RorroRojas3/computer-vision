	/*
	** Reads a binary image from the comand line, and creates
	** a segmentation of all 8-connected regions.
	** This version uses recursion, which is inefficient in terms
	** of memory and speed, and also causes a non-uniform outward
	** growth from the seed point.
	*/

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	unsigned char	*image;
	FILE		*fpt;
	char		header[80];
	int		ROWS,COLS,BYTES,r,c;
	int		RegionSize,*RegionPixels,TotalRegions;
	void		RecursePaintFill();

	if (argc != 2)
  	{
  		printf("Usage:  pf-recurse [image]\n");
  		exit(0);
  	}

	/* Allocate memory for images.  Read image (raw grey). */
	printf("Reading:  %s\n",argv[1]);
	if ((fpt=fopen(argv[1],"rb")) == NULL)
  	{
  		printf("Unable to open %s for reading\n",argv[1]);
  		exit(0);
  	}
	fscanf(fpt,"%s %d %d %d",header,&COLS,&ROWS,&BYTES);
	if (strcmp(header,"P5") != 0  ||  BYTES != 255)
  	{
  		printf("Must be a greyscale PPM 8-bit image\n");
  		exit(0);
  	}
	image=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char));
	RegionPixels=(int *)calloc(ROWS*COLS,sizeof(int));
	header[0]=fgetc(fpt);	/* whitespace character after header */
	fread(image,1,ROWS*COLS,fpt);
	fclose(fpt);

	/* check if image is binary (all pixels 0 or 255) */
	for (r=0; r<ROWS*COLS; r++)
	{
  		if (image[r] != 0  &&  image[r] != 255)
  		{
    		break;
    	}
		if (r < ROWS*COLS)
  		{
  			printf("Image is not binary (0/255)\n");
  			exit(0);
  		}
  }

	TotalRegions=0;
	for (r=0; r<ROWS; r++)
  	{
  		for (c=0; c<COLS; c++)
    	{
    		if (image[r*COLS+c] == 255)
      		{
      			TotalRegions++;
      			if (TotalRegions == 255)
        		{
        			printf("Segmentation incomplete.  Ran out of labels.\n");
        			break;
        		}
      			RegionSize=0;
      			RecursePaintFill(image,ROWS,COLS,r,c,255,TotalRegions,
				RegionPixels,&RegionSize);
      			printf("Region labeled %d is %d in size\n",TotalRegions,RegionSize);
      		}
    	}		
  		if (c < COLS)
  		{
    		break;	/* ran out of labels -- break both loops */
    	}
 	 }
 	 
	printf("%d total regions were found\n",TotalRegions);

	if ((fpt=fopen("seg.ppm","wb")) == NULL)
  	{
  		printf("Unable to open file for writing\n");
  		exit(0);
  	}
	fprintf(fpt,"P5 %d %d 255\n",COLS,ROWS);
	fwrite(image,1,ROWS*COLS,fpt);
	fclose(fpt);
}


	/*
	** Given an image, a starting point, and a label, this routine
	** paint-fills (8-connected) the area with the given new label.
	*/

void RecursePaintFill(unsigned char *image,	/* image data */
		int ROWS,int COLS,	/* size of image */
		int r,int c,		/* pixel to paint from */
		int paint_over_label,	/* image label to paint over */
		int new_label,		/* image label for painting */
		int *indices,		/* output:  indices of pixels painted */
		int *count)		/* output:  count of pixels painted */
{
	int	r2,c2;

	if (image[r*COLS+c] != paint_over_label)
	{
  		return;
	}
	image[r*COLS+c]=new_label;
	indices[*count]=(r*COLS+c);
	(*count)++;
	for (r2=-1; r2<=1; r2++)
	{
  		for (c2=-1; c2<=1; c2++)
    	{
    		if (r+r2 >= 0  &&  r+r2 < ROWS  &&  c+c2 >= 0  &&  c+c2 < COLS)
    		{
      			RecursePaintFill(image,ROWS,COLS,r+r2,c+c2,paint_over_label, new_label,indices,count);
			}
    	}
    }
}

