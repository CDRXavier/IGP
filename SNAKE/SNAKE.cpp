// SNAKE.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "SNAKE.h"
//define glocal strings
constexpr int maxStringLength = 100;
WCHAR titleString[maxStringLength];
WCHAR windowClassName[maxStringLength];

//the windows shell
#include <windows.h>
//function pre-definition
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void resetGame();
#define maxLength 256
int rangedRand(int min, int max);
void createFood();
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Control(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInst;
//global variables
RECT food;
RECT snakeBody[maxLength];
int Direction;
int snakeLength;
constexpr int winWidth = 800;
constexpr int winHeight = 600;
constexpr int blockSize = 20;
//bool keystate[8];

//entry point function
int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow)
{

	/*
	// create a window class. (any value?)
	const wchar_t CLASS_NAME[] = L"game";
	WNDCLASS wc = {};
	hInst = hInstance;
	//pointer to application-definded function called the window procedure.
	wc.lpfnWndProc = WindowProc;
	//handle to the application instance.
	wc.hInstance = hInstance;
	//identifies the window class.
	wc.lpszClassName = CLASS_NAME;

	//register the window class.
	RegisterClass(&wc);
	*/

	//initialize global strings.
	LoadStringW(hInstance, IDS_APP_TITLE, titleString, maxStringLength);
	LoadStringW(hInstance, IDC_SNAKE, windowClassName, maxStringLength);


	//initialize HDC HERE
	//HDC hdc;
	//Register the window class name. (any value)
	//WCHAR CLASS_NAME = windowClassName;
	//zero terminated string. Don't know why.
	WNDCLASSEXW wcex = {};
	//window class is now a EXW (extra?)
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SNAKE);
	hInst = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SNAKE);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SNAKE));

	//pointer to application-definded function called the window procedure.
	wcex.lpfnWndProc = WindowProc;
	//handle to the application instance.
	wcex.hInstance = hInstance;
	//identifies the window class.
	wcex.lpszClassName = windowClassName;
	//EXW type windows class supports additional icon!
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SNAKE));
	//registers the windows class. Similarly, we are using the ExW variation.
	RegisterClassExW(&wcex);

	// Create the window.

	HWND hWnd = CreateWindowEx(
		0,                              // Optional window styles.
		windowClassName,                     // Window class
		L"WindowsProject",              // Window title
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),// Window style
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, winWidth + 16, winHeight + 39,
		//starting x, starting y, width, height

		NULL,       // Parent window  
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hWnd == NULL)
		return -2;

	//initialize the game
	resetGame();

	//draw the window.
	ShowWindow(hWnd, nCmdShow);

	// Run the message loop.
	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void createFood()
{
	bool check;

	do {
		check = false;
		food.left = rangedRand(0, 20) * blockSize;
		food.right = food.left + blockSize;
		food.top = rangedRand(0, 20) * blockSize;
		food.bottom = food.top + blockSize;
		for (int i = 0; i < snakeLength; i++) {
			if (food.left == snakeBody[i].left && food.top == snakeBody[i].top) {
				i = snakeLength;
				check = true;
			}
		}
	} while (check);
	if (snakeLength > 255) {
		food.left = -40;
		food.right = food.left + blockSize;
		food.top = -40;
		food.bottom = food.top + blockSize;
	}
}

void resetGame()
{
	snakeLength = 3;
	Direction = 3;
	createFood();
	snakeBody[0].left = blockSize;
	snakeBody[0].right = snakeBody[0].left + blockSize;
	snakeBody[0].top = blockSize;
	snakeBody[0].bottom = snakeBody[0].top + blockSize;
	snakeBody[1].left = blockSize * 2;
	snakeBody[1].right = snakeBody[1].left + blockSize;
	snakeBody[1].top = blockSize;
	snakeBody[1].bottom = snakeBody[1].top + blockSize;
	snakeBody[2].left = blockSize * 3;
	snakeBody[2].right = snakeBody[2].left + blockSize;
	snakeBody[2].top = blockSize;
	snakeBody[2].bottom = snakeBody[2].top + blockSize;
}

int rangedRand(int min, int max)
{
	int u = (int)((double)rand() / (RAND_MAX) * (max - min)) + min;
	return u;
}



