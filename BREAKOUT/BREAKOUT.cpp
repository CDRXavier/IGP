// BREAKOUT.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "BREAKOUT.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
constexpr int winWidth = 400;
constexpr int winHeight = 250;
constexpr int ballSize = 5;
constexpr int pLength = 40;
constexpr int ROWS = 15;
constexpr int COLUMNS = 5;
constexpr int brickWidth = 25;
constexpr int brickHeight = 10;
int score = 0;
//global variables
//int mouseX, mouseY;
//RECT square;
//int coord[2];
//bool keyState[8];
//int food[2];
bool running;
bool framePassed = false;
unsigned int nextFrameStart;
bool bounce;
int roundDotvx;
int roundDotvy;
RECT pedal;
RECT roundDot;
class BRICK {

public:
	bool collision;
	RECT coord;
};
BRICK bricks[ROWS][COLUMNS];

// Forward declarations of functions included in this code module:
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
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
		return 2;
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
}

int setup(HWND hWnd) {
	pedal.top = winHeight - 20;
	pedal.bottom = pedal.top + 10;
	pedal.left = (winWidth / 2) - 20;
	pedal.right = pedal.left + pLength;
	InvalidateRect(hWnd, &pedal, TRUE);
	roundDot.top = winHeight / 2;
	roundDot.bottom = roundDot.top + 10;
	roundDot.left = winWidth / 2;
	roundDot.right = roundDot.left + 10;
	InvalidateRect(hWnd, &roundDot, TRUE);
	roundDotvy = 8;
	running = true;
	for (int row = 0; row < ROWS; row++) {
		for (int column = 0; column < COLUMNS; column++) {
			bricks[row][column].collision = true;
			bricks[row][column].coord.left = 20 + brickWidth * row;
			bricks[row][column].coord.right = bricks[row][column].coord.left + brickWidth;
			bricks[row][column].coord.top = 20 + brickHeight * column;
			bricks[row][column].coord.bottom = bricks[row][column].coord.top + brickHeight;
		}
	}
	return 0;
}

int main(HWND hWnd) {
	if (nextFrame(20)) {
		InvalidateRect(hWnd, &roundDot, TRUE);
		roundDot.top = roundDot.top + roundDotvy;
		roundDot.left = roundDot.left + roundDotvx;
		roundDot.bottom = roundDot.top + ballSize;
		roundDot.right = roundDot.left + ballSize;
		InvalidateRect(hWnd, &roundDot, TRUE);

		if ((0x8000 & GetAsyncKeyState(VK_LEFT)) != 0) {
			InvalidateRect(hWnd, &pedal, TRUE);
			pedal.left = pedal.left - 8;
			pedal.right = pedal.left + pLength;
			InvalidateRect(hWnd, &pedal, TRUE);
		}
		if ((0x8000 & GetAsyncKeyState(VK_RIGHT)) != 0) {
			InvalidateRect(hWnd, &pedal, TRUE);
			pedal.left = pedal.left + 8;
			pedal.right = pedal.left + pLength;
			InvalidateRect(hWnd, &pedal, TRUE);
		}

		if (roundDot.right > pedal.left&& roundDot.left < pedal.right && roundDot.bottom > pedal.top) {
			roundDotvy = -roundDotvy;
			roundDotvx = roundDotvx - (float(roundDot.left) - float(pedal.left) - float(pLength) / 2) / 3.0f;
		}
		if (roundDotvx > 12) roundDotvx = roundDotvx = 12;
		if (roundDotvx < -12) roundDotvx = roundDotvx = -12;

		if (roundDot.left < 0) {
			roundDotvx = -roundDotvx;
			roundDot.left = 0;
			roundDot.right = roundDot.left + ballSize;
		}
		if (roundDot.right > winWidth) {
			roundDotvx = -roundDotvx;
			roundDot.right = winWidth;
			roundDot.left = roundDot.right - ballSize;
			for (int row = 0; row < ROWS; row++) {
				for (int column = 0; column < COLUMNS; column++) {
					InvalidateRect(hWnd, &bricks[row][column].coord, TRUE);
				}
			}
		}
		if (roundDot.top < 0) {
			roundDotvy = -roundDotvy;
			roundDot.top = 0;
			roundDot.bottom = roundDot.top + ballSize;
		}
		if (roundDot.bottom > winHeight) {
			roundDotvy = -roundDotvy;
			roundDot.bottom = winHeight;
			roundDot.top = roundDot.bottom - ballSize;
		}


		//Bounce off Bricks
		for (int row = 0; row < ROWS; row++)
			for (int column = 0; column < COLUMNS; column++)
				if (bricks[row][column].collision) {
					if (roundDot.right > bricks[row][column].coord.left&&
						roundDot.top < bricks[row][column].coord.bottom &&
						roundDot.left < bricks[row][column].coord.right &&
						roundDot.bottom > bricks[row][column].coord.top) {
						switch (row) {
						case 7: case 6: score = score + 1; break;
						case 5: case 4: score = score + 3; break;
						case 3: case 2: score = score + 5; break;
						case 1: case 0: score = score + 7; break;
						}
						bricks[row][column].collision = false;
						//Vertical collision
						if (roundDot.top < bricks[row][column].coord.bottom || roundDot.bottom > bricks[row][column].coord.top) {
							//Only bounce once each ball move
							if (bounce) {
								roundDotvy = -roundDotvy;
								bounce = false;

							}
						}
						//Hoizontal collision
						else if (roundDot.right > bricks[row][column].coord.left || roundDot.left < bricks[row][column].coord.right) {
							//Only bounce once brick each ball move
							if (bounce) {
								roundDotvx = -roundDotvx;
								bounce = false;
							}
						}
						InvalidateRect(hWnd, &bricks[row][column].coord, TRUE);
					}


				}
		
		bounce = true;
	}
	return 0;
}

