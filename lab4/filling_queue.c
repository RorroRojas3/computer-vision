	/*
	** Reads a binary image from the comand line, and creates
	** a segmentation of all 8-connected regions.
	** This version uses a queue for PaintFill, which provides
	** growth uniformly outward from the seed point.
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
	void		QueuePaintFill();

	if (argc != 2)
	{
		printf("Usage:  pf-queue [image]\n");
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
      			QueuePaintFill(image,ROWS,COLS,r,c,255,TotalRegions,NULL,&RegionSize);
      			printf("Region labeled %d is %d in size\n",TotalRegions,RegionSize);
      		}
    	}
  		if (c < COLS)
   	 	break;	/* ran out of labels -- break both loops */
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

#define MAX_QUEUE 10000	/* max perimeter size (pixels) of border wavefront */

void QueuePaintFill(unsigned char *image,	/* image data */
		int ROWS,int COLS,	/* size of image */
		int r,int c,		/* pixel to paint from */
		int paint_over_label,	/* image label to paint over */
		int new_label,		/* image label for painting */
		int *indices,		/* output:  indices of pixels painted */
		int *count)		/* output:  count of pixels painted */
{
	int	r2,c2;
	int	queue[MAX_QUEUE],qh,qt;

	*count=0;
	if (image[r*COLS+c] != paint_over_label)
  		return;
	image[r*COLS+c]=new_label;
	if (indices != NULL)
  		indices[0]=r*COLS+c;
	queue[0]=r*COLS+c;
	qh=1;	/* queue head */
	qt=0;	/* queue tail */
	*count=1;
	while (qt != qh)
  	{	
  		for (r2=-1; r2<=1; r2++)
  		{
    		for (c2=-1; c2<=1; c2++)
      		{
      			if (r2 == 0  &&  c2 == 0)
        			continue;
      			if ((queue[qt]/COLS+r2) < 0  ||  (queue[qt]/COLS+r2) >= ROWS  ||
	  					(queue[qt]%COLS+c2) < 0  ||  (queue[qt]%COLS+c2) >= COLS)
        			continue;
      			if (image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2] != paint_over_label)
        			continue;
      			image[(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2]=new_label;
      			if (indices != NULL)
        			indices[*count]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      			(*count)++;
      			queue[qh]=(queue[qt]/COLS+r2)*COLS+queue[qt]%COLS+c2;
      			qh=(qh+1)%MAX_QUEUE;
      			if (qh == qt)
        		{
        			printf("Max queue size exceeded\n");
        			exit(0);
        		}
      		}
  			qt=(qt+1)%MAX_QUEUE;
  		}
  	}
}

