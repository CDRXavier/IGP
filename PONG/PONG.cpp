// IGEP.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Resource.h"


//define glocal strings
constexpr int maxStringLength = 100;
WCHAR titleString[maxStringLength];
WCHAR windowClassName[maxStringLength];

//global variables
//int mouseX, mouseY;
//RECT square;
//int coord[2];
//bool keyState[8];
//int food[2];
bool running;
bool Demo;
bool SingleUser;
HBRUSH brush;
HINSTANCE hInst;
bool framePassed = false;
unsigned int nextFrameStart;
HMENU hmenu;

//function identifier

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Control(HWND, UINT, WPARAM, LPARAM);
int WINAPI wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int);
//don't worry about inconsistent annotation. It's custom-made, after all.
constexpr bool isKeyDown(SHORT);
int rangedRand(int, int);
int main(HWND);
int setup(HWND);
bool nextFrame(unsigned int);


RECT pPedal;
RECT cPedal;
RECT roundDot;
int roundDotvx;
int roundDotvy;

constexpr int winHeight = 300;
constexpr int winWidth = 400;
constexpr int pWidth = 10;
constexpr int pLength = 40;
constexpr int sqrSize = 20;
constexpr int ballSize = 10;

//entry point function
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR    pCmdLine,
	_In_ int       nCmdShow)


	//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	//initialize global strings.
	LoadStringW(hInstance, IDS_APP_TITLE, titleString, maxStringLength);
	LoadStringW(hInstance, IDC_PONG, windowClassName, maxStringLength);


	//initialize HDC HERE
	//HDC hdc;
	//Register the window class name. (any value)
	//WCHAR CLASS_NAME = windowClassName;
	//zero terminated string. Don't know why.
	WNDCLASSEXW wcex = {};
	//window class is now a EXW (extra?)
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PONG);
	hInst = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_PONG);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PONG));

	//pointer to application-definded function called the window procedure.
	wcex.lpfnWndProc = WindowProc;
	//handle to the application instance.
	wcex.hInstance = hInstance;
	//identifies the window class.
	wcex.lpszClassName = windowClassName;
	//EXW type windows class supports additional icon!
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_PONG));

	//registers the windows class. Similarly, we are using the ExW variation.
	RegisterClassExW(&wcex);

	// Create the window.
	//again, ExW window class.
	HWND hWnd = CreateWindowExW(
		0,															// Optional window styles.
		windowClassName,													// Window class
		L"PONG",													// Window title
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

	
	hmenu = GetMenu(hWnd);
	

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

int setup(HWND hWnd)
{
	//set the location of the square
	pPedal.left = 10;
	pPedal.top = 0;
	pPedal.bottom = pPedal.top + pLength;
	pPedal.right = pPedal.left + pWidth;
	cPedal.left = winWidth - 20;
	cPedal.top = 0;
	cPedal.bottom = cPedal.top + pLength;
	cPedal.right = cPedal.left + pWidth;
	roundDot.top = 150;
	roundDot.left = 200;
	roundDot.bottom = roundDot.top + ballSize;
	roundDot.right = roundDot.left + ballSize;
	roundDotvx = 8;
	roundDotvy = 0;
	Demo = false;
	SingleUser = true;
	//SetRect(&pPedal, coord[0], coord[1], pWidth + coord[0], pLength + coord[1]);
	//food[0] = 50;
	//food[1] = 50;
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
		InvalidateRect(hWnd, &roundDot, TRUE);
		roundDot.top = roundDot.top + roundDotvy;
		roundDot.left = roundDot.left + roundDotvx;
		roundDot.bottom = roundDot.top + ballSize;
		roundDot.right = roundDot.left + ballSize;
		InvalidateRect(hWnd, &roundDot, TRUE);
		if (isKeyDown(GetAsyncKeyState(0x57)) && (pPedal.top > 0) && !Demo) {
			InvalidateRect(hWnd, &pPedal, TRUE);
			pPedal.top = pPedal.top - 4;
			pPedal.bottom = pPedal.bottom - 4;
			InvalidateRect(hWnd, &pPedal, TRUE);
		}

		if (isKeyDown(GetAsyncKeyState(VK_UP)) && (cPedal.top > 0) && !SingleUser) {
			InvalidateRect(hWnd, &cPedal, TRUE);
			cPedal.top = cPedal.top - 4;
			cPedal.bottom = cPedal.bottom - 4;
			InvalidateRect(hWnd, &cPedal, TRUE);
		}
		if (Demo) {
			if (pPedal.top > roundDot.top) {
				InvalidateRect(hWnd, &pPedal, TRUE);
				pPedal.top = pPedal.top - 4;
				pPedal.bottom = pPedal.bottom - 4;
				InvalidateRect(hWnd, &pPedal, TRUE);
			}
			if (pPedal.bottom < roundDot.bottom) {
				InvalidateRect(hWnd, &pPedal, TRUE);
				pPedal.top = pPedal.top + 4;
				pPedal.bottom = pPedal.bottom + 4;
				InvalidateRect(hWnd, &pPedal, TRUE);
			}
		}
		
		//W
		//if ((isKeyDown(GetAsyncKeyState(0x41)) || isKeyDown(GetAsyncKeyState(VK_LEFT))) && (coord[0] > 0))
		//	coord[0] = coord[0] - 4;//A
		if (isKeyDown(GetAsyncKeyState(0x53)) && (pPedal.bottom < winHeight) && !Demo)
		{
			InvalidateRect(hWnd, &pPedal, TRUE);
			pPedal.top = pPedal.top + 4;
			pPedal.bottom = pPedal.bottom + 4;
			InvalidateRect(hWnd, &pPedal, TRUE);
		}
		if (isKeyDown(GetAsyncKeyState(VK_DOWN)) && (cPedal.bottom < winHeight) && !SingleUser)
		{
			InvalidateRect(hWnd, &cPedal, TRUE);
			cPedal.top = cPedal.top + 4;
			cPedal.bottom = cPedal.bottom + 4;
			InvalidateRect(hWnd, &cPedal, TRUE);
		}
		if (SingleUser) {
			if (cPedal.top > roundDot.top) {
				InvalidateRect(hWnd, &cPedal, TRUE);
				cPedal.top = cPedal.top - 4;
				cPedal.bottom = cPedal.bottom - 4;
				InvalidateRect(hWnd, &cPedal, TRUE);
			}
			if (cPedal.bottom < roundDot.bottom) {
				InvalidateRect(hWnd, &cPedal, TRUE);
				cPedal.top = cPedal.top + 4;
				cPedal.bottom = cPedal.bottom + 4;
				InvalidateRect(hWnd, &cPedal, TRUE);
			}
		}
		
		if (roundDot.left < 0) {
			roundDotvx = -roundDotvx;
			roundDot.left = 0;
			roundDot.right = roundDot.left + ballSize;
		}
		if (roundDot.right > winWidth) {
			roundDotvx = -roundDotvx;
			roundDot.right = winWidth;
			roundDot.left = roundDot.right - ballSize;
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
		if (roundDot.left < pPedal.right && roundDot.top < pPedal.bottom && roundDot.bottom > pPedal.top) {
			roundDotvx = -roundDotvx;
			roundDotvy = roundDotvy + float(roundDot.top - pPedal.top - pLength / 2) / 4;
		}
		if (roundDot.right > cPedal.left&& roundDot.top < cPedal.bottom && roundDot.bottom > cPedal.top) {
			roundDotvx = -roundDotvx;
			roundDotvy = roundDotvy + float(roundDot.top - cPedal.top - pLength / 2) / 4;
		}
		if (roundDotvy > 12) roundDotvy = roundDotvy = 12;
		if (roundDotvy < -12) roundDotvy = roundDotvy = -12;

		//S
	//if ((isKeyDown(GetAsyncKeyState(0x44)) || isKeyDown(GetAsyncKeyState(VK_RIGHT))) && (coord[0] < (winWidth - sqrSize)))
	//	coord[0] = coord[0] + 4;//D

		return 0;
	}
}

constexpr bool isKeyDown(
	SHORT keyState) {
	return ((keyState & 0x8000) != 0);
}

//int rangedRand(int min, int max)
//{
//	int u = (int)((double)rand() / (RAND_MAX + 1) * (max - min)) + min;
//	return u;
//}



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
		MENUITEMINFO menuDemo = { 0 };
		MENUITEMINFO menuOneP = { 0 };
		menuDemo.fMask = MIIM_STATE;
		menuOneP.fMask = MIIM_STATE;
		menuDemo.cbSize = sizeof(MENUITEMINFO);
		menuOneP.cbSize = sizeof(MENUITEMINFO);
		// Parse the menu selections:
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_CTRL:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CTRLBOX), hWnd, Control);
			break;
		case IDM_ONEPLAYER:
			GetMenuItemInfo(hmenu, IDM_ONEPLAYER, FALSE, &menuOneP);
			if (menuOneP.fState == MFS_CHECKED) {
				menuOneP.fState = MFS_UNCHECKED;
				SingleUser = false;
			}
			else {
				menuOneP.fState = MFS_CHECKED;
				SingleUser = true;
			}
			
			SetMenuItemInfo(hmenu, IDM_DEMO, FALSE, &menuDemo);
		break;
		case IDM_DEMO:
			GetMenuItemInfo(hmenu, IDM_DEMO, FALSE, &menuDemo);
			if (menuDemo.fState == MFS_CHECKED) {
				menuDemo.fState = MFS_UNCHECKED;
				Demo = false;
			}
			else {
				menuDemo.fState = MFS_CHECKED;
				Demo = true;
			}
			SetMenuItemInfo(hmenu, IDM_DEMO, FALSE, &menuDemo);
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
		FillRect(hdc, &ps.rcPaint, (HBRUSH)6);
		//all right. We need to do this trick here
		//first, we need to create the objects and store it in RAM
		//then we refer to these objects when we are going to use it
		//and then, af ter we are done with them, we DESTROY THEM!
		//no more handle leaks.
		HBRUSH brush1 = CreateSolidBrush(RGB(75, 75, 75));
		HBRUSH brush2 = CreateSolidBrush(RGB(0, 200, 200));
		FillRect(hdc, &pPedal, brush1);
		FillRect(hdc, &cPedal, brush1);
		FillRect(hdc, &roundDot, brush2);

		DeleteDC(hdc);
		DeleteObject(brush1);
		DeleteObject(brush2);

		EndPaint(hWnd, &ps);
		//these are not deleting the hdc properly. don't know what to say ...
	}
				 return 0;
				 //case WM_LBUTTONUP:
	case WM_MOUSEACTIVATE:
		return 0;
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