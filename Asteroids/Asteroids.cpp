// Asteroids.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "Asteroids.h"

#define maxStringLength 100

#define winWidth 800
#define winHeight 600
#define calcWidth (winWidth * 100)
#define calcHeight (winHeight * 100)
#define bulletSize 2
// Global Variables:
WCHAR titleString[maxStringLength];
WCHAR windowClassName[maxStringLength];
HINSTANCE hInst;
bool framePassed;
unsigned int nextFrameStart;
bool running;
struct keystate {
	bool left;
	bool right;
	bool up;
	bool down;
	bool spaceB;
};
keystate STDKey;

signed char bulletCount;
signed char asteroidCount;
signed char rockCount;
signed char pebbleCount;
signed char life;
signed char level;
unsigned short score;
signed short tick;
//bool turnC;
enum class ProgState : int { Main, Simulation, DataDisplay, Pause };


//constants (for calculation of ship movement)
static const signed char heading[24][2] = {
{ +00, -20}, { +05, -19},
{ +10, -17}, { +14, -14},
{ +17, -10}, { +19, -05},
{ +20, +00}, { +19, +05},
{ +17, +10}, { +14, +14},
{ +10, +17}, { +05, +19},
{ +00, +20}, { -05, +19},
{ -10, +17}, { -14, +14},
{ -17, +10}, { -19, +05},
{ -20, +00}, { -19, -05},
{ -17, -10}, { -14, -14},
{ -10, -17}, { -05, -19}
};

static const signed short shipTbl[6][24] = {
	//ship.var
	// 0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16    17    18    19    20    21    22    23
	{ -1131, -1385, -1545, -1600, -1545, -1385, -1131, -800, -414, -0000, +414, +800, +1131, +1385, +1545, +1600, +1545, +1385, +1131, +800, +414, +0000,  -414, -800}, //left point x
	{+1131, +800, +414, +0000, -414, -800, -1131, -1385, -1545, -1600, -1545, -1385, -1131, -800, -414, -0000, +414, +800, +1131, +1385, +1545, +1600, +1545, +1385}, //left point y
	{+1600, +1545, +1385, +1131, +800, +414, +0000, -414, -800, -1131, -1385, -1545, -1600, -1545, -1385, -1131, -800, -414, -0000, +414, +800, +1131, +1385, +1545}, //central point x
	{-0000, +414, +800, +1131, +1385, +1545, +1600, +1545, +1385, +1131, +800, +414, +0000,  -414, -800, -1131, -1385, -1545, -1600, -1545, -1385, -1131, -800, -414}, //central point y
	{-1131, -800, -414, -0000, +414, +800, +1131, +1385, +1545, +1600, +1545, +1385, +1131, +800, +414, +0000, -414, -800, -1131, -1385, -1545, -1600, -1545, -1385}, //right point x
	{ -1131, -1385, -1545, -1600, -1545, -1385, -1131, -800, -414, -0000, +414, +800, +1131, +1385, +1545, +1600, +1545, +1385, +1131, +800, +414, +0000,  -414, -800}  //right point y
};

class Object {
public:
	int x;
	int y;
	signed short vx;
	signed short vy;
	signed char var;
private:
};
class advObject {
public:
	RECT coord;
	signed short vx;
	signed short vy;
	signed char var;
};
class extObject {
public:
	RECT coord;
	int x;
	int y;
	signed short vx;
	signed short vy;
	signed char var;
};

