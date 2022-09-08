// Phoenix.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Phantom.h"

#define MAX_LOADSTRING 100




#define winWidth 600
#define winHeight 400

#define bulletSize 4


#define playerSize 20

#define enemySize 20


#define maxEnemies 48
#define maxBullets 4
// Global Variables:

HINSTANCE hInst;                                // current instance
WCHAR titleString[MAX_LOADSTRING];                  // The title bar text
WCHAR windowClassName[MAX_LOADSTRING];            // the main window class name
bool framePassed;
unsigned int nextFrameStart;



// Program Variables:
//int shipX;
//int shipY;
int score;
int level;
bool running;
int health;

class Object {
public:
	RECT coord;
	int paramA;
};

struct keystate {
	bool keyS;
	bool keyW;
	bool up;
	bool down;
	bool keyA;
	bool keyD;
	bool left;
	bool right;
};
keystate STDKey;

Object ship;
Object enemies[maxEnemies];
int enemyCount;
Object bullets[maxBullets];
int bulletCount;

// Forward declarations of functions included in this code module:
int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int);
LRESULT CALLBACK   WindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
int setup(HWND);
int main(HWND);
int radar(HWND);
int guidance(HWND);
int trajectory(HWND);
int random(int, int);
int firing(HWND);
int APIENTRY wWinMain
(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	//UNREFERENCED_PARAMETER(hPrevInstance);
	//UNREFERENCED_PARAMETER(lpCmdLine);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, titleString, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_PHOENIX, windowClassName, MAX_LOADSTRING);


	WNDCLASSEXW wcex = {};
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PHOENIX);
	//stores the instance handle (used in creation of pop up windows)
	hInst = hInstance;
	//load the default arrow cursor
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//defines the background color (what for?)
	wcex.hbrBackground = (HBRUSH)0x0B;
	//load the window name from the resources
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDI_PHOENIX);
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	//give the window a icon
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PHOENIX));
	//pointer to application-definded function called the window procedure.
	wcex.lpfnWndProc = WindowProc;
	//handle to the application instance.
	wcex.hInstance = hInstance;
	//identifies the window class.
	wcex.lpszClassName = windowClassName;
	//EXW type windows class supports additional icon!
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PHOENIX));
	//registers the windows class.
	RegisterClassExW(&wcex);

	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowExW(
		//optional window style
		0,
		//window class
		windowClassName,
		//window title
		L"Phoenix",
		//window style
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
		//starting x, starting y, width, height
		CW_USEDEFAULT, CW_USEDEFAULT, winWidth + 16, winHeight + 59,
		//parent window
		NULL,
		//menu
		NULL,
		//Instance handle
		hInstance,
		//Additional application data
		NULL
	);
	//if window handle is unable to created (crash!!!)
	if (!hWnd)
		return -2;
	//draw the window.
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	setup(hWnd);
	//HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PHOENIX));

	MSG msg;

	// Main message loop:
	while (running) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		main(hWnd);
	}
	return 0;
}

