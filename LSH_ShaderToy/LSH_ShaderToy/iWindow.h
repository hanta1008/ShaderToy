#pragma once

#include "targetver.h"

#define _CRT_SECURE_NO_WARNINGS
#define CP_ENCODE CP_UTF8	// for eclispe + mingw32 : encode utf8
#define WIN32_LEAN_AND_MEAN // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

// Windows 헤더 파일
#include <windows.h>
#include <CommCtrl.h>
#pragma comment(lib, "comctl32")

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "logSystem.h"

extern HINSTANCE g_hInst;
extern HWND g_hWnd;
extern HDC g_hDC;

class WndCtrlSystem;
extern WndCtrlSystem* wcs;

typedef HBRUSH(*WND_COLOR_METHOD)(HWND hwnd, HDC hdc);
typedef void(*WND_UPDATE_METHOD)(HWND hwnd);

enum WndStyle
{
	WndStatic = 0,
	WndButton,

	WndCheckBox,
	WndComboBox,
	WndListBox,
	WndRadio,

	WndEdit,

	WndOpenGL,		// OpenGL 2.0이상 적용 쉐이더 적용
	WndListView,	// 게임 툴 제작시, 사용할 일 없음
};

//***************************
// WndCtrlSystem
//***************************
typedef struct _WndCtrl {
	HWND hwnd;
	WndStyle style;
	WND_COLOR_METHOD methodColor;
	WND_UPDATE_METHOD methodUpdate;
	int wndID;
}WndCtrl;

class WndCtrlSystem
{
public:
	WndCtrlSystem();
	virtual ~WndCtrlSystem();

public:
	void addWndCtrl(HWND hwnd, WndStyle style, WND_COLOR_METHOD colorMethod, WND_UPDATE_METHOD updateMethod, int wndID);
	void updateWndCtrl(WPARAM wParam, LPARAM lParam);
public:
	WndCtrl* wc;
	int wndCtrlCount;
};

// --------------------------------------
// ctrl - static
// --------------------------------------
HWND createWndStatic(int posx, int posy, int width, int height, 
	WND_COLOR_METHOD colorMethod, WND_UPDATE_METHOD updateMethod, const char* str);

// --------------------------------------
// ctrl - button
// --------------------------------------
HWND createWndButton(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str);
void wndButtonUpdate(HWND hwnd);

// --------------------------------------
// ctrl - check box
// --------------------------------------
HWND createWndCheckBox(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str);
bool getCheckBox(HWND hwnd);
void setCheckBox(HWND hwnd, bool on);
void wndCheckBoxUpdate(HWND hwnd);

// --------------------------------------
// ctrl - combo box
// --------------------------------------
HWND createWndComboBox(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char** str, int strNum);
void addWndComboBox(HWND hwnd, int index, const char* str);
void addWndComboBox(HWND hwnd, const char** str, int strNum);
void removeWndComboBox(HWND hwnd, int index);
int indexWndComboBox(HWND hwnd);
int countWndComboBox(HWND hwnd);
char* getWndComboBox(HWND hwnd, int index);
void setWndComboBox(HWND hwnd, int index);
void wndComboBoxUpdate(HWND hwnd);

// --------------------------------------
// ctrl - list box
// --------------------------------------
HWND createWndListBox(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char** str, int strNum);
void addWndListBox(HWND hwnd, int index, const char* str);
void addWndListBox(HWND hwnd, const char** str, int strNum);
void removeWndListBox(HWND hwnd, int index);
int indexWndListBox(HWND hwnd);
int countWndListBox(HWND hwnd);
char* getWndListBox(HWND hwnd, int index);
void setWndListBox(HWND hwnd, int index);
void wndListBoxUpdate(HWND hwnd);

// --------------------------------------
// ctrl - radio
// --------------------------------------
HWND createWndRadio(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str);
bool getWndRadio(HWND hwnd);
void setWndRadio(HWND hwnd, bool on);
void wndRadioUpdate(HWND hwnd);

// --------------------------------------
// ctrl - group box
// --------------------------------------
HWND createWndGroupBox(int posx, int posy, int width, int height, const char* str);

// --------------------------------------
// ctrl - edit
// --------------------------------------
#if 0
enum WndEditStyle {
	WndEditAll = 0,
	WndEditInt,
	WndEditFloat,
};

HWND createWndEdit(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, WndEditStyle style, const char* str, int strLengthMax);
HWND createWndEditMultiline(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str, int strLengthMax);

//눈으로만 봐두자...
typedef LRESULT(CALLBACK *EDIT_METHOD)(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK editAllWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK editIntWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK editFloatWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK editWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, WndEditStyle style);
LRESULT CALLBACK editMultilineWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, WndEditStyle style);
#endif

// --------------------------------------
// common(모든 HWND에 공통적으로 사용)
// --------------------------------------
void enableWnd(HWND hwnd, bool enable); //컨트롤러의 활성/비활성화
void setWndText(HWND hwnd, const char* szFormat, ...);
char* getWndText(HWND hwnd);
int getWndInt(HWND hwnd);
float getWndFloat(HWND hwnd);

//***************************
// utf
//***************************
wchar_t* utf8_to_utf16(const char* szFormat, ...);
char* utf16_to_utf8(const wchar_t* str);