/*
	Name: Rodrigo Ignacio Rojas Garcia
	Lab#: 4
*/

// Library Declaration Section
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include <windows.h>
#include <wingdi.h>
#include <winuser.h>
#include <process.h>	
#include "resource.h"
#include "globals.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	MSG			msg;
	HWND		hWnd;
	WNDCLASS	wc;

	wc.style=CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc=(WNDPROC)WndProc;
	wc.cbClsExtra=0;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hIcon=LoadIcon(hInstance,"ID_PLUS_ICON");
	wc.hCursor=LoadCursor(NULL,IDC_ARROW);
	wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName="ID_MAIN_MENU";
	wc.lpszClassName="PLUS";

	if (!RegisterClass(&wc))
	{
		return(FALSE);
	}

	hWnd=CreateWindow("PLUS","plus program", WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL, CW_USEDEFAULT,0,400,400,NULL,NULL,hInstance,NULL);
	if (!hWnd)
	{
		return(FALSE);
	}

	ShowScrollBar(hWnd,SB_BOTH,FALSE);
	ShowWindow(hWnd,nCmdShow);
	UpdateWindow(hWnd);
	MainWnd=hWnd;

	ShowPixelCoords=0;
	play_mode = 0;
	step_mode = 0;

	strcpy(filename,"");
	OriginalImage=NULL;
	ROWS=COLS=0;

	InvalidateRect(hWnd,NULL,TRUE);
	UpdateWindow(hWnd);

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(msg.wParam);
}




