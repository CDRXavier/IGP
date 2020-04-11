// BREAKOUT.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "BREAKOUT.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // handle to current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
#define winWidth 520
#define winHeight 340
#define ballSize 5
#define COLUMNS 18
#define ROWS 6
#define brickWidth 26
#define brickHeight 10
#define brdBnd 26
//int mouseX, mouseY;
//RECT square;
//int coord[2];
//bool keyState[8];
//int food[2];
int score;
int pLength;
bool running;
bool framePassed = false;
unsigned int nextFrameStart;
bool bounce;
int roundDotvx;
int roundDotvy;
RECT pedal;
RECT roundDot;
RECT scoreBox;
RECT leftBrd;
RECT rightBrd;
RECT topBrd;
struct keystate {
	bool left;
	bool right;
};
keystate STDKey;
class BRICK {
public:
	bool collision;
	RECT coord;
};
BRICK bricks[COLUMNS][ROWS];

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(_In_ HWND, _In_opt_ UINT, _In_ WPARAM, _In_ LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int main(HWND);
int setup(HWND);
bool nextFrame(unsigned int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BREAKOUT, szWindowClass, MAX_LOADSTRING);

	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BREAKOUT));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BREAKOUT);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_BREAKOUT));
	RegisterClassExW(&wcex);
	// Create the window.
	//again, ExW window class.
	HWND hWnd = CreateWindowExW(
		0,															// Optional window styles.
		szWindowClass,													// Window class
		L"BREAKOUT",													// Window title
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),	// Window style
		CW_USEDEFAULT, CW_USEDEFAULT, winWidth + 16, winHeight + 59,
		//starting x, starting y, width, height 

		NULL,       // Parent window  
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (!hWnd)
	{
		return -2;
	}
	ShowWindow(hWnd, nCmdShow);
	setup(hWnd);
	MSG msg;
	// Main message loop:
	while (running) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		main(hWnd);
	}
	return 0;
}

int setup(HWND hWnd) {
	leftBrd.top = 0;
	leftBrd.bottom = winHeight;
	leftBrd.left = 0;
	leftBrd.right = brdBnd;
	rightBrd.top = 0;
	rightBrd.bottom = winHeight;
	rightBrd.left = winWidth - brdBnd;
	rightBrd.right = winWidth;
	topBrd.top = 0;
	topBrd.bottom = brdBnd;
	topBrd.left = 0;
	topBrd.right = winWidth;
	pLength = 40;
	pedal.top = winHeight - 20;
	pedal.bottom = pedal.top + 8;
	pedal.left = (winWidth / 2) - 20;
	pedal.right = pedal.left + pLength;
	InvalidateRect(hWnd, &pedal, TRUE);
	roundDot.top = winHeight / 2;
	roundDot.bottom = roundDot.top + 10;
	roundDot.left = winWidth / 2;
	roundDot.right = roundDot.left + 10;
	InvalidateRect(hWnd, &roundDot, TRUE);
	roundDotvy = 4;
	scoreBox.top = 0;
	scoreBox.bottom = 26;
	scoreBox.left = 200;
	scoreBox.right = 300;
	running = true;
	score = 0;

	for (int column = 0; column < COLUMNS; column++)
		for (int row = 0; row < ROWS; row++) {
			bricks[column][row].collision = true;
			bricks[column][row].coord.left = brdBnd + brickWidth * column;
			bricks[column][row].coord.right = bricks[column][row].coord.left + brickWidth;
			bricks[column][row].coord.top = brdBnd + brdBnd + brickHeight * row;
			bricks[column][row].coord.bottom = bricks[column][row].coord.top + brickHeight;
		}
	return 0;
}