POINT shipArray[4];
Object ship;
extObject bullet[8];
extObject asteroids[8];
extObject rocks[16];
extObject pebbles[32];
Object UFO;
RECT UFORect;
extObject UFOB;
//UFO:
//UFO includes UFO's x,y,vx & vy as well as heading(for bullets)
//UFOB && bullets use var as storage for bullet life
//rocks, pebbles and asteroids use it as different sprite choice
#define shipX0  (ship.x + shipTbl[0][ship.var] / 2)
#define shipY0  (ship.y + shipTbl[1][ship.var] / 2)
#define shipX1  (ship.x + shipTbl[2][ship.var] / 2)
#define shipY1  (ship.y + shipTbl[3][ship.var] / 2)
#define shipX2  (ship.x + shipTbl[4][ship.var] / 2)
#define shipY2  (ship.y + shipTbl[5][ship.var] / 2)
unsigned char invasion = 0;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool nextFrame(unsigned int);
void PaintProc(PAINTSTRUCT, HDC);
int setup(HWND);
int main(HWND);
int random(int, int);
void guidance();
void trajectory();
void firing(HWND);
void collision();
void asteroid();
void resetField();
void rock();
void pebble();
void radar();
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ PWSTR    pCmdLine,
	_In_ int       nCmdShow)
{
	//initialize global strings.
	LoadStringW(hInstance, IDS_APP_TITLE, titleString, maxStringLength);
	LoadStringW(hInstance, IDC_ASTEROIDS, windowClassName, maxStringLength);

	WNDCLASSEXW wcex = {};
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ASTEROIDS);
	//stores the instance handle (used in creation of pop up windows)
	hInst = hInstance;
	//load the default arrow cursor
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//defines the background color (what for?)
	wcex.hbrBackground = (HBRUSH)0x0B;
	//load the window name from the resources
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_ASTEROIDS);
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	//give the window a icon
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ASTEROIDS));
	//pointer to application-definded function called the window procedure.
	wcex.lpfnWndProc = WindowProc;
	//handle to the application instance.
	wcex.hInstance = hInstance;
	//identifies the window class.
	wcex.lpszClassName = windowClassName;
	//EXW type windows class supports additional icon!
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ASTEROIDS));
	//registers the windows class.
	RegisterClassExW(&wcex);

	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowExW(
		//optional window style
		0,
		//window class
		windowClassName,
		//window title
		L"Asteroids",
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
	//runs setup once
	setup(hWnd);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	//The message loop.
	MSG msg/* = {}*/;
	while (running) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) != 0) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				running = false;
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
	ship.x = 15000;
	ship.y = 12500;
	ship.var = 4;
	running = true;
	resetField();
	return 0;
}
int main(
	HWND hWnd)
{
	if (nextFrame(30)) {
		InvalidateRgn(hWnd, NULL, TRUE);
		shipArray[0].x = shipX0 / 100;
		shipArray[0].y = shipY0 / 100;
		shipArray[1].x = shipX1 / 100;
		shipArray[1].y = shipY1 / 100;
		shipArray[2].x = shipX2 / 100;
		shipArray[2].y = shipY2 / 100;
		shipArray[3].x = (ship.x - shipTbl[2][ship.var] / 8) / 100;
		shipArray[3].y = (ship.y - shipTbl[3][ship.var] / 8) / 100;
		guidance();
		trajectory();
		asteroid();
		rock();
		pebble();
		radar();
	}
	return 0;
}
void PaintProc(
	PAINTSTRUCT ps, HDC hdc)
{
	//initialize a array of points to draw triangle

	FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(GRAY_BRUSH));
	SelectObject(hdc, GetStockObject(WHITE_PEN));
	if (tick > -120) {
		SelectObject(hdc, GetStockObject((tick < 0) ? WHITE_BRUSH : GRAY_BRUSH));
		Polygon(hdc, shipArray, 4);
	}
	for (int b = 0; b < bulletCount; ++b)
		FillRect(hdc, &bullet[b].coord, (HBRUSH)GetStockObject(WHITE_BRUSH));

	SelectObject(hdc, GetStockObject(GRAY_BRUSH));
	for (short a = 0; a < asteroidCount; ++a)
	Ellipse(hdc, (asteroids[a].x -2400) / 100, (asteroids[a].y -2400) / 100, (asteroids[a].x + 2400) / 100, (asteroids[a].y + 2400) / 100);
	for (short r = 0; r < rockCount; ++r)
	Ellipse(hdc, (rocks[r].x - 1600) / 100, (rocks[r].y - 1600) / 100, (rocks[r].x + 1600) / 100, (rocks[r].y + 1600) / 100);
	for (short p = 0; p < pebbleCount; ++p)
	Ellipse(hdc, (pebbles[p].x - 800) / 100, (pebbles[p].y - 800) / 100, (pebbles[p].x + 800) / 100, (pebbles[p].y + 800) / 100);
}

