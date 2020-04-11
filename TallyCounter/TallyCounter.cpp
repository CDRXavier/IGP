// TallyCounter.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "TallyCounter.h"
#include <string.h>




#define MAX_LOADSTRING 100
#define winHeight 300
#define winWidth 400

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR titleString[MAX_LOADSTRING];                  // The title bar text
WCHAR windowClassName[MAX_LOADSTRING];            // the main window class name
bool running;
unsigned int nextFrameStart;
unsigned int Number[4] = { 0,0,0,0 };
RECT Digit[4];

//function identifier
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int);
int main(HWND);
int IntStr(int anumber, const int);
int setup(HWND);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR    pCmdLine,
	_In_ int       nCmdShow)


	//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	//initialize global strings.
	LoadStringW(hInstance, IDS_APP_TITLE, titleString, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TALLYCOUNTER, windowClassName, MAX_LOADSTRING);


	//initialize HDC HERE
	//HDC hdc;
	//Register the window class name. (any value)
	//WCHAR CLASS_NAME = windowClassName;
	//zero terminated string. Don't know why.
	WNDCLASSEXW wcex = {};
	//window class is now a EXW (extra?)
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TALLYCOUNTER);
	hInst = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TALLYCOUNTER);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TALLYCOUNTER));

	//pointer to application-definded function called the window procedure.
	wcex.lpfnWndProc = WndProc;
	//handle to the application instance.
	wcex.hInstance = hInstance;
	//identifies the window class.
	wcex.lpszClassName = windowClassName;
	//EXW type windows class supports additional icon!
	wcex.hIconSm = wcex.hIcon;

	//registers the windows class. Similarly, we are using the ExW variation.
	RegisterClassExW(&wcex);

	// Create the window.
	//again, ExW window class.
	HWND hWnd = CreateWindowExW(
		0,															// Optional window styles.
		windowClassName,													// Window class
		L"TallyCoutner",													// Window title
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

	//we don't need a accelerator table.

	//setting variables
	setup(hWnd);

	//The message loop.
	MSG msg/* = {}*/;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if (msg.message == WM_QUIT)
			running = false;
		main(hWnd);
	}
	return 0;
}

int setup(
	HWND hWnd)
{
	SetRect(&Digit[0], 250, 100, 300, 200);
	SetRect(&Digit[1], 200, 100, 250, 200);
	SetRect(&Digit[2], 150, 100, 200, 200);
	SetRect(&Digit[3], 100, 100, 150, 200);

	running = true;
	return 0;
}

int main(
	HWND hWnd)
{
	if (Number[0] > 9) {
		Number[0] = 0;
		Number[1] = Number[1] + 1;
	}
	if (Number[1] > 9) {
		Number[1] = 0;
		Number[2] = Number[2] + 1;
	}
	if (Number[2] > 9) {
		Number[2] = 0;
		Number[3] = Number[3] + 1;
	}
	if (Number[3] > 9) {
		Number[3] = 0;
	}
	return 0;
}

/*
int IntTStr(int anumber, wchar_t buffer[]) {
	bool FP = true;
	int TMP = anumber;
	int Digit = 4;
	/*
	while (FP) {
		TMP = TMP / 10;
		if (TMP > 0)
			Digit = Digit + 1;
		else
			FP = false;
	}*//*
	//then create array
	//put in value
	if (Digit <= (sizeof(buffer) / 4))
		for (int i = 0; i < Digit - 1; ++i) {
			int temp = anumber;
			int TAMP = 10;
			//extract bits
			for (int d = 0; d < i; ++d)
				TAMP = TAMP * 10;
			temp = temp % TAMP;
			for (int d = 0; d < i - 1; ++d)
				temp = temp / 10;
			//ARR[Digit - 1 - i] = temp;
			switch (temp) {
			case 1:
				buffer[Digit - 1 - i] = '1';
				break;
			case 2:
				buffer[Digit - 1 - i] = '2';
				break;
			case 3:
				buffer[Digit - 1 - i] = '3';
				break;
			case 4:
				buffer[Digit - 1 - i] = '4';
				break;
			case 5:
				buffer[Digit - 1 - i] = '5';
				break;
			case 6:
				buffer[Digit - 1 - i] = '6';
				break;
			case 7:
				buffer[Digit - 1 - i] = '7';
				break;
			case 8:
				buffer[Digit - 1 - i] = '8';
				break;
			case 9:
				buffer[Digit - 1 - i] = '9';
				break;
			case 0:
				buffer[Digit - 1 - i] = '0';
				break;
			}
			//if (i = Digit - 1) buffer[Digit - i] = '\0';
		}
	return 0;
}
*/




//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu and returns from accelerator table
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
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
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
		FillRect(hdc, &ps.rcPaint, (HBRUSH)0x05);
		wchar_t num[2];
		HFONT hFont = CreateFont(26, 10, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_RASTER_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH, TEXT("Courier"));
		SelectObject(hdc, hFont);
		//Sets the coordinates for the rectangle in which the text is to be formatted.
		//colorref 0x00rrggbb
		
		SetBkColor(hdc, 0x00F0F0F0);
		DrawFocusRect(hdc, &Digit[0]);
		_ltow_s(Number[0], num, 2, 10);
		DrawText(hdc, num, -1, &Digit[0], DT_CENTER | DT_NOCLIP | DT_VCENTER | DT_SINGLELINE);

		DrawFocusRect(hdc, &Digit[1]);
		_ltow_s(Number[1], num, 2, 10);
		DrawText(hdc, num, -1, &Digit[1], DT_CENTER | DT_NOCLIP | DT_VCENTER | DT_SINGLELINE);

		DrawFocusRect(hdc, &Digit[2]);
		_ltow_s(Number[2], num, 2, 10);
		DrawText(hdc, num, -1, &Digit[2], DT_CENTER | DT_NOCLIP | DT_VCENTER | DT_SINGLELINE);

		DrawFocusRect(hdc, &Digit[3]);
		_ltow_s(Number[3], num, 2, 10);
		DrawText(hdc, num, -1, &Digit[3], DT_CENTER | DT_NOCLIP | DT_VCENTER | DT_SINGLELINE);
		
		DeleteObject(hFont);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_UP:
		case VK_SPACE:
			Number[0] = Number[0] + 1;
			break;
		case VK_RETURN:
			if (Number[0] <= min(min(Number[0], Number[1]), min(Number[2], Number[3]))) Number[0] = Number[0] + 1;
			if (Number[1] <= min(min(Number[0], Number[1]), min(Number[2], Number[3]))) Number[1] = Number[1] + 1;
			if (Number[2] <= min(min(Number[0], Number[1]), min(Number[2], Number[3]))) Number[2] = Number[2] + 1;
			if (Number[3] <= min(min(Number[0], Number[1]), min(Number[2], Number[3]))) Number[3] = Number[3] + 1;
			if (Number[0] > 9) Number[0] = 0;
			if (Number[1] > 9) Number[1] = 0;
			if (Number[2] > 9) Number[2] = 0;
			if (Number[3] > 9) Number[3] = 0;
		}
		InvalidateRect(hWnd, &Digit[0], TRUE);
		InvalidateRect(hWnd, &Digit[1], TRUE);
		InvalidateRect(hWnd, &Digit[2], TRUE);
		InvalidateRect(hWnd, &Digit[3], TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
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