int main(HWND hWnd) {
	if (nextFrame(30)) {
		InvalidateRect(hWnd, &roundDot, TRUE);
		roundDot.top = roundDot.top + roundDotvy;
		roundDot.left = roundDot.left + roundDotvx;
		roundDot.bottom = roundDot.top + ballSize;
		roundDot.right = roundDot.left + ballSize;
		if (STDKey.left) {
			InvalidateRect(hWnd, &pedal, TRUE);
			pedal.left = pedal.left - 8;
			pedal.right = pedal.left + pLength;
		}
		if (STDKey.right) {
			InvalidateRect(hWnd, &pedal, TRUE);
			pedal.left = pedal.left + 8;
			pedal.right = pedal.left + pLength;
			
		}

		if (roundDot.right > pedal.left&& roundDot.left < pedal.right && roundDot.bottom > pedal.top) {
			roundDotvy = -4;
			roundDotvx = roundDotvx + (int)(float(roundDot.left) - float(pedal.left) - float(pLength) / 2) / 4.0f;
			if (roundDotvx == 0) roundDotvx = (rand() > 0) ? 1 : -1;
		}
		if (roundDotvx > 8) roundDotvx = roundDotvx = 8;
		if (roundDotvx < -8) roundDotvx = roundDotvx = -8;

		if (roundDot.left < brdBnd)
			roundDotvx = -roundDotvx;
		
		if (roundDot.right > winWidth - brdBnd)
			roundDotvx = -roundDotvx;
		if (roundDot.top < 0) 
			roundDotvy = -roundDotvy;
		
		if (roundDot.bottom > winHeight) 
			roundDotvy = -roundDotvy;
		


		//Bounce off Bricks
		for (int column = 0; column < COLUMNS; column++)
			for (int row = 0; row < ROWS; row++)
				if (bricks[column][row].collision) {
					if (roundDot.right > bricks[column][row].coord.left&&
						roundDot.top < bricks[column][row].coord.bottom &&
						roundDot.left < bricks[column][row].coord.right &&
						roundDot.bottom > bricks[column][row].coord.top) {
						switch (row) {
						case 7: case 6: score = score + 1; break;
						case 5: case 4: score = score + 3; break;
						case 3: case 2: score = score + 5; break;
						case 1: case 0: score = score + 7; 
							pLength = 25;  break;
						}
						bricks[column][row].collision = false;
						//Vertical collision
						if (roundDot.top < bricks[column][row].coord.bottom || roundDot.bottom > bricks[column][row].coord.top) {
							//Only bounce once each ball move
							if (bounce) {
								roundDotvy = -roundDotvy;
								bounce = false;
							}
						}
						//Hoizontal collision
						if (roundDot.right > bricks[column][row].coord.left || roundDot.left < bricks[column][row].coord.right) {
							//Only bounce once brick each ball move
							if (bounce) {
								roundDotvx = -roundDotvx;
								bounce = false;
							}
						}
						InvalidateRect(hWnd, &bricks[column][row].coord, TRUE);
					}
				}
		InvalidateRect(hWnd, &roundDot, TRUE);
		InvalidateRect(hWnd, &pedal, TRUE);
		InvalidateRect(hWnd, &scoreBox, TRUE);
		bounce = true;
	}
	
	return 0;
}