void guidance() {
	if (ship.vy > 0)ship.vy = ship.vy - 1;
	if (ship.vy < 0)ship.vy = ship.vy + 1;
	if (ship.vy > 0)ship.vx = ship.vx - 1;
	if (ship.vx < 0)ship.vx = ship.vx + 1;
	//if (abs(ship.vx) < 5) ship.vx = 0;
	//if (abs(ship.vy) < 5) ship.vy = 0;
	if (ship.vy > 600)ship.vy = 600;
	if (ship.vy < -600)ship.vy = -600;
	if (ship.vx > 600)ship.vx = 600;
	if (ship.vx < -600)ship.vx = -600;
	ship.x = ship.x + ship.vx;
	ship.y = ship.y + ship.vy;
	if (ship.x < 0)
		ship.x = ship.x + calcWidth;
	if (ship.x > calcWidth)
		ship.x = ship.x - calcWidth;
	if (ship.y < 0)
		ship.y = ship.y + calcHeight;
	if (ship.y > calcHeight)
		ship.y = ship.y - calcHeight;
	if (tick > -120) {
		if (STDKey.left)
			ship.var = ship.var - 1;
		if (STDKey.right)
			ship.var = ship.var + 1;
		if (ship.var > 23)
			ship.var = ship.var - 24;
		if (ship.var < 0)
			ship.var = ship.var + 24;

		if (STDKey.up) {
			ship.vx = ship.vx + shipTbl[2][ship.var] / 100;
			ship.vy = ship.vy + shipTbl[3][ship.var] / 100;
		}
		if (STDKey.down) {
			if (random(0, 3) < 2)
				ship.vx = ship.vy = 0;
			else
				collision();
			ship.x = random(0, (winWidth * 100));
			ship.y = random(0, (winHeight * 100));
		}
	}
}


