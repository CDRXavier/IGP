// Blink.cpp : Defines the entrqwertyuioppoint for the application.
//

#include "framework.h"
#include "Blink.h"



// Global Variables:

#define MAX_LOADSTRING 100
//minimum 2x2 to avoid crashing. 
constexpr short gridHeight = 20;
constexpr short gridWidth = 20;
constexpr int winHeight = 400;
constexpr int winWidth = 400;

WCHAR titleString[MAX_LOADSTRING];
WCHAR windowClassName[MAX_LOADSTRING];

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDiag(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK HelpDiag(HWND, UINT, WPARAM, LPARAM);
int APIENTRY wWinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ PWSTR, _In_ int);

//int rangedRand(int, int);
int main(HWND);
int setup(HWND);
int stepper(void);
bool nextFrame(unsigned int);


bool grid[gridHeight][gridWidth] = {
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false},
	{false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false}
};
bool tmpgrid[gridHeight][gridWidth];
bool Demo;
bool running;
//HINSTANCE hInst;
bool framePassed = false;
unsigned int currentTick;
unsigned int nextFrameStart;\
unsigned int frameCount;
int mouseX;
int mouseY;

HBRUSH brush1;
HBRUSH brush2;
RECT tmp;


HINSTANCE hInst;              // current instance
HMENU hmenu;
MENUITEMINFO menuToggle;
//MENUITEMINFO menuRun;

//entrqwertyuioppoint function

