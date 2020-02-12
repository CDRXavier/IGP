// IGEP.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "IGEP.h"

constexpr int winHeight = 200;
constexpr int winWidth = 200;
constexpr int sqrSize = 20;

//define glocal strings
constexpr int maxStringLength = 100;
WCHAR titleString[maxStringLength];
WCHAR windowClassName[maxStringLength];

//global variables
int mouseX, mouseY;
RECT square;
int coord[2];
//bool keyState[8];
int food[2];
RECT roundDot;
bool running;
HBRUSH brush;
HINSTANCE hInst;
bool framePassed = false;
unsigned int nextFrameStart;

//function identifier
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Control(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE,
	_In_opt_ HINSTANCE,
	_In_ PWSTR,
	_In_ int);
//don't worry about inconsistent annotation. It's custom-made, after all.
constexpr bool isKeyDown(SHORT);
int rangedRand(int, int);
int main(HWND);
int setup(HWND);
bool nextFrame(unsigned int);


//entry point function
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR pCmdLine,
	_In_ int nCmdShow)
{
	//initialize global strings.
	LoadStringW(hInstance, IDS_APP_TITLE, titleString, maxStringLength);
	LoadStringW(hInstance, IDC_IGEP, windowClassName, maxStringLength);

	//Register the window class name. (any value)
	//WCHAR CLASS_NAME = windowClassName;
	//zero terminated string. Don't know why.
	WNDCLASSEXW wcex = {};
	//window class is now a EXW (extra?)
	//wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_IGEP);
	hInst = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_IGEP);
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
//	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_IGEP));
	//pointer to application-definded function called the window procedure.
	wcex.lpfnWndProc = WindowProc;
	//handle to the application instance.
	wcex.hInstance = hInstance;
	//identifies the window class.
	wcex.lpszClassName = windowClassName;
	//EXW type windows class supports additional icon!
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_IGEP));
	//registers the windows class. Similarly, we are using the ExW variation.
	RegisterClassExW(&wcex);

	// Create the window.
	//again, ExW window class.
	HWND hWnd = CreateWindowExW(
		0,															// Optional window styles.
		windowClassName,													// Window class
		L"IGEP",													// Window title
		(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),	// Window style
		CW_USEDEFAULT, CW_USEDEFAULT, winWidth + 16, winHeight + 59,
		//starting x, starting y, width, height 

		NULL,       // Parent window  
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);
	//if window handle is unable to created (crash!!!)
	if (!hWnd)
		return 2;
	//draw the window.
	ShowWindow(hWnd, nCmdShow);

	//load accelerator table
//	HACCEL hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_IGEP));

	//setting variables
	setup(hWnd);

	//The message loop.
	MSG msg/* = {}*/;
	while (running) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0) {
			//if (!TranslateAccelerator(msg.hWnd, hAccel, &msg)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					running = false;
			//}
		}
		main(hWnd);
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