void firing(HWND hWnd) {
	if (bulletCount < 8) {
		bullet[bulletCount].vx = shipTbl[2][ship.var];
		bullet[bulletCount].vy = shipTbl[3][ship.var];
		bullet[bulletCount].x = shipX1;
		bullet[bulletCount].y = shipY1;
		bullet[bulletCount].coord.left = bullet[bulletCount].x / 100;
		bullet[bulletCount].coord.top = bullet[bulletCount].y / 100;
		bullet[bulletCount].coord.bottom = bullet[bulletCount].coord.top + bulletSize;
		bullet[bulletCount].coord.right = bullet[bulletCount].coord.left + bulletSize;
		bullet[bulletCount].var = 100;
		InvalidateRect(hWnd, &bullet[bulletCount].coord, TRUE);
		bulletCount = bulletCount + 1;
	}
}
void trajectory() {
	for (int b = 0; b < bulletCount; b++) {
		bullet[b].x = bullet[b].x + bullet[b].vx;
		bullet[b].y = bullet[b].y + bullet[b].vy;
		if (bullet[b].x < 0)
			bullet[b].x = bullet[b].x + (calcWidth);
		if (bullet[b].x > calcWidth)
			bullet[b].x = bullet[b].x - (calcWidth);
		if (bullet[b].y < 0)
			bullet[b].y = bullet[b].y + (calcHeight);
		if (bullet[b].y > calcHeight)
			bullet[b].y = bullet[b].y - (calcHeight);
		bullet[b].coord.left = bullet[b].x / 100;
		bullet[b].coord.top = bullet[b].y / 100;
		bullet[b].coord.bottom = bullet[b].coord.top + bulletSize;
		bullet[b].coord.right = bullet[b].coord.left + bulletSize;
		
		
		if (bullet[b].var < 0) {
			bullet[b].var = 0;
			for (int c = b; c < bulletCount - 1; c++)
				bullet[c] = bullet[c + 1];
			bullet[bulletCount - 1].coord.left = 0;
			bullet[bulletCount - 1].coord.top = 0;
			bullet[bulletCount - 1].coord.right = 0;
			bullet[bulletCount - 1].coord.bottom = 0;
			bullet[bulletCount - 1].vx = 0;
			bullet[bulletCount - 1].vy = 0;
			bullet[bulletCount - 1].var = 0;
			bulletCount = bulletCount - 1;
		}
		bullet[b].var = bullet[b].var - 4;
	}
}
void asteroid() {
	for (int a = 0; a < asteroidCount; a++) {
		asteroids[a].x = asteroids[a].x + asteroids[a].vx;
		asteroids[a].y = asteroids[a].y + asteroids[a].vy;
		if (asteroids[a].x < 0) asteroids[a].x = asteroids[a].x + (calcWidth);
		if (asteroids[a].x > calcWidth) asteroids[a].x = asteroids[a].x - (calcWidth);
		if (asteroids[a].y < 0) asteroids[a].y = asteroids[a].y + (calcHeight);
		if (asteroids[a].y > calcHeight) asteroids[a].y = asteroids[a].y - (calcHeight);
		for (int c = 0; c < bulletCount; c++) {
			if ((abs(bullet[c].x - asteroids[a].x) <= 2400 && abs(bullet[c].y - asteroids[a].y) <= 2400) ||
				(abs(bullet[c].x - asteroids[a].x) + abs(bullet[c].y - asteroids[a].y) < 3000)) {
				for (int e = c; e < bulletCount - 1; e++) 
					bullet[e] = bullet[e + 1];
				
				bullet[bulletCount - 1].x = 0;
				bullet[bulletCount - 1].vx = 0;
				bullet[bulletCount - 1].y = 0;
				bullet[bulletCount - 1].vy = 0;
				bullet[bulletCount - 1].var = 0;
				bulletCount = bulletCount - 1;

				score = score + 2;

				rocks[rockCount].x = asteroids[a].x;
				rocks[rockCount].y = asteroids[a].y;
				rocks[rockCount].vx = random(-40, 40) * 4;
				rocks[rockCount].vy = random(-40, 40) * 4;
				rocks[rockCount].var = random(0, 4);
				rockCount = rockCount + 1;
				rocks[rockCount].x = asteroids[a].x;
				rocks[rockCount].y = asteroids[a].y;
				rocks[rockCount].vx = random(-40, 40) * 4;
				rocks[rockCount].vy = random(-40, 40) * 4;
				rocks[rockCount].var = random(0, 4);
				rockCount = rockCount + 1;

				for (int d = a; d < asteroidCount - 1; d++) 
					asteroids[d] = asteroids[d + 1];
				
				asteroidCount = asteroidCount - 1;
			}
		}
		
	}
}
void rock() {
	for (int r = 0; r < rockCount; r++) {
		rocks[r].x = rocks[r].x + rocks[r].vx;
		rocks[r].y = rocks[r].y + rocks[r].vy;
		if (rocks[r].x < 0) rocks[r].x = rocks[r].x + (calcWidth);
		if (rocks[r].x > calcWidth) rocks[r].x = rocks[r].x - (calcWidth);
		if (rocks[r].y < 0) rocks[r].y = rocks[r].y + (calcHeight);
		if (rocks[r].y > calcHeight) rocks[r].y = rocks[r].y - (calcHeight);
		for (int c = 0; c < bulletCount; c++) {
			if ((abs(bullet[c].x - rocks[r].x) <= 1600 && abs(bullet[c].y - rocks[r].y) <= 1600) ||
				(abs(bullet[c].x - rocks[r].x) + abs(bullet[c].y - rocks[r].y) < 2000)) {
				for (int e = c; e < bulletCount - 1; e++) 
					bullet[e] = bullet[e + 1];
				
				bullet[bulletCount - 1].x = 0;
				bullet[bulletCount - 1].vx = 0;
				bullet[bulletCount - 1].y = 0;
				bullet[bulletCount - 1].vy = 0;
				bullet[bulletCount - 1].var = 0;
				bulletCount = bulletCount - 1;

				score = score + 4;

				pebbles[pebbleCount].x = rocks[r].x;
				pebbles[pebbleCount].y = rocks[r].y;
				pebbles[pebbleCount].vx = random(-40, 40) * 4;
				pebbles[pebbleCount].vy = random(-40, 40) * 4;
				pebbles[pebbleCount].var = random(0, 4);
				pebbleCount = pebbleCount + 1;
				pebbles[pebbleCount].x = rocks[r].x;
				pebbles[pebbleCount].y = rocks[r].y;
				pebbles[pebbleCount].vx = random(-40, 40) * 4;
				pebbles[pebbleCount].vy = random(-40, 40) * 4;
				pebbles[pebbleCount].var = random(0, 4);
				pebbleCount = pebbleCount + 1;

				for (int d = r; d < rockCount - 1; d++) 
					rocks[d] = rocks[d + 1];
				rockCount = rockCount - 1;
			}
		}
	}
}
void pebble() {
	for (int p = 0; p < pebbleCount; p++) {
		pebbles[p].x = pebbles[p].x + pebbles[p].vx;
		pebbles[p].y = pebbles[p].y + pebbles[p].vy;
		if (pebbles[p].x < 0) pebbles[p].x = pebbles[p].x + (calcWidth);
		if (pebbles[p].x > calcWidth) pebbles[p].x = pebbles[p].x - (calcWidth);
		if (pebbles[p].y < 0) pebbles[p].y = pebbles[p].y + (calcHeight);
		if (pebbles[p].y > calcHeight) pebbles[p].y = pebbles[p].y - (calcHeight);
		for (int c = 0; c < bulletCount; c++) {
			if (abs(bullet[c].x - pebbles[p].x) <= 800 && abs(bullet[c].y - pebbles[p].y) <= 800 || 
				(abs(bullet[c].x - pebbles[p].x) + abs(bullet[c].y - pebbles[p].y) < 1000)) {
				for (int e = c; e < bulletCount - 1; e++)
					bullet[e] = bullet[e + 1];
				bullet[bulletCount - 1].x = 0;
				bullet[bulletCount - 1].vx = 0;
				bullet[bulletCount - 1].y = 0;
				bullet[bulletCount - 1].vy = 0;
				bullet[bulletCount - 1].var = 0;
				bulletCount = bulletCount - 1;

				score = score + 8;

				for (int d = p; d < pebbleCount - 1; d++) 
					pebbles[d] = pebbles[d + 1];
				pebbleCount = pebbleCount - 1;
			}
		}
	}
}
void radar() {
	if (tick >= 0) {
		for (int a = 0; a < asteroidCount; a++) {
			if ((abs(asteroids[a].x - shipX0) < 1200 && abs(asteroids[a].y - shipY0) < 1200) ||
				((abs(asteroids[a].x - shipX0) + abs(asteroids[a].y - shipY0)) < 1600) ||
				(abs(asteroids[a].x - shipX1) < 1200 && abs(asteroids[a].y - shipY1) < 1200) ||
				((abs(asteroids[a].x - shipX1) + abs(asteroids[a].y - shipY1)) < 1600) ||
				(abs(asteroids[a].x - shipX2) < 1200 && abs(asteroids[a].y - shipY2) < 1200) ||
				((abs(asteroids[a].x - shipX2) + abs(asteroids[a].y - shipY2)) < 1600)) {
				collision();
				rocks[rockCount].x = asteroids[a].x + asteroids[a].vx;
				rocks[rockCount].y = asteroids[a].y + asteroids[a].vy;
				rocks[rockCount].vx = random(-40, 40) * 4;
				rocks[rockCount].vy = random(-40, 40) * 4;
				rocks[rockCount].var = random(0, 4);
				rockCount = rockCount + 1;
				rocks[rockCount].x = asteroids[a].x + asteroids[a].vx;
				rocks[rockCount].y = asteroids[a].y + asteroids[a].vy;
				rocks[rockCount].vx = random(-40, 40) * 4;
				rocks[rockCount].vy = random(-40, 40) * 4;
				rocks[rockCount].var = random(0, 4);
				rockCount = rockCount + 1;
				for (int d = a; d < asteroidCount - 1; d++)
					asteroids[d] = asteroids[d + 1];
				asteroidCount = asteroidCount - 1;
			}
		}
		for (int r = 0; r < rockCount; r++) {
			if ((abs(rocks[r].x - shipX0) < 800 && abs(rocks[r].y - shipY0) < 800) ||
				(abs(rocks[r].x - shipX0) + abs(rocks[r].y - shipY0) < 1200) ||
				(abs(rocks[r].x - shipX1) < 800 && abs(rocks[r].y - shipY1) < 800) ||
				(abs(rocks[r].x - shipX1) + abs(rocks[r].y - shipY1) < 1200) ||
				(abs(rocks[r].x - shipX2) < 800 && abs(rocks[r].y - shipY2) < 800) ||
				(abs(rocks[r].x - shipX2) + abs(rocks[r].y - shipY2) < 1200)) {
				collision();
				pebbles[pebbleCount].x = rocks[r].x + rocks[r].vx;
				pebbles[pebbleCount].y = rocks[r].y + rocks[r].vy;
				pebbles[pebbleCount].vx = random(-50, 50);
				pebbles[pebbleCount].vy = random(-50, 50);
				pebbles[pebbleCount].var = random(0, 4);
				pebbleCount = pebbleCount + 1;
				pebbles[pebbleCount].x = rocks[r].x + rocks[r].vx;
				pebbles[pebbleCount].y = rocks[r].y + rocks[r].vy;
				pebbles[pebbleCount].vx = random(-40, 40) * 4;
				pebbles[pebbleCount].vy = random(-40, 40) * 4;
				pebbles[pebbleCount].var = random(0, 4);
				pebbleCount = pebbleCount + 1;
				for (int d = r; d < rockCount - 1; d++)
					rocks[d] = rocks[d + 1];
				rockCount = rockCount - 1;
			}
		}
		for (int p = 0; p < pebbleCount; p++) {
			if ((abs(pebbles[p].x - shipX0) < 400 && abs(pebbles[p].y - shipY0) < 400) ||
				((abs(pebbles[p].x - shipX0) + abs(pebbles[p].y - shipY0)) < 600) ||
				(abs(pebbles[p].x - shipX1) < 400 && abs(pebbles[p].y - shipY1) < 400) ||
				((abs(pebbles[p].x - shipX1) + abs(pebbles[p].y - shipY1)) < 600) ||
				(abs(pebbles[p].x - shipX2) < 400 && abs(pebbles[p].y - shipY2) < 400) ||
				((abs(pebbles[p].x - shipX2) + abs(pebbles[p].y - shipY2)) < 600)) {
				collision();
				for (int d = p; d < pebbleCount - 1; d++)
					pebbles[d] = pebbles[d + 1];
				pebbleCount = pebbleCount - 1;
			}
		}
	}
	
	else {
		tick = tick + 1;
		if (tick >= 0) tick = 120;
	}
	if (asteroidCount == 0 && rockCount == 0 && pebbleCount == 0 && tick >= 0 && invasion <= 0) {
		tick = tick - 1;
		if (tick <= 0) {
			tick = 120;
			if (level < 8) level = level + 1;
			for (signed short s = 0; s < level; s++) {
				asteroids[asteroidCount].x = random(0, winWidth * 40) - winWidth * 25;
				asteroids[asteroidCount].y = random(0, winHeight * 40) - winWidth * 25;
				asteroids[asteroidCount].vx = random(-40, 40) * 4;
				asteroids[asteroidCount].vy = random(-40, 40) * 4;
				asteroids[asteroidCount].var = random(0, 4);
				asteroidCount = asteroidCount + 1;
			}
		}
	}
}
void collision() {
	tick = -240;
	life = life - 1;
	ship.x = winWidth * 50;
	ship.y = winHeight * 50;
	ship.vx = 0;
	ship.vy = 0;
}
void resetField() {
	level = 0;
	ship.vx = ship.vy = 0;
	tick = 120;
	bulletCount = 0;
	asteroidCount = rockCount = pebbleCount = 0;
}

