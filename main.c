#pragma warning (disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <conio.h>
#include <math.h>
#include <windows.h>

#define ESC 27
#define SCREEN_SIZE 120
#define ROTATE 0.1
#define CAMERAPOINT 1.5


typedef struct _3DCOORD {
	double x;
	double y;
	double z;
}XYZ;

typedef struct _CUBE {
	XYZ F_T_L;
	XYZ F_T_R;
	XYZ F_B_L;
	XYZ F_B_R;
	XYZ B_T_L;
	XYZ B_T_R;
	XYZ B_B_L;
	XYZ B_B_R;
}CUBE;

void control(char* axis, int* direction);
void init_cube(CUBE* CUBE);
void rotate_cube(CUBE* CUBE, char axis, int direction);
XYZ rotateX(XYZ point, int direction);
XYZ rotateY(XYZ point, int direction);
XYZ rotateZ(XYZ point, int direction);
COORD pointProjection(XYZ point);
void pointConnection(COORD point1, COORD point2);
void render(CUBE* cube);
void stop(int returnvalue);
void title(char title[]);
void windowSize(int width, int height);
void fontSize(int size);
void cursorView(bool show);

bool SCREEN[SCREEN_SIZE + 1][SCREEN_SIZE + 1];
char buffer[((SCREEN_SIZE + 1) * 2) * (SCREEN_SIZE + 1) + 1];

int main() {
	title("3D CUBE");
	fontSize(4);
	windowSize(SCREEN_SIZE + 1, SCREEN_SIZE + 1);
	cursorView(false);

	CUBE CUBE;
	init_cube(&CUBE);

	char axis;
	int direction;

	while (true) {
		render(&CUBE);
		stop(printf("%s", buffer));
		control(&axis, &direction);
		rotate_cube(&CUBE, axis, direction);
	}
}

void control(char* axis, int* direction) {
	while (1) {
		switch (getch()) {
		case 'w': *axis = 'X'; *direction = +1; return;
		case 's': *axis = 'X'; *direction = -1; return;
		case 'a': *axis = 'Y'; *direction = +1; return;
		case 'd': *axis = 'Y'; *direction = -1; return;
		case 'q': *axis = 'Z'; *direction = +1; return;
		case 'e': *axis = 'Z'; *direction = -1; return;
		case ESC:   exit(0);
		}
	}
}

void init_cube(CUBE* CUBE) {
	CUBE->F_T_L.x = -1;
	CUBE->F_T_L.y = 1;
	CUBE->F_T_L.z = -1;

	CUBE->F_T_R.x = 1;
	CUBE->F_T_R.y = 1;
	CUBE->F_T_R.z = -1;

	CUBE->F_B_L.x = -1;
	CUBE->F_B_L.y = -1;
	CUBE->F_B_L.z = -1;

	CUBE->F_B_R.x = 1;
	CUBE->F_B_R.y = -1;
	CUBE->F_B_R.z = -1;

	CUBE->B_T_L.x = -1;
	CUBE->B_T_L.y = 1;
	CUBE->B_T_L.z = 1;

	CUBE->B_T_R.x = 1;
	CUBE->B_T_R.y = 1;
	CUBE->B_T_R.z = 1;

	CUBE->B_B_L.x = -1;
	CUBE->B_B_L.y = -1;
	CUBE->B_B_L.z = 1;

	CUBE->B_B_R.x = 1;
	CUBE->B_B_R.y = -1;
	CUBE->B_B_R.z = 1;
}

void rotate_cube(CUBE* CUBE, char axis, int direction) {
	XYZ(*rotateFunction)(XYZ, int);

	switch (axis) {
	case 'X': rotateFunction = rotateX; break;
	case 'Y': rotateFunction = rotateY; break;
	case 'Z': rotateFunction = rotateZ; break;
	default: exit(MessageBox(NULL, "정의되지 않은 축에 대한 회전", "오류", MB_OK));
	}

	CUBE->F_T_L = rotateFunction(CUBE->F_T_L, direction);
	CUBE->F_T_R = rotateFunction(CUBE->F_T_R, direction);
	CUBE->F_B_L = rotateFunction(CUBE->F_B_L, direction);
	CUBE->F_B_R = rotateFunction(CUBE->F_B_R, direction);
	CUBE->B_T_L = rotateFunction(CUBE->B_T_L, direction);
	CUBE->B_T_R = rotateFunction(CUBE->B_T_R, direction);
	CUBE->B_B_L = rotateFunction(CUBE->B_B_L, direction);
	CUBE->B_B_R = rotateFunction(CUBE->B_B_R, direction);
}

XYZ rotateX(XYZ point, int direction) {
	XYZ newPoint =
	{
		point.x,
		point.y*cos(ROTATE*direction) - point.z*sin(ROTATE*direction),
		point.y*sin(ROTATE*direction) + point.z*cos(ROTATE*direction)
	};
	return newPoint;
}

XYZ rotateY(XYZ point, int direction) {
	XYZ newPoint =
	{
		point.z*sin(ROTATE*direction) + point.x*cos(ROTATE*direction),
		point.y,
		point.z*cos(ROTATE*direction) - point.x*sin(ROTATE*direction)
	};
	return newPoint;
}

