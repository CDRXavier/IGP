// Rain.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Rain.h"


//define glocal strings
constexpr int maxStringLength = 100;
WCHAR titleString[maxStringLength];
WCHAR windowClassName[maxStringLength];
#define winWidth 600
#define winHeight 400
#define dropSize 10
#define dropAmount 99
// Global Variables:
HINSTANCE hInst;
bool framePassed;
unsigned int nextFrameStart;
bool running;
RECT drops[dropAmount];

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool nextFrame(unsigned int);
int setup(HWND);
int main(HWND);


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR    pCmdLine,
	_In_ int       nCmdShow)
	//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	//initialize global strings.
	LoadStringW(hInstance, IDS_APP_TITLE, titleString, maxStringLength);
	LoadStringW(hInstance, IDC_RAIN, windowClassName, maxStringLength);


	//initialize HDC HERE
	//HDC hdc;
	//Register the window class name. (any value)
	//WCHAR CLASS_NAME = windowClassName;
	//zero terminated string. Don't know why.
	WNDCLASSEXW wcex = {};
	//window class is now a EXW (extra?)
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RAIN);
	hInst = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_RAIN);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_RAIN));

	//pointer to application-definded function called the window procedure.
	wcex.lpfnWndProc = WindowProc;
	//handle to the application instance.
	wcex.hInstance = hInstance;
	//identifies the window class.
	wcex.lpszClassName = windowClassName;
	//EXW type windows class supports additional icon!
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_RAIN));
	//registers the windows class. Similarly, we are using the ExW variation.
	RegisterClassExW(&wcex);

	// Create the window. Again, ExW window class.
	HWND hWnd = CreateWindowExW(
		0,															// Optional window styles.
		windowClassName,													// Window class
		L"Rain",													// Window title
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
		return -2;
	//draw the window.
	ShowWindow(hWnd, nCmdShow);
	//runs setup once
	setup(hWnd);

	//The message loop.
	MSG msg/* = {}*/;
	while (running) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0) {
			//if (!TranslateAccelerator(msg.hWnd, acceleratorTableHandle, &msg)) {
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

int setup(
	HWND hWnd)
{
	running = true;
	for (int i = 0; i < dropAmount; ++i) {
		drops[i].left = 6 * i;
		drops[i].top = rand() % 400;
		drops[i].bottom = drops[i].top + dropSize;
		drops[i].right = drops[i].left + dropSize;
		InvalidateRect(hWnd, &drops[i], true);
	}
	return 0;
}
int main(
	HWND hWnd)
{
	if (nextFrame(20)) {
		for (int i = 0; i < dropAmount; ++i) {
			InvalidateRect(hWnd, &drops[i], true);
			drops[i].top = drops[i].top + 10;
			drops[i].bottom = drops[i].top + dropSize;
			if (drops[i].top > 400) {
				drops[i].top = drops[i].top - 400;
				drops[i].bottom = drops[i].top + dropSize;
			}
			InvalidateRect(hWnd, &drops[i], true);
		}
	}
	return 0;
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
		//GetMenuItemInfo(HMENU, UINT, fByPosition, LPMENUITEMINFOW);
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
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
	return 0;
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
		FillRect(hdc, &ps.rcPaint, (HBRUSH)0x05);
		//all right. We need to do this trick here
		//first, we need to create the objects and store it in RAM
		//then we refer to these objects when we are going to use it
		//and then, af ter we are done with them, we DESTROY THEM!
		//no more handle leaks.
		//HBRUSH brush1 = CreateSolidBrush(RGB(75, 75, 75));
		HBRUSH dropBrush = CreateSolidBrush(RGB(0, 128, 225));
		HPEN outLiner= CreatePen(PS_SOLID, 1, RGB(0, 128, 255));
		SelectObject(hdc, dropBrush);
		SelectObject(hdc, outLiner);
		for (int i = 0; i < dropAmount; ++i)
			Ellipse(hdc, drops[i].left, drops[i].top, drops[i].right, drops[i].bottom);
			//FillRect(hdc, &drops[i], dropBrush);
		DeleteObject(outLiner);
		DeleteObject(dropBrush);
		DeleteDC(hdc);
		//DeleteObject(brush1);
		EndPaint(hWnd, &ps);
	}
				 return 0;
				 //case WM_LBUTTONUP:
	
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
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