//the DispatchMessage function will call this function
//the window procedure of the window that is the target of the message.
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
//HWND is the handle
//uMsg is the message code
//WPARAM and LPARAM are additional data to the message
//LRESULT is a int value that's a "response" to the message
{
	int wmId = LOWORD(wParam);
	switch (uMsg) {
		
	case WM_COMMAND:
		
		switch (wmId) {
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_CTRL:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CTRLBOX), hWnd, Control);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
			break;
		}
		break;
	case WM_CLOSE:
		//if (MessageBox(hWnd, L"Really quit?", L"My application", MB_OKCANCEL) == IDOK)
		DestroyWindow(hWnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_SIZE:
	case WM_PAINT:
	{
		//painting procedures
		//initialize PAINTSTRUCT here	
		PAINTSTRUCT ps;
		//create hardware drawing contact (or whatever they call it)
		HDC hdc = BeginPaint(hWnd, &ps);
		HBRUSH brush1 = CreateSolidBrush(RGB(100, 100, 100));
		HBRUSH brush2 = CreateSolidBrush(RGB(150, 150, 150));
		HBRUSH brush3 = CreateSolidBrush(RGB(0, 255, 0));
		//create a "brush" to paint stuff
		//the background
		FillRect(hdc, &ps.rcPaint, (HBRUSH)6);
		for (int i = 0; i < snakeLength; i++) {
			//set the dimension of the snake bodies
			//SetRect(&body, snakeBody[i][0], snakeBody[i][1], snakeBody[i][0] + blockSize, snakeBody[i][1] + blockSize);
			if (i < snakeLength - 1)
				FillRect(hdc, &snakeBody[i], brush1);
			else
				FillRect(hdc, &snakeBody[i], brush2);
			DrawFocusRect(hdc, &snakeBody[i]);

		}
		FillRect(hdc, &food, brush3);
		ReleaseDC(hWnd, hdc);
		DeleteObject(brush1);
		DeleteObject(brush2);
		DeleteObject(brush3);
		EndPaint(hWnd, &ps);
	}
	return 0;

	case WM_KEYDOWN:
		//key = ((int)(short)LOWORD(lParam));
		//we'll just ASSUME it's the right key. enough condition crap.
		//so you can't ASSUME it's right because it does nonsense
		for (int i = 0; i < snakeLength - 1; i++)
			InvalidateRect(hWnd, &snakeBody[i], TRUE);
		switch (wParam) {
		case 0x57: // w key
		case VK_UP:
			if (Direction != 2 && (snakeBody[snakeLength - 1].top > 0)) {
				Direction = 0;
				for (int i = 0; i < snakeLength - 1; i++)
					snakeBody[i] = snakeBody[i + 1];
				snakeBody[snakeLength - 1].top = snakeBody[snakeLength - 1].top - blockSize;
				snakeBody[snakeLength - 1].bottom = snakeBody[snakeLength - 1].bottom - blockSize;
			}
			break;
		case VK_LEFT:
		case 0x41: // a key
			if (Direction != 3 && (snakeBody[snakeLength - 1].left > 0)) {
				Direction = 1;
				for (int i = 0; i < snakeLength - 1; i++)
					snakeBody[i] = snakeBody[i + 1];
				snakeBody[snakeLength - 1].left = snakeBody[snakeLength - 1].left - blockSize;
				snakeBody[snakeLength - 1].right = snakeBody[snakeLength - 1].right - blockSize;
			}
			break;
		case VK_DOWN:
		case 0x53: // s key
			if (Direction != 0 && (snakeBody[snakeLength - 1].bottom < winHeight)) {
				Direction = 2;
				for (int i = 0; i < snakeLength - 1; i++)
					snakeBody[i] = snakeBody[i + 1];
				snakeBody[snakeLength - 1].top = snakeBody[snakeLength - 1].top + blockSize;
				snakeBody[snakeLength - 1].bottom = snakeBody[snakeLength - 1].bottom + blockSize;
			}
			break;
		case VK_RIGHT:
		case 0x44: // d key
			if (Direction != 1 && (snakeBody[snakeLength - 1].right < winWidth)) {
				Direction = 3;
				for (int i = 0; i < snakeLength - 1; i++)
					snakeBody[i] = snakeBody[i + 1];
				snakeBody[snakeLength - 1].left = snakeBody[snakeLength - 1].left + blockSize;
				snakeBody[snakeLength - 1].right = snakeBody[snakeLength - 1].right + blockSize;
			}
			break;
		}
		//check food collision
		for (int i = 0; i < snakeLength; ++i) {
			InvalidateRect(hWnd, &snakeBody[i], TRUE);
			if (food.left == snakeBody[i].left && food.top == snakeBody[i].top) {
				snakeBody[snakeLength].left = food.left;
				snakeBody[snakeLength].top = food.top;
				snakeBody[snakeLength].right = snakeBody[snakeLength].left + blockSize;
				snakeBody[snakeLength].bottom = snakeBody[snakeLength].top + blockSize;
				snakeLength = snakeLength + 1;
				InvalidateRect(hWnd, &food, TRUE);
				createFood();
				InvalidateRect(hWnd, &food, TRUE);
			}
		}
		return 0;
	}
	//default action toward messages
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}



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

INT_PTR CALLBACK Control(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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