int setup(HWND hWnd) {
	//set the location of the square
	coord[0] = 20;
	coord[1] = 20;
	SetRect(&square, coord[0], coord[1], sqrSize + coord[0], sqrSize + coord[1]);
	food[0] = 50;
	food[1] = 50;
	//give the square a initial brush
	brush = CreateSolidBrush(RGB(75, 75, 75));
	running = true;
	//return to signal completion
	return 0;
}
int main(HWND hWnd)
{
	if (nextFrame(30)) {
		//the nextFrame workes well (in eliminating unnecessary CPU usage), but the higher the framerate the more CPU it uses (doesn't make no sense)
		//if pumping 60 fps, it will use about 10% of the CPU
		int oldCoord[2];
		oldCoord[0] = coord[0];
		oldCoord[1] = coord[1];

		if ((isKeyDown(GetKeyState(0x57)) || isKeyDown(GetKeyState(VK_UP))) && (coord[1] > 0))
			coord[1] = coord[1] - 4;//W
		if ((isKeyDown(GetKeyState(0x41)) || isKeyDown(GetKeyState(VK_LEFT))) && (coord[0] > 0))
			coord[0] = coord[0] - 4;//A
		if ((isKeyDown(GetKeyState(0x53)) || isKeyDown(GetKeyState(VK_DOWN))) && (coord[1] < (winHeight - sqrSize)))
			coord[1] = coord[1] + 4;//S
		if ((isKeyDown(GetKeyState(0x44)) || isKeyDown(GetKeyState(VK_RIGHT))) && (coord[0] < (winWidth - sqrSize)))
			coord[0] = coord[0] + 4;//D

		if ((oldCoord[0] != coord[0]) || (oldCoord[1] != coord[1]))
		{
			HWND hWnd = GetActiveWindow();
			InvalidateRect(hWnd, &square, TRUE);
			SetRect(&square, coord[0], coord[1], coord[0] + sqrSize, coord[1] + sqrSize);
			InvalidateRect(hWnd, &square, TRUE);
			if ((abs(coord[0] - food[0]) < 20) && (abs(coord[1] - food[1]) < sqrSize) &&
				(abs(coord[0] - food[0]) + abs(coord[1] - food[1]) < 1.3 * sqrSize)) {
				//food[1] = food[1] + 20;
				food[0] = 4 * rangedRand(0, (winWidth - sqrSize) / 4);
				food[1] = 4 * rangedRand(0, (winHeight - sqrSize) / 4);
				InvalidateRect(hWnd, &roundDot, TRUE);
			}
			SetRect(&roundDot, food[0], food[1], food[0] + sqrSize, food[1] + sqrSize);
			InvalidateRect(hWnd, &roundDot, TRUE);
		}
		return 0;
	}
}

constexpr bool isKeyDown(SHORT keyState)
{
	return ((keyState & 0x8000) != 0);
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
	switch (uMsg) {
		//from the menu selections, the WM_COMMAND is sent
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_CTRL:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CTRLBOX), hWnd, Control);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
				   break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		running = false;
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT: {
		//painting procedures
		//initialize PAINTSTRUCT here
		PAINTSTRUCT ps;
		//Handle Device Context
		HDC hdc = BeginPaint(hWnd, &ps);
		//create a "brush" to paint stuff
		//the background
		FillRect(hdc, &ps.rcPaint, (HBRUSH)6);
		//the square
		FillRect(hdc, &square, brush);
		//the food
		Ellipse(hdc, food[0], food[1], food[0] + sqrSize, food[1] + sqrSize);
		EndPaint(hWnd, &ps);
	}
				 return 0;
	case WM_LBUTTONUP:
		//case WM_MOUSEACTIVATE:
	case WM_MOUSEMOVE:
		mouseX = ((int)(short)LOWORD(lParam));
		mouseY = ((int)(short)HIWORD(lParam));
		//tell the system that the rectangle here need to be redrawn

		//compare mouse coordinate with the boarder of the square
		if (mouseX > coord[0] && mouseX < (sqrSize + coord[0]) && mouseY > coord[1] && mouseY < (sqrSize + coord[1])) {
			brush = CreateSolidBrush(RGB(75, 75, 75));
			InvalidateRect(hWnd, &square, TRUE);
		}
		else {
			brush = CreateSolidBrush(RGB(100, 100, 100));
			InvalidateRect(hWnd, &square, TRUE);
		}
		return 0;

	case WM_LBUTTONDOWN:
		mouseX = ((int)(short)LOWORD(lParam));
		mouseY = ((int)(short)HIWORD(lParam));

		if (mouseX > coord[0] && mouseX < (sqrSize + coord[0]) && mouseY > coord[1] && mouseY < (sqrSize + coord[1])) {
			InvalidateRect(hWnd, &roundDot, TRUE);
			food[0] = 4 * rangedRand(0, (winWidth - sqrSize) / 4);
			food[1] = 4 * rangedRand(0, (winHeight - sqrSize) / 4);
			SetRect(&roundDot, food[0], food[1], food[0] + sqrSize, food[1] + sqrSize);
			InvalidateRect(hWnd, &roundDot, TRUE);
			InvalidateRect(hWnd, &square, TRUE);
			brush = CreateSolidBrush(RGB(0, 0, 0));
		}
		return 0;
	}
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