XYZ rotateZ(XYZ point, int direction) {
	XYZ newPoint =
	{
		point.x*cos(ROTATE*direction) - point.y*sin(ROTATE*direction),
		point.x*sin(ROTATE*direction) + point.y*cos(ROTATE*direction),
		point.z
	};
	return newPoint;
}

COORD pointProjection(XYZ point) {
	COORD returnCOORD;
	returnCOORD.X = (short)(((point.x / (point.z + 1 + CAMERAPOINT)) + 1) * (SCREEN_SIZE / 2));
	returnCOORD.Y = (short)(((point.y / (point.z + 1 + CAMERAPOINT)) + 1) * (SCREEN_SIZE / 2));
	return returnCOORD;
}

void pointConnection(COORD point1, COORD point2) {
	int dx = point2.X - point1.X;
	int dy = point2.Y - point1.Y;
	int ix = (dx > 0) - (dx < 0);
	int iy = (dy > 0) - (dy < 0);
	int err;

	dx = abs(dx) * 2;
	dy = abs(dy) * 2;

	SCREEN[point1.Y][point1.X] = true;

	if (dx >= dy) {
		err = dy - (dx / 2);
		while (point1.X != point2.X) {
			if (err >= 0) {
				if ((err != 0) || (ix > 0)) {
					point1.Y += iy;
					err -= dx;
				}
			}
			point1.X += ix;
			err += dy;
			SCREEN[point1.Y][point1.X] = true;
		}
	}
	else {
		err = dx - (dy / 2);
		while (point1.Y != point2.Y) {
			if (err >= 0) {
				if ((err != 0) || (iy > 0)) {
					point1.X += ix;
					err -= dy;
				}
			}
			point1.Y += iy;
			err += dx;
			SCREEN[point1.Y][point1.X] = true;
		}
	}
}

void render(CUBE* cube) {
	memset(SCREEN, false, sizeof(SCREEN));

	pointConnection(pointProjection(cube->F_T_L), pointProjection(cube->F_T_R));
	pointConnection(pointProjection(cube->F_B_L), pointProjection(cube->F_B_R));
	pointConnection(pointProjection(cube->F_T_L), pointProjection(cube->F_B_L));
	pointConnection(pointProjection(cube->F_T_R), pointProjection(cube->F_B_R));

	pointConnection(pointProjection(cube->B_T_L), pointProjection(cube->B_T_R));
	pointConnection(pointProjection(cube->B_B_L), pointProjection(cube->B_B_R));
	pointConnection(pointProjection(cube->B_T_L), pointProjection(cube->B_B_L));
	pointConnection(pointProjection(cube->B_T_R), pointProjection(cube->B_B_R));

	pointConnection(pointProjection(cube->F_T_L), pointProjection(cube->B_T_L));
	pointConnection(pointProjection(cube->F_T_R), pointProjection(cube->B_T_R));
	pointConnection(pointProjection(cube->F_B_L), pointProjection(cube->B_B_L));
	pointConnection(pointProjection(cube->F_B_R), pointProjection(cube->B_B_R));

	for (int y = SCREEN_SIZE; y >= 0; y--) {
		for (int x = 0; x <= SCREEN_SIZE; x++) {
			if (SCREEN[y][x]) {
				buffer[((SCREEN_SIZE - y) * (SCREEN_SIZE + 1) + x) * 2] = -95;
				buffer[((SCREEN_SIZE - y) * (SCREEN_SIZE + 1) + x) * 2 + 1] = -31;
			}
			else {
				buffer[((SCREEN_SIZE - y) * (SCREEN_SIZE + 1) + x) * 2] = ' ';
				buffer[((SCREEN_SIZE - y) * (SCREEN_SIZE + 1) + x) * 2 + 1] = ' ';
			}
		}
	}
}

void stop(int returnvalue) {
	COORD pos = { 0,0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void title(char title[]) {
	char middleadjust[30];
	sprintf(middleadjust, "title %s", title);
	system(middleadjust);
}

void windowSize(int width, int height) {
	char middleadjust[30];
	sprintf(middleadjust, "mode con cols=%d lines=%d", width * 2, height);
	system(middleadjust);
}

void fontSize(int size) {
	const wchar_t *font_name = L"FONT";
	const int font_height = size;

	HFONT font_handle =
		CreateFontW
		(
			font_height,
			0, 0, 0,
			FW_NORMAL,
			0, 0, 0,
			DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			font_name
		);

	LOGFONT logfont;
	GetObject(font_handle, sizeof logfont, &logfont);

	CONSOLE_FONT_INFOEX font_info = { sizeof(CONSOLE_FONT_INFOEX) };
	HANDLE cons_out = GetStdHandle(STD_OUTPUT_HANDLE);

	font_info.nFont = 12;
	font_info.dwFontSize.X = logfont.lfWidth;
	font_info.dwFontSize.Y = logfont.lfHeight;
	font_info.FontFamily = FF_DONTCARE;
	font_info.FontWeight = logfont.lfWeight;
	wcscpy(font_info.FaceName, font_name);
	SetCurrentConsoleFontEx(cons_out, FALSE, &font_info);
}

void cursorView(bool show) {
	CONSOLE_CURSOR_INFO ConsoleCursor = { 1,show };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ConsoleCursor);
}