int guidance
(HWND hWnd)
{
	return 0;
}
int firing
(HWND hWnd)
{
	if (bulletCount < maxBullets) {
		bullets[bulletCount].coord.left = ship.coord.left;
		bullets[bulletCount].coord.top = ship.coord.top + 10;
		bullets[bulletCount].coord.bottom = bullets[bulletCount].coord.top + bulletSize;
		bullets[bulletCount].coord.right = bullets[bulletCount].coord.left + bulletSize;
		bulletCount = bulletCount + 1;
	}
	return 0;
}
int trajectory
(HWND hWnd)
{

	for (int b = 0; b < bulletCount; ++b) {
		//InvalidateRect(hWnd, &bullets[b].coord, true);
		bullets[b].coord.left = bullets[b].coord.left + 8;
		bullets[b].coord.right = bullets[b].coord.left + bulletSize;
		if (bullets[b].coord.right > winWidth) {
			for (int c = b; c < maxBullets - 1; ++c)
				bullets[c] = bullets[c + 1];
			bulletCount = bulletCount - 1;
		}
		//InvalidateRect(hWnd, &bullets[b].coord, true);

	}
	return 0;
}
int radar
(HWND hWnd)
{
	for (int i = 0; i < enemyCount; ++i) {
		for (int b = 0; b < bulletCount; ++b) {
			if (bullets[b].coord.top > enemies[i].coord.top &&
				bullets[b].coord.bottom < enemies[i].coord.bottom &&
				bullets[b].coord.left > enemies[i].coord.left &&
				bullets[b].coord.right < enemies[i].coord.right) {
				for (int e = i; e < enemyCount - 1; ++e)
					enemies[e] = enemies[e + 1];
				for (int c = b; c < bulletCount - 1; ++c)
					bullets[c] = bullets[c + 1];
				bulletCount = bulletCount - 1;
				enemyCount = enemyCount - 1;
			}
		}
		enemies[i].coord.left = enemies[i].coord.left - 2;
		if (enemies[i].coord.left < 0)
			enemies[i].coord.left = winWidth;
		enemies[i].coord.right = enemies[i].coord.left + enemySize;
	}

	if (enemyCount == 0) {
		level = level + 1;
		for (int i = 0; i < (level < maxEnemies ? level : maxEnemies); ++i) {
			enemies[enemyCount].coord.top = random(10, winHeight);
			enemies[enemyCount].coord.bottom = enemies[enemyCount].coord.top + enemySize;
			enemies[enemyCount].coord.left = random(0, 100) + winWidth;
			enemies[enemyCount].coord.right = enemies[enemyCount].coord.left + enemySize;
			enemies[enemyCount].paramA = random(0, 4);
			enemyCount = enemyCount + 1;
		}
	}

	return 0;
}
int resetField
(HWND hWnd)
{
	ship.coord.left = 150;
	ship.coord.top = 125;
	ship.coord.bottom = ship.coord.top + 20;
	ship.coord.right = ship.coord.left + 20;
	ship.paramA = 0;
	bulletCount = 0;
	enemyCount = 0;
	for (int i = 0; i < maxBullets; ++i) {
		bullets[i].coord.top = 0;
		bullets[i].coord.bottom = 0;
		bullets[i].coord.left = 0;
		bullets[i].coord.right = 0;
		bullets[i].paramA = 0;
	}
	for (int i = 0; i < maxEnemies; ++i) {
		enemies[i].coord.top = 0;
		enemies[i].coord.bottom = 0;
		enemies[i].coord.left = 0;
		enemies[i].coord.right = 0;
		enemies[i].paramA = 0;
	}
	return 0;

}
void PaintProc
(PAINTSTRUCT ps, HDC hdc)
{
	//initialize a array of points to draw triangle

	FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(GRAY_BRUSH));
	//SelectObject(hdc, GetStockObject(WHITE_PEN));
	Rectangle(hdc, 0, 0, level * 10, 10);
	FrameRect(hdc, &ship.coord, (HBRUSH)GetStockObject(WHITE_BRUSH));
	//if (tick > -120) {
	SelectObject(hdc, GetStockObject(WHITE_BRUSH));
	//Polygon(hdc, shipArray, 4);
	//}
	POINT shapeCoord[5];
	for (int b = 0; b < bulletCount; ++b)
		FillRect(hdc, &bullets[b].coord, (HBRUSH)GetStockObject(WHITE_BRUSH));

	for (int e = 0; e < enemyCount; ++e) {

		switch (enemies[e].paramA) {
		case 0:
			shapeCoord[0].x = enemies[e].coord.left + 6;
			shapeCoord[0].y = enemies[e].coord.top;
			shapeCoord[1].x = enemies[e].coord.left - 8;
			shapeCoord[1].y = enemies[e].coord.top + 8;
			shapeCoord[2].x = enemies[e].coord.left + 6;
			shapeCoord[2].y = enemies[e].coord.top + 16;
			shapeCoord[3].x = enemies[e].coord.left + 6;
			shapeCoord[3].y = enemies[e].coord.top;
			Polyline(hdc, shapeCoord, 4);
			break;
		case 1:
			shapeCoord[0].x = enemies[e].coord.left;
			shapeCoord[0].y = enemies[e].coord.top;
			shapeCoord[2].x = enemies[e].coord.left + 2;
			shapeCoord[2].y = enemies[e].coord.top + 16;
			shapeCoord[1].x = enemies[e].coord.left + 11;
			shapeCoord[1].y = enemies[e].coord.top - 1;
			shapeCoord[3].x = enemies[e].coord.left + 9;
			shapeCoord[3].y = enemies[e].coord.top + 14;
			shapeCoord[4].x = enemies[e].coord.left;
			shapeCoord[4].y = enemies[e].coord.top;
			Polyline(hdc, shapeCoord, 5);
			break;
		case 2:
			FrameRect(hdc, &enemies[e].coord, (HBRUSH)GetStockObject(WHITE_BRUSH));
			break;
		case 3:
			Ellipse(hdc, enemies[e].coord.left, enemies[e].coord.top, enemies[e].coord.right, enemies[e].coord.bottom);
			break;
		}

	}

}