LRESULT CALLBACK WndProc (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HMENU				hMenu;
	OPENFILENAME		ofn;
	FILE				*fpt;
	HDC					hDC;
	char				header[320],text[320];
	int					BYTES,xPos,yPos;

	switch (uMsg)
	{
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				/* DETERMINES IF USER SELECTED OPTION ON GUI*/
				case ID_SHOWPIXELCOORDS:
					ShowPixelCoords = (ShowPixelCoords + 1) % 2;
					PaintImage();
					break;
				case ID_PLAY_MODE:
					play_mode = (play_mode + 1) % 2;
					break;
				case ID_STEP_MODE:
					step_mode = (step_mode + 1) % 2;
					break;

			
				case ID_FILE_LOAD:
					if (OriginalImage != NULL)
					{
						free(OriginalImage);
						OriginalImage = NULL;
					}
					memset(&(ofn), 0, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.lpstrFile = filename;
					filename[0] = 0;
					ofn.nMaxFile = MAX_FILENAME_CHARS;
					ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
					ofn.lpstrFilter = "PPM files\0*.ppm\0All files\0*.*\0\0";
					if (!(GetOpenFileName(&ofn)) || filename[0] == '\0')
					{
						break;		/* user cancelled load */
					}
					if ((fpt = fopen(filename, "rb")) == NULL)
					{
						MessageBox(NULL, "Unable to open file", filename, MB_OK | MB_APPLMODAL);
						break;
					}
					fscanf(fpt, "%s %d %d %d", header, &COLS, &ROWS, &BYTES);
					if (strcmp(header, "P5") != 0 || BYTES != 255)
					{
						MessageBox(NULL, "Not a PPM (P5 greyscale) image", filename, MB_OK | MB_APPLMODAL);
						fclose(fpt);
						break;
					}
					OriginalImage = (unsigned char *)calloc(ROWS*COLS, 1);
					header[0] = fgetc(fpt);	/* whitespace character after header */
					fread(OriginalImage, 1, ROWS*COLS, fpt);
					fclose(fpt);
					SetWindowText(hWnd, filename);
					PaintImage();
					break;

				case ID_FILE_QUIT:
					DestroyWindow(hWnd);
					break;
			}
			break;

		case WM_SIZE:		  /* could be used to detect when window size changes */
			PaintImage();
			return(DefWindowProc(hWnd,uMsg,wParam,lParam));
			break;

		case WM_PAINT:
			PaintImage();
			return(DefWindowProc(hWnd,uMsg,wParam,lParam));
			break;

		case WM_LBUTTONDOWN:case WM_RBUTTONDOWN:
			return(DefWindowProc(hWnd,uMsg,wParam,lParam));
			break;

		case WM_MOUSEMOVE:
			if (ShowPixelCoords == 1)
			{
				  xPos=LOWORD(lParam);
				  yPos=HIWORD(lParam);
				  if (xPos >= 0  &&  xPos < COLS  &&  yPos >= 0  &&  yPos < ROWS)
				  {
						sprintf(text,"%d, %d => %d", xPos, yPos, OriginalImage[yPos*COLS+xPos]);
						hDC=GetDC(MainWnd);
						TextOut(hDC,0,0,text,strlen(text));		/* draw text on the window */
						SetPixel(hDC,xPos,yPos,RGB(255,0,0));	/* color the cursor position red */
						ReleaseDC(MainWnd,hDC);
				  }
			}
		return(DefWindowProc(hWnd,uMsg,wParam,lParam));
		break;
	  
		case WM_KEYDOWN:
			if (wParam == 's' || wParam == 'S')
			{
				PostMessage(MainWnd, WM_COMMAND, ID_SHOWPIXELCOORDS, 0);	  /* send message to self */
			}
			/* IF USER PRESSES J, IT WILL GROW THE REGION BY ONE PIXEL AT A TIME*/
			if (wParam == 'j' || wParam == 'J')
			{
				PostMessage(MainWnd, WM_COMMAND, ID_STEP_MODE, 0);
			}
			if ((TCHAR)wParam == '1')
			{
				TimerRow=TimerCol=0;
				SetTimer(MainWnd,TIMER_SECOND,10,NULL);	/* start up 10 ms timer */
			}
			if ((TCHAR)wParam == '2')
			{
				KillTimer(MainWnd,TIMER_SECOND);			/* halt timer, stopping generation of WM_TIME events */
				PaintImage();								/* redraw original image, erasing animation */
			}
			if ((TCHAR)wParam == '3')
			{
				ThreadRunning=1;
				_beginthread(AnimationThread,0,MainWnd);	/* start up a child thread to do other work while this thread continues GUI */
			}
 			if ((TCHAR)wParam == '4')
			{
				ThreadRunning=0;							/* this is used to stop the child thread (see its code below) */
			}
			return(DefWindowProc(hWnd,uMsg,wParam,lParam));
			break;

		case WM_TIMER:	  /* this event gets triggered every time the timer goes off */
			hDC=GetDC(MainWnd);
			SetPixel(hDC,TimerCol,TimerRow,RGB(0,0,255));	/* color the animation pixel blue */
			ReleaseDC(MainWnd,hDC);
			TimerRow++;
			TimerCol+=2;
			break;

		case WM_HSCROLL:	  /* this event could be used to change what part of the image to draw */
			PaintImage();	  /* direct PaintImage calls eliminate flicker; the alternative is InvalidateRect(hWnd,NULL,TRUE); UpdateWindow(hWnd); */
			return(DefWindowProc(hWnd,uMsg,wParam,lParam));
			break;

		case WM_VSCROLL:	  /* this event could be used to change what part of the image to draw */
			PaintImage();
			return(DefWindowProc(hWnd,uMsg,wParam,lParam));
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return(DefWindowProc(hWnd,uMsg,wParam,lParam));
			break;
	}

	hMenu=GetMenu(MainWnd);

	/* CHECKS AND UNCHECKS IF SELECTED OPTION ON GUI*/
	if (ShowPixelCoords == 1)
	{
		CheckMenuItem(hMenu, ID_SHOWPIXELCOORDS, MF_CHECKED);	/* you can also call EnableMenuItem() to grey(disable) an option */
	}
	else
	{
		CheckMenuItem(hMenu, ID_SHOWPIXELCOORDS, MF_UNCHECKED);
	}
	if (play_mode == 1)
	{
		CheckMenuItem(hMenu, ID_PLAY_MODE, MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hMenu, ID_PLAY_MODE,  MF_UNCHECKED);
	}
	if (step_mode == 1)
	{
		CheckMenuItem(hMenu, ID_STEP_MODE, MF_CHECKED);
	}
	else
	{
		CheckMenuItem(hMenu, ID_STEP_MODE, MF_UNCHECKED);
	}

	DrawMenuBar(hWnd);

	return(0L);
}