bool nextFrame(unsigned int rate) {
	unsigned int currentTick = static_cast<unsigned int>(GetTickCount());
	//don't worry about it. We are just using it for calculating framerate.
	unsigned int remaining;
	rate = 1000 / rate;

	if (framePassed) {
		framePassed = false;
		return false;
	}
	if (currentTick < nextFrameStart) {
		remaining = nextFrameStart - currentTick;
		if (remaining > 5)
			Sleep(remaining - 5);
		return false;
	}
	//if none of them
	nextFrameStart = currentTick + rate;
	//lastFrameStart = currentTick;
	framePassed = true;
	return framePassed;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_CTRL:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CTRLBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			running = false;
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		//HFONT hFont = CreateFontA(16, 40, 0, 0, 800, false, false, false, ANSI_CHARSET, OUT_RASTER_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, 0);
		HBRUSH backg = CreateSolidBrush(0x00202020);
		HBRUSH red = CreateSolidBrush(RGB(200, 72, 72));
		HBRUSH orange = CreateSolidBrush(RGB(192, 108, 58));
		HBRUSH beige = CreateSolidBrush(RGB(180, 121, 48));
		HBRUSH yellow = CreateSolidBrush(RGB(164, 162, 41));
		HBRUSH green = CreateSolidBrush(RGB(72, 159, 72));
		HBRUSH blue = CreateSolidBrush(RGB(66, 72, 199));
		HBRUSH indigo = CreateSolidBrush(RGB(128, 32, 225));
		//HBRUSH white = CreateSolidBrush(RGB(196, 196, 196));
		FillRect(hdc, &ps.rcPaint, backg);
		FillRect(hdc, &topBrd, (HBRUSH)0x0B);
		FillRect(hdc, &leftBrd, (HBRUSH)0x0B);
		FillRect(hdc, &rightBrd, (HBRUSH)0x0B);
		//FillRect(hdc, &scoreBox, (HBRUSH)0x0B);
		//0x01 0x0B 0x0C light grey
		//0x02 0x0A black
		//0x03 light blue
		//0x04 light blue gray
		//0x05 very light grey
		//0x06 0x0D very very light grey
		//0x07 brown grey
		//0x08 black
		//0x09 black
		wchar_t valueeeee[4];
		//0x0E blue
		//0x0F white
		_itow_s(score, valueeeee, 4, 10);
		HFONT hFont = CreateFont(26, 10, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_RASTER_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, TEXT("Courier"));
		SelectObject(hdc, hFont);
		//Sets the coordinates for the rectangle in which the text is to be formatted.
		//colorref 0x00rrggbb
		
		SetBkColor(hdc, 0x00B4B4B4);
		SetTextColor(hdc, 0x00202020);
		DrawText(hdc, valueeeee, -1, &scoreBox, DT_RIGHT);
		//DrawTextA(hdc, valueeeee, -1, &scoreBox, DT_CENTER);
		FillRect(hdc, &pedal, (HBRUSH)0x0B);
		FillRect(hdc, &roundDot, (HBRUSH)0x0B);
		for (int column = 0; column < COLUMNS; column++)
			for (int row = 0; row < ROWS; row++)
				if (bricks[column][row].collision)
					switch (row) {
					case 1: //orange
						FillRect(hdc, &bricks[column][row].coord, orange);
						break;
					case 2: //yellow
						FillRect(hdc, &bricks[column][row].coord, beige);
						break;
					case 3: //green
						FillRect(hdc, &bricks[column][row].coord, yellow);
						break;
					case 4: //blue
						FillRect(hdc, &bricks[column][row].coord, green);
						break;
					case 5: //purple
						FillRect(hdc, &bricks[column][row].coord, blue);
						break;
					case 6: //indigo
						FillRect(hdc, &bricks[column][row].coord, indigo);
						break;
						//case 7: //white
							//FillRect(hdc, &bricks[column][row].coord, white);
							//break;
					default: //red
						FillRect(hdc, &bricks[column][row].coord, red);
					}


		DeleteDC(hdc);
		DeleteObject(backg);
		DeleteObject(red);
		DeleteObject(orange);
		DeleteObject(yellow);
		DeleteObject(green);
		DeleteObject(blue);
		DeleteObject(beige);
		DeleteObject(indigo);
		DeleteObject(hFont);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		switch (wParam) {
		case 0x41:
		case VK_LEFT:
			STDKey.left = true;
			break;
		case 0x44:
		case VK_RIGHT:
			STDKey.right = true;
			break;
		}
		return 0;
	case WM_KEYUP:
		switch (wParam) {
		case 0x41:
		case VK_LEFT:
			STDKey.left = false;
			break;
		case 0x44:
		case VK_RIGHT:
			STDKey.right = false;
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		running = false;
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