int random(
	int min,
	int max)
{
	int u = (int)((float)rand() / (RAND_MAX) * (max - min)) + min;
	return u;
}



LRESULT CALLBACK WindowProc(
	HWND hWnd, //handle of window
	UINT uMsg, //message code
	WPARAM wParam, //additional data to message
	LPARAM lParam) //LRESULT is int value that is a "response" to the message
{
	switch (uMsg) {
	case WM_COMMAND: {
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
				break;
	case WM_PAINT: {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		PaintProc(ps, hdc);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_KEYDOWN:
		switch (wParam) {
		case 0x57:
		case VK_UP:
			STDKey.up = true;
			break;
		case 0x53:
		case VK_DOWN:
			STDKey.down = true;
			break;
		case 0x41:
		case VK_LEFT:
			STDKey.left = true;
			break;
		case 0x44:
		case VK_RIGHT:
			STDKey.right = true;
			break;
		case VK_SPACE:
			STDKey.spaceB = true;
			firing(hWnd);
			break;
		}
		return 0;
	case WM_KEYUP:
		switch (wParam) {
		case 0x57:
		case VK_UP:
			STDKey.up = false;
			break;
		case 0x53:
		case VK_DOWN:
			STDKey.down = false;
			break;
		case 0x41:
		case VK_LEFT:
			STDKey.left = false;
			break;
		case 0x44:
		case VK_RIGHT:
			STDKey.right = false;
			break;
		case VK_SPACE:
			STDKey.spaceB = false;
			break;
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
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