int random(
	int min,
	int max)
{
	int u = (int)((float)rand() / (RAND_MAX) * (max - min)) + min;
	return u;
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
		if (remaining > 1)
			Sleep(remaining - 1);
		return false;
	}
	//if none of them
	nextFrameStart = currentTick + rate;
	//lastFrameStart = currentTick;
	framePassed = true;
	return framePassed;
}

int setup
(HWND hWnd)
{
	running = true;
	resetField(hWnd);
	return 0;
}
int main
(HWND hWnd)
{
	if (nextFrame(30)) {
		InvalidateRgn(hWnd, NULL, TRUE);
		if (STDKey.keyW || STDKey.up) ship.coord.top = ship.coord.top - 4;
		if (STDKey.keyS || STDKey.down) ship.coord.top = ship.coord.top + 4;
		if (STDKey.keyA || STDKey.left) ship.coord.left = ship.coord.left - 4;
		if (STDKey.keyD || STDKey.right) ship.coord.left = ship.coord.left + 4;
		ship.coord.bottom = ship.coord.top + (20);
		ship.coord.right = ship.coord.left + (20);
		guidance(hWnd);
		trajectory(hWnd);
		radar(hWnd);
	}
	return 0;
}

//
//  FUNCTION:WindowProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WindowProc
(HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam)
{
	switch (uMsg)
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
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
	case WM_KEYDOWN:
		switch (wParam) {
		case 0x57:
			STDKey.keyW = true;
			break;

		case 0x53:
			STDKey.keyS = true;
			break;
		case 0x41:
			STDKey.keyA = true;
			break;
		case 0x44:
			STDKey.keyD = true;
			break;
		case VK_DOWN:
			STDKey.down = true;
			break;
		case VK_UP:
			STDKey.up = true;
			break;
		case VK_LEFT:
			STDKey.left = true;
			break;
		case VK_RIGHT:
			STDKey.right = true;
			break;
		case VK_SPACE:
			firing(hWnd);
			break;
		}
		return 0;
	case WM_KEYUP:
		switch (wParam) {
		case 0x57:
			STDKey.keyW = false;
			break;
		case 0x53:
			STDKey.keyS = false;
			break;
		case 0x41:
			STDKey.keyA = false;
			break;
		case 0x44:
			STDKey.keyD = false;
			break;
		case VK_DOWN:
			STDKey.down = false;
			break;
		case VK_UP:
			STDKey.up = false;
			break;
		case VK_LEFT:
			STDKey.left = false;
			break;
		case VK_RIGHT:
			STDKey.right = false;
			break;
		}
		return 0;

		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		PaintProc(ps, hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		running = false;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
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