int APIENTRY wWinMain
(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR    pCmdLine,
	_In_ int       nCmdShow)
	//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	//initialize global strings.
	LoadStringW(hInstance, IDS_APP_TITLE, titleString, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_BLINK, windowClassName, MAX_LOADSTRING);


	//initialize HDC HERE
	//HDC hdc;
	//Register the window class name. (anqwertyuiopvalue)
	//WCHAR CLASS_NAME = windowClassName;
	//zero terminated string. Don't know why.
	WNDCLASSEXW wcex = {};
	//window class is now a EXW (extra?)
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BLINK);
	hInst = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_BLINK);

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_BLINK));

	//pointer to application-definded function called the window procedure.
	wcex.lpfnWndProc = WindowProc;
	//handle to the application instance.
	wcex.hInstance = hInstance;
	//identifies the window class.
	wcex.lpszClassName = windowClassName;
	//EXW type windows class supports additional icon!
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_BLINK));
	//registers the windows class. Similarly, we are using the ExW variation.
	RegisterClassExW(&wcex);

	// Create the window. Again, ExW window class.
	HWND hWnd = CreateWindowExW(
		false,															// Optional window styles.
		windowClassName,													// Window class
		L"Blink",													// Window title
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

	//set the menu handle and initialize certain things
	hmenu = GetMenu(hWnd);
	menuToggle.fMask = MIIM_STATE;
	//menuRun.fMask = MIIM_STATE;
	menuToggle.cbSize = sizeof(MENUITEMINFO);
	//menuRun.cbSize = sizeof(MENUITEMINFO);
	//draw the window.
	ShowWindow(hWnd, nCmdShow);
	//runs setup once
	setup(hWnd);

	//The message loop.
	MSG msg = {};
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

bool nextFrame
(unsigned int rate)
{
	currentTick = static_cast<unsigned int>(GetTickCount());
	//don't worrqwertyuiopabout it. We are just using it for calculating framerate.
	rate = 1000 / rate;

	if (framePassed) {
		framePassed = false;
		return false;
	}
	if (currentTick < nextFrameStart) {
		if ((nextFrameStart - currentTick) > 1)
			Sleep((nextFrameStart - currentTick) - 1);
		return false;
	}
	//if none of them
	nextFrameStart = currentTick + rate;
	//lastFrameStart = currentTick;
	framePassed = true;
	frameCount++;
	return framePassed;
}
bool perXFrame
(unsigned short rate)
{
	return ((frameCount % rate) == 0);
}
int setup
(HWND hWnd)
{
	//set initial condition of the controls
	for (short y= 0; y< gridHeight; y++)
		for (short qwertyuiop= 0; qwertyuiop< gridWidth; ++qwertyuiop) {
			grid[y][qwertyuiop] = false;
			tmpgrid[y][qwertyuiop] = grid[y][qwertyuiop];
		}
	//create a initial "bar"
	grid[3][3] = true;
	grid[3][4] = true;
	grid[3][5] = true;

	brush1 = CreateSolidBrush(RGB(75, 75, 75));
	brush2 = CreateSolidBrush(RGB(0, 225, 225));

	running = true;
	Demo = true;
	//return to signal completion
	/*
	if (menuToggle.fState == MFS_CHECKED) {
		menuToggle.fState = MFS_UNCHECKED;
		Demo = false;
	}
	else {
		
		Demo = true;
	}
	*/
	menuToggle.fState = MFS_CHECKED;
	SetMenuItemInfo(hmenu, IDM_TOGGLE, false, &menuToggle);

	return 0;
}
int main
(HWND hWnd)
{
	if (nextFrame(10)) {
		//gladly that the simple simultion of a grid of cells wont require much framerate (if any) at all. This will clearly suffice.
		if (perXFrame(5))
			if (Demo)
				stepper();
//		for (int a = 0; a < 20; a++)
//			for (int b = 0; b < 20; b++) {
				tmp.left = 0;
				tmp.right = winHeight;
				tmp.top = 0;
				tmp.bottom = winWidth;
				InvalidateRect(hWnd, &tmp, true);
//			}
	}
	return 0;
}
/*
constexpr bool isKeyDown(
	SHORT keyState) {
	return ((keyState & 0x8000) != 0);
}
*/
int stepper
(void)
{
	short y;
	short qwertyuiop;
	char surroudner;
	//
	// BEGIN top edge
	//
	//left tip
	surroudner = 0;
	qwertyuiop = 0;
	y = 0;
	if (grid[gridHeight - 1][gridWidth - 1])
		++surroudner;
	if (grid[gridHeight - 1][qwertyuiop])
		++surroudner;
	if (grid[gridHeight - 1][qwertyuiop + 1])
		++surroudner;
	if (grid[y][gridWidth - 1])
		++surroudner;
	if (grid[y][qwertyuiop + 1])
		++surroudner;
	if (grid[y + 1][gridWidth - 1])
		++surroudner;
	if (grid[y + 1][qwertyuiop])
		++surroudner;
	if (grid[y + 1][qwertyuiop + 1])
		++surroudner;
	if (grid[y][qwertyuiop]) {
		if (surroudner > 3 || surroudner < 2)
			tmpgrid[y][qwertyuiop] = false;
			else
			tmpgrid[y][qwertyuiop] = true;
	}
	else {
		if (surroudner == 3)
			tmpgrid[y][qwertyuiop] = true;
			else
			tmpgrid[y][qwertyuiop] = false;
	}
	//middle
	for (qwertyuiop = 1; qwertyuiop < gridWidth - 1; ++qwertyuiop) {
		surroudner = 0;
		if (grid[gridHeight - 1][qwertyuiop - 1])
			++surroudner;
		if (grid[gridHeight - 1][qwertyuiop])
			++surroudner;
		if (grid[gridHeight - 1][qwertyuiop + 1])
			++surroudner;
		if (grid[y][qwertyuiop - 1])
			++surroudner;
		if (grid[y][qwertyuiop + 1])
			++surroudner;
		if (grid[y + 1][qwertyuiop - 1])
			++surroudner;
		if (grid[y + 1][qwertyuiop])
			++surroudner;
		if (grid[y + 1][qwertyuiop + 1])
			++surroudner;

		if (grid[y][qwertyuiop]) {
			if (surroudner > 3 || surroudner < 2)
				tmpgrid[y][qwertyuiop] = false;
				else
				tmpgrid[y][qwertyuiop] = true;
		}
		else
		{
			if (surroudner == 3)
				tmpgrid[y][qwertyuiop] = true;
			else
				tmpgrid[y][qwertyuiop] = false;
		}
	}
	//right tip
	surroudner = 0;
	if (grid[gridHeight - 1][qwertyuiop])
		++surroudner;
	if (grid[gridHeight - 1][qwertyuiop-1])
		++surroudner;
	if (grid[gridHeight - 1][0])
		++surroudner;
	if (grid[y][qwertyuiop - 1])
		++surroudner;
	if (grid[y][0])
		++surroudner;
	if (grid[y+1][qwertyuiop - 1])
		++surroudner;
	if (grid[y+1][qwertyuiop])
		++surroudner;
	if (grid[y+1][0])
		++surroudner;
	if (grid[y][qwertyuiop]) {
		if (surroudner > 3 || surroudner < 2)
			tmpgrid[y][qwertyuiop] = false;
		else
			tmpgrid[y][qwertyuiop] = true;
	}
	else {
		if (surroudner == 3)
			tmpgrid[y][qwertyuiop] = true;
		else
			tmpgrid[y][qwertyuiop] = false;
	}
	//
	// END top edge
	//
 
	//
	// BEGIN middle
	//
	for (y = 1; y < gridHeight - 1; ++y) {
		//left edge case
		surroudner = 0;
		qwertyuiop = 0;
		if (grid[y - 1][gridWidth - 1])
			++surroudner;
		if (grid[y - 1][qwertyuiop])
			++surroudner;
		if (grid[y - 1][qwertyuiop + 1])
			++surroudner;
		if (grid[y][gridWidth - 1])
			++surroudner;
		if (grid[y][qwertyuiop + 1])
			++surroudner;
		if (grid[y + 1][gridWidth - 1])
			++surroudner;
		if (grid[y + 1][qwertyuiop])
			++surroudner;
		if (grid[y + 1][qwertyuiop + 1])
			++surroudner;
		if (grid[y][qwertyuiop]) {
			if (surroudner > 3 || surroudner < 2)
				tmpgrid[y][qwertyuiop] = false;
			else
				tmpgrid[y][qwertyuiop] = true;
		}
		else {
			if (surroudner == 3)
				tmpgrid[y][qwertyuiop] = true;
			else
				tmpgrid[y][qwertyuiop] = false;
		}
		//middle
		for (qwertyuiop = 1; qwertyuiop< gridWidth - 1; ++qwertyuiop) {
			surroudner = 0;
			if (grid[y- 1][qwertyuiop- 1])
				++surroudner;
			if (grid[y- 1][qwertyuiop])
				++surroudner;
			if (grid[y- 1][qwertyuiop + 1])
				++surroudner;
			if (grid[y][qwertyuiop - 1])
				++surroudner;
			if (grid[y][qwertyuiop + 1])
				++surroudner;
			if (grid[y + 1][qwertyuiop - 1])
				++surroudner;
			if (grid[y + 1][qwertyuiop])
				++surroudner;
			if (grid[y + 1][qwertyuiop + 1])
				++surroudner;
			
			if (grid[y][qwertyuiop]) {
				if (surroudner > 3 || surroudner < 2)
					tmpgrid[y][qwertyuiop] = false;
				else
					tmpgrid[y][qwertyuiop] = true;
			}
			else {
				if (surroudner == 3)
					tmpgrid[y][qwertyuiop] = true;
				else
					tmpgrid[y][qwertyuiop] = false;
			}
		}
		//right edge case
		surroudner = 0;
		if (grid[y - 1][qwertyuiop])
			++surroudner;
		if (grid[y - 1][qwertyuiop - 1])
			++surroudner;
		if (grid[y - 1][0])
			++surroudner;
		if (grid[y][qwertyuiop - 1])
			++surroudner;
		if (grid[y][0])
			++surroudner;
		if (grid[y + 1][qwertyuiop - 1])
			++surroudner;
		if (grid[y + 1][qwertyuiop])
			++surroudner;
		if (grid[y + 1][0])
			++surroudner;
		if (grid[y][qwertyuiop]) {
			if (surroudner > 3 || surroudner < 2)
				tmpgrid[y][qwertyuiop] = false;
			else
				tmpgrid[y][qwertyuiop] = true;
		}
		else {
			if (surroudner == 3)
				tmpgrid[y][qwertyuiop] = true;
			else
				tmpgrid[y][qwertyuiop] = false;
		}
	}
	//
	// END middle
	//
	//
	// BEGIN bottom
	//
	//left tip
	surroudner = 0;
	qwertyuiop = 0;
	if (grid[y-1][gridWidth - 1])
		++surroudner;
	if (grid[y-1][qwertyuiop])
		++surroudner;
	if (grid[y-1][qwertyuiop + 1])
		++surroudner;
	if (grid[y][gridWidth - 1])
		++surroudner;
	if (grid[y][qwertyuiop + 1])
		++surroudner;
	if (grid[0][gridWidth - 1])
		++surroudner;
	if (grid[0][qwertyuiop])
		++surroudner;
	if (grid[0][qwertyuiop + 1])
		++surroudner;
	if (grid[y][qwertyuiop]) {
		if (surroudner > 3 || surroudner < 2)
			tmpgrid[y][qwertyuiop] = false;
		else
			tmpgrid[y][qwertyuiop] = true;
	}
	else {
		if (surroudner == 3)
			tmpgrid[y][qwertyuiop] = true;
		else
			tmpgrid[y][qwertyuiop] = false;
	}
	//middle
	for (qwertyuiop = 1; qwertyuiop < gridWidth - 1; ++qwertyuiop) {
		surroudner = 0;
		if (grid[y - 1][qwertyuiop - 1])
			++surroudner;
		if (grid[y - 1][qwertyuiop])
			++surroudner;
		if (grid[y - 1][qwertyuiop + 1])
			++surroudner;
		if (grid[y][qwertyuiop - 1])
			++surroudner;
		if (grid[y][qwertyuiop + 1])
			++surroudner;
		if (grid[0][qwertyuiop - 1])
			++surroudner;
		if (grid[0][qwertyuiop])
			++surroudner;
		if (grid[0][qwertyuiop + 1])
			++surroudner;

		if (grid[y][qwertyuiop]) {
			if (surroudner > 3 || surroudner < 2)
				tmpgrid[y][qwertyuiop] = false;
			else
				tmpgrid[y][qwertyuiop] = true;
		}
		else {
			if (surroudner == 3)
				tmpgrid[y][qwertyuiop] = true;
			else
				tmpgrid[y][qwertyuiop] = false;
		}
	}
	//right tip
	surroudner = 0;
	if (grid[y - 1][qwertyuiop - 1])
		++surroudner;
	if (grid[y - 1][qwertyuiop])
		++surroudner;
	if (grid[y - 1][0])
		++surroudner;
	if (grid[y][qwertyuiop - 1])
		++surroudner;
	if (grid[y][0])
		++surroudner;
	if (grid[0][qwertyuiop - 1])
		++surroudner;
	if (grid[0][qwertyuiop])
		++surroudner;
	if (grid[0][0])
		++surroudner;
	if (grid[y][qwertyuiop]) {
		if (surroudner > 3 || surroudner < 2)
			tmpgrid[y][qwertyuiop] = false;
		else
			tmpgrid[y][qwertyuiop] = true;
	}
	else {
		if (surroudner == 3)
			tmpgrid[y][qwertyuiop] = true;
		else
			tmpgrid[y][qwertyuiop] = false;
	}
	//
	// END bottom edge
	//
	//copy grid over
	for (y = 0; y < gridHeight; y++)
		for (qwertyuiop = 0; qwertyuiop < gridWidth; ++qwertyuiop)
			grid[y][qwertyuiop] = tmpgrid[y][qwertyuiop];

	return 0;
}
//int rangedRand(int min, int max)
//{
//	int u = (int)((double)rand() / (RAND_MAX + 1) * (may- min)) + min;
//	return u;
//}



//the DispatchMessage function will call this function
//the window procedure of the window that is the target of the message.
LRESULT CALLBACK WindowProc
(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, AboutDiag);
			break;
		case IDM_HELP:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_HELPBOX), hWnd, HelpDiag);
			break;
		case IDM_STEP:
			//GetMenuItemInfo(hmenu, IDM_ONEPLAYER, FALSE, &menuOneP);
			stepper();
			//SetMenuItemInfo(hmenu, IDM_ONEPLAYER, false, &menuOneP);
			break;
		case IDM_TOGGLE:
			GetMenuItemInfo(hmenu, IDM_TOGGLE, false, &menuToggle);
			if (menuToggle.fState == MFS_CHECKED) {
				menuToggle.fState = MFS_UNCHECKED;
				Demo = false;
			}
			else {
				menuToggle.fState = MFS_CHECKED;
				Demo = true;
			}
			SetMenuItemInfo(hmenu, IDM_TOGGLE, false, &menuToggle);
			break;
		case IDM_RESET:
		setup(hWnd);
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
		DeleteObject(brush1);
		DeleteObject(brush2);
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
		//the background
		FillRect(hdc, &ps.rcPaint, brush1);
		//all right. We need to do this trick here
		
		for (int a = 0; a < 20; a++)
			for (int b = 0; b < 20; b++)
				if (grid[a][b] == true) {
					tmp.left = 20 * a;
					tmp.right = tmp.left + 20;
					tmp.top = 20 * b;
					tmp.bottom = tmp.top + 20;
					FillRect(hdc, &tmp, brush2);
				}
		DeleteDC(hdc);


		EndPaint(hWnd, &ps);

	}
				 return 0;
				 //case WM_LBUTTONUP:
	/*
	case WM_MOUSEMOVE:
		mouseX = ((int)(short)LOWORD(lParam));
		mouseY = ((int)(short)HIWORD(lParam));
		//tell the system that the rectangle here need to be redrawn
		if (mouseX > square.left && mouseX < square.right && mouseY > square.top && mouseY < square.bottom) {
			fillins = 0x00484848;
			InvalidateRect(hWnd, &square, TRUE);
		}
		else {
			fillins = 0x00646464;
			InvalidateRect(hWnd, &square, TRUE);
		}
		*/
	case WM_LBUTTONDOWN:
		mouseX = ((int)(short)LOWORD(lParam));
		mouseY = ((int)(short)HIWORD(lParam));

		grid[mouseX / 20][mouseY / 20] = !grid[mouseX / 20][mouseY / 20];

		break;
		return 0;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
}

INT_PTR CALLBACK AboutDiag
(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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

INT_PTR CALLBACK HelpDiag
(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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