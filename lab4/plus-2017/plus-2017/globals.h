
#define SQR(x) ((x)*(x))	/* macro for square */
#ifndef M_PI			/* in case M_PI not found in math.h */
#define M_PI 3.1415927
#endif
#ifndef M_E
#define M_E 2.718282
#endif

#define MAX_FILENAME_CHARS	320

char	filename[MAX_FILENAME_CHARS];

HWND	MainWnd;

		// Display flags
int		ShowPixelCoords;
int play_mode;
int step_mode;

		// Image data
unsigned char	*OriginalImage;
int				ROWS, COLS;

#define TIMER_SECOND	1			/* ID of timer used for animation */
#define MAX_QUEUE 10000

		// Drawing flags
int		TimerRow,TimerCol;
int		ThreadRow,ThreadCol;
int		ThreadRunning;

int mouse_x_pos, mouse_y_pos;
int is_j_pressed;
int fill_thread_running;
int red, green, blue;
HWND temp;
CHOOSECOLOR color;
DWORD rgbCurrent;
COLORREF acrCustClr[16];
HBRUSH hbrush;
int end_thread;
char threshold[256];
char radius[256];
int thresh;
int rad;
int total_threads;
int *indices;

		// Function prototypes
LRESULT CALLBACK WndProc(HWND,UINT,WPARAM,LPARAM);
void PaintImage();
void AnimationThread(void *);		/* passes address of window */
void region_grow(void *); // Passes address of window