constexpr bool isKeyDown(SHORT keyState)
{
	return ((keyState & 0x8000) != 0);
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
		case IDM_EXIT:
			DestroyWindow(hWnd);
			running = false;
			break;
		case IDM_CTRL:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CTRLBOX), hWnd, About);

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		HBRUSH brush = CreateSolidBrush(RGB(20, 20, 20));
		HBRUSH red = CreateSolidBrush(RGB(237, 28, 36));
		HBRUSH orange = CreateSolidBrush(RGB(225, 128, 64));
		HBRUSH yellow = CreateSolidBrush(RGB(225, 225, 32));
		HBRUSH green = CreateSolidBrush(RGB(32, 225, 32));
		HBRUSH blue = CreateSolidBrush(RGB(64, 128, 225));
		HBRUSH purple = CreateSolidBrush(RGB(196, 0, 225));
		HBRUSH indigo = CreateSolidBrush(RGB(128, 32, 225));
		HBRUSH white = CreateSolidBrush(RGB(196, 196, 196));
		FillRect(hdc, &ps.rcPaint, (HBRUSH)0x0B);
		//0x01 0x0B 0x0C light grey
		//0x02 0x0A black
		//0x03 light blue
		//0x04 light blue gray
		//0x05 very light grey
		//0x06 0x0D very very light grey
		//0x07 brown grey
		//0x08 black
		//0x09 black
		char valueeeee[6];
		//0x0E blue
		//0x0F white
		_itoa_s(172, valueeeee, 5, 10);
		DrawTextA(hdc, valueeeee, 5, &ps.rcPaint, DT_LEFT);
		FillRect(hdc, &pedal, brush);
		FillRect(hdc, &roundDot, brush);
		for (int row = 0; row < ROWS; row++) {
			for (int column = 0; column < COLUMNS; column++) {
				if (bricks[row][column].collision)
					switch (column) {
					case 1: //orange
						FillRect(hdc, &bricks[row][column].coord, orange);
						break;
					case 2: //yellow
						FillRect(hdc, &bricks[row][column].coord, yellow);
						break;
					case 3: //green
						FillRect(hdc, &bricks[row][column].coord, green);
						break;
					case 4: //blue
						FillRect(hdc, &bricks[row][column].coord, blue);
						break;
					case 5: //purple
						FillRect(hdc, &bricks[row][column].coord, purple);
						break;
					case 6: //indigo
						FillRect(hdc, &bricks[row][column].coord, indigo);
						break;
					case 7: //white
						FillRect(hdc, &bricks[row][column].coord, white);
						break;
					default: //red
						FillRect(hdc, &bricks[row][column].coord, red);
					}
			}
		}
		DeleteDC(hdc);
		DeleteObject(brush);
		DeleteObject(red);
		DeleteObject(orange);
		DeleteObject(yellow);
		DeleteObject(green);
		DeleteObject(blue);
		DeleteObject(purple);
		DeleteObject(indigo);
		DeleteObject(white);
		EndPaint(hWnd, &ps);
	}
	break;
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
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