void PaintImage()

{
	PAINTSTRUCT			Painter;
	HDC					hDC;
	BITMAPINFOHEADER	bm_info_header;
	BITMAPINFO			*bm_info;
	int					i,r,c,DISPLAY_ROWS,DISPLAY_COLS;
	unsigned char		*DisplayImage;

	if (OriginalImage == NULL)
	{
		return;		/* no image to draw */
	}

	/* Windows pads to 4-byte boundaries.  We have to round the size up to 4 in each dimension, filling with black. */
	DISPLAY_ROWS = ROWS;
	DISPLAY_COLS = COLS;

	if (DISPLAY_ROWS % 4 != 0)
	{
		DISPLAY_ROWS = (DISPLAY_ROWS / 4 + 1) * 4;
	}
	if (DISPLAY_COLS % 4 != 0)
	{
		DISPLAY_COLS = (DISPLAY_COLS / 4 + 1) * 4;
	}

	DisplayImage = (unsigned char *)calloc(DISPLAY_ROWS*DISPLAY_COLS,1);
	
	for (r = 0; r < ROWS; r++)
	{
		for (c = 0; c < COLS; c++)
		{
			DisplayImage[r*DISPLAY_COLS + c] = OriginalImage[r*COLS + c];
		}
	}

	BeginPaint(MainWnd,&Painter);
	hDC=GetDC(MainWnd);
	bm_info_header.biSize=sizeof(BITMAPINFOHEADER); 
	bm_info_header.biWidth=DISPLAY_COLS;
	bm_info_header.biHeight=-DISPLAY_ROWS; 
	bm_info_header.biPlanes=1;
	bm_info_header.biBitCount=8; 
	bm_info_header.biCompression=BI_RGB; 
	bm_info_header.biSizeImage=0; 
	bm_info_header.biXPelsPerMeter=0; 
	bm_info_header.biYPelsPerMeter=0;
	bm_info_header.biClrUsed=256;
	bm_info_header.biClrImportant=256;
	bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
	bm_info->bmiHeader=bm_info_header;

	for (i=0; i<256; i++)
	{
		  bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
		  bm_info->bmiColors[i].rgbReserved=0;
	} 

	SetDIBitsToDevice(hDC,0,0,DISPLAY_COLS,DISPLAY_ROWS,0,0,0, /* first scan line */DISPLAY_ROWS, /* number of scan lines */DisplayImage,bm_info,DIB_RGB_COLORS);
	ReleaseDC(MainWnd,hDC);
	EndPaint(MainWnd,&Painter);

	free(DisplayImage);
	free(bm_info);
}


void AnimationThread(HWND AnimationWindowHandle)

{
	HDC		hDC;
	char	text[300];

	ThreadRow=ThreadCol=0;
	while (ThreadRunning == 1)
	{
		  hDC=GetDC(MainWnd);
		  SetPixel(hDC,ThreadCol,ThreadRow,RGB(0,255,0));	/* color the animation pixel green */
		  sprintf(text,"%d, %d",ThreadRow,ThreadCol);
		  TextOut(hDC,300,0,text,strlen(text));		/* draw text on the window */
		  ReleaseDC(MainWnd,hDC);
		  ThreadRow+=3;
		  ThreadCol++;
		  Sleep(100);		/* pause 100 ms */
	}
}

