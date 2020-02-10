#include "iWindow.h"

HINSTANCE g_hInst;	// 현재 프로그램의 메인 인스턴스입니다.
HWND g_hWnd;
HDC g_hDC;


//***************************
// WndCtrlSystem
//***************************
WndCtrlSystem* wcs;

#define MAX 1000
WndCtrlSystem::WndCtrlSystem()
{
	showLog("WndCtrlSystem()");
	wc = (WndCtrl*)malloc(sizeof(WndCtrl)*MAX);
	wndCtrlCount = 0;
}
WndCtrlSystem::~WndCtrlSystem()
{
	free(wc);
}

void WndCtrlSystem::addWndCtrl(HWND hwnd, WndStyle style, WND_COLOR_METHOD colorMethod, WND_UPDATE_METHOD updateMethod, int wndID)
{
	//WndCtrl* wc = &(wcs->wc[wndID]);
	WndCtrl* _wc = (wcs->wc);
	WndCtrl* wc = _wc + wndID;
	wcs->wndCtrlCount++;

	wc->hwnd = hwnd;
	wc->style = style;
	wc->methodColor = colorMethod;
	wc->methodUpdate = updateMethod;
	wc->wndID = wndID;

	printf("%d번째 컨트롤러 등록 성공\n", wndID);
}

void WndCtrlSystem::updateWndCtrl(WPARAM wParam, LPARAM lParam)
{
	int wndID = LOWORD(wParam);

	WndCtrl* wc = &(wcs->wc[wndID]);
	switch (wc->style)
	{
	case WndStatic:
	{
		break;
	}
	case WndButton:
	{
		if (wc->methodUpdate != NULL)
			wc->methodUpdate(wc->hwnd);
		break;
	}
	case WndCheckBox:
	{
		bool isChecked = getCheckBox(wc->hwnd);
		setCheckBox(wc->hwnd, !isChecked);

		if (wc->methodUpdate != NULL)
			wc->methodUpdate(wc->hwnd);
		break;
	}
	case WndComboBox:
	{
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			if (wc->methodUpdate)
				wc->methodUpdate(wc->hwnd);
		}
		break;
	}
	case WndListBox:
	{
		switch (HIWORD(wParam)) 
		{
		//LBN_SELCHANGE : 사용자에 의해 선택이 변경되었다.
		case LBN_SELCHANGE:
			if (wc->methodUpdate)
				wc->methodUpdate(wc->hwnd);
			break;
		//LBN_DBLCLK : 리스트 박스를 더블클릭하였다.
		case LBN_DBLCLK:
			//MessageBox(NULL, TEXT("더블클릭"), TEXT("리스트박스"), MB_OK);
			break;
		}
		break;
	}
	case WndRadio:
	{
		if (wc->methodUpdate != NULL)
		{
			wc->methodUpdate(wc->hwnd);
		}
		break;
	}
	case WndEdit:
	{
		switch (HIWORD(wParam)) {
			case EN_MAXTEXT:
			{
				//printf("ID_EDIT-EN_MAXTEXT\n");
				break;
			}
			case EN_UPDATE:
			{
				//wchar_t str[1024];
				//int length = GetWindowTextLength(wc->hWnd) + 1;
				//GetWindowText(c->hWnd, str, length);
				//printf("ID_EDIT-EN_UPDATE[%s]\n", str);
				break;
			}
			case EN_CHANGE:
			{
				if (wc->methodUpdate)
					wc->methodUpdate(wc->hwnd);
				break;
			}
			case EN_KILLFOCUS:
			{
				if (wc->methodUpdate)
					wc->methodUpdate(wc->hwnd);

				//char* s = getWndText(c->hWnd);
				//wchar_t* wstr = utf8_to_utf16(s);
				//MessageBox(NULL, wstr, TEXT("에딧"), MB_OK);
				//free(wstr);
				break;
			}
		}
		break;
	}
	default:
		break;
	}
}

//---------------------------
// ctrl - static
//---------------------------
HWND createWndStatic(int posx, int posy, int width, int height, 
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str)
{
	if (str == NULL)
		str = "타이틀이 없습니다.";

	wchar_t* title = utf8_to_utf16(str);
	int wndID = wcs->wndCtrlCount;

	HWND hwnd = CreateWindow(WC_STATIC, title,
		WS_CHILD | WS_VISIBLE | ES_CENTER,// ES_CENTER, ES_RIGHT
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);
	free(title);

	wcs->addWndCtrl(hwnd, WndStatic, methodColor, methodUpdate, wndID);

	return hwnd;
}

// --------------------------------------
// ctrl - button
// --------------------------------------
HWND createWndButton(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str)
{
	if (str == NULL)
		str = "타이틀이 없습니다.";

	wchar_t* title = utf8_to_utf16(str);
	int wndID = wcs->wndCtrlCount;

	HWND hwnd = CreateWindow(WC_BUTTON, title,
		WS_TABSTOP |WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);
	free(title);

	wcs->addWndCtrl(hwnd, WndButton, methodColor, methodUpdate, wndID);

	return hwnd;
}

void wndButtonUpdate(HWND hwnd)
{
	showLog("wndButtonUpdate()");
}

// --------------------------------------
// ctrl - check box
// --------------------------------------
HWND createWndCheckBox(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str)
{
	if (str == NULL)
		str = "타이틀이 없습니다.";

	wchar_t* title = utf8_to_utf16(str);
	int wndID = wcs->wndCtrlCount;

	HWND hwnd = CreateWindow(WC_BUTTON, title,
		WS_TABSTOP |WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);
	free(title);

	setCheckBox(hwnd, false); //처음 생성하면 체크를 꺼준다..
	wcs->addWndCtrl(hwnd, WndCheckBox, methodColor, methodUpdate, wndID);

	return hwnd;
}
bool getCheckBox(HWND hwnd)
{
	//BM_GETCHECK : 체크 박스가 현재 체크되어 있는 상태인지를 조사
	return SendMessage(hwnd, (UINT)BM_GETCHECK, (WPARAM)0, (LPARAM)0);
}
void setCheckBox(HWND hwnd, bool isCheck)
{
	//BM_SETCHECK : 체크 박스의 체크 상태를 변경하며 wParam에 변경할 체크 상태를 보내주면 된다.
	SendMessage(hwnd, BM_SETCHECK, isCheck ? BST_CHECKED : BST_UNCHECKED, 0);
}

void wndCheckBoxUpdate(HWND hwnd)
{
	showLog("wndCheckBoxUpdate()");
}

// --------------------------------------
// ctrl - combo box
// --------------------------------------
/*콤보박스 만들때 세로 크기를 충분히 크게 해줘야 리스트가 보인다!!!*/
HWND createWndComboBox(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char** str, int strNum)
{
	int wndID = wcs->wndCtrlCount;

	HWND hwnd = CreateWindow(WC_COMBOBOX, NULL,
		WS_TABSTOP |WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN | CBS_HASSTRINGS,
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);

	addWndComboBox(hwnd, str, strNum);
	setWndComboBox(hwnd, strNum - 1);
	wcs->addWndCtrl(hwnd, WndComboBox, methodColor, methodUpdate, wndID);

	return hwnd;
}
void addWndComboBox(HWND hwnd, int index, const char* str)
{
	wchar_t* s = utf8_to_utf16(str);
	SendMessage(hwnd, CB_ADDSTRING, (WPARAM)index, (LPARAM)s);
	free(s);
}
void addWndComboBox(HWND hwnd, const char** str, int strNum)
{
	for (int i = 0; i < strNum; i++)
		addWndComboBox(hwnd, i, str[i]);
}
void removeWndComboBox(HWND hwnd, int index)
{
	SendMessage(hwnd, CB_DELETESTRING, (WPARAM)index, (LPARAM)0);
}
int indexWndComboBox(HWND hwnd)
{
	int index = SendMessage(hwnd, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	return index;
}
int countWndComboBox(HWND hwnd)
{
	return SendMessage(hwnd, CB_GETCOUNT, 0, 0);
}
char* getWndComboBox(HWND hwnd, int index)
{
	wchar_t wstr[128];
	SendMessage(hwnd, CB_GETLBTEXT, index, (LPARAM)wstr);
	return utf16_to_utf8(wstr);
}
void setWndComboBox(HWND hwnd, int index)
{
	SendMessage(hwnd, (UINT)CB_SETCURSEL, (WPARAM)index, (LPARAM)0);
}

void wndComboBoxUpdate(HWND hwnd)
{
	showLog("wndComboBoxUpdate");
}

// --------------------------------------
// ctrl - list box
// --------------------------------------
HWND createWndListBox(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char** str, int strNum)
{
	int wndID = wcs->wndCtrlCount;

	HWND hwnd = CreateWindow(WC_LISTBOX, NULL,
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_NOTIFY | WS_HSCROLL | WS_VSCROLL,
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);

	addWndListBox(hwnd, str, strNum);
	setWndListBox(hwnd, strNum - 1);
	wcs->addWndCtrl(hwnd, WndListBox, methodColor, methodUpdate, wndID);
	return hwnd;
}
void addWndListBox(HWND hwnd, int index, const char* str)
{
	wchar_t* s = utf8_to_utf16(str);
	SendMessage(hwnd, LB_ADDSTRING, (WPARAM)index, (LPARAM)s);
	//SendMessage(hwnd, LB_INSERTSTRING, (WPARAM)index, (LPARAM)s);
	free(s);
}

void addWndListBox(HWND hwnd, const char** str, int strNum)
{
	for (int i = 0; i < strNum; i++)
		addWndListBox(hwnd, strNum, str[i]);
}

void removeWndListBox(HWND hwnd, int index)
{
	SendMessage(hwnd, LB_DELETESTRING, (WPARAM)index, (LPARAM)0);
}

int indexWndListBox(HWND hwnd)
{
	int index = SendMessage(hwnd, (UINT)LB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	return index;
}

int countWndListBox(HWND hwnd)
{
	return SendMessage(hwnd, LB_GETCOUNT, 0, 0);
}

char* getWndListBox(HWND hwnd, int index)
{
	wchar_t wstr[128];
	SendMessage(hwnd, LB_GETTEXT, index, (LPARAM)wstr);
	return utf16_to_utf8(wstr);
}

void setWndListBox(HWND hwnd, int index)
{
	SendMessage(hwnd, (UINT)LB_SETCURSEL, (WPARAM)index, (LPARAM)0);
}

void wndListBoxUpdate(HWND hwnd)
{
	showLog("wndListBoxUpdate");
}

// --------------------------------------
// ctrl - radio
// --------------------------------------
HWND createWndRadio(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str)
{
	int wndID = wcs->wndCtrlCount;

	wchar_t* title = NULL;
	if (str) title = utf8_to_utf16(str);

	HWND hwnd = CreateWindow(WC_BUTTON, title,
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);

	if (title) free(title);

	setWndRadio(hwnd, false);
	wcs->addWndCtrl(hwnd, WndRadio, methodColor, methodUpdate, wndID);
	return hwnd;
}

bool getWndRadio(HWND hwnd)
{
	return SendMessage(hwnd, (UINT)BM_GETCHECK, (WPARAM)0, (LPARAM)0);
}

void setWndRadio(HWND hwnd, bool on)
{
	SendMessage(hwnd, BM_SETCHECK, on ? BST_CHECKED : BST_UNCHECKED, 0);
}

void wndRadioUpdate(HWND hwnd)
{
	bool isChecked = getWndRadio(hwnd);
	setWndRadio(hwnd, !isChecked);
	showLog("wndRadioUpdate");
}

// --------------------------------------
// ctrl - group box
// --------------------------------------
HWND createWndGroupBox(int posx, int posy, int width, int height, const char* str)
{
	int wndID = wcs->wndCtrlCount;

	wchar_t* s = utf8_to_utf16(str);
	HWND hwnd = CreateWindow(WC_BUTTON, s,
		WS_CHILD | WS_VISIBLE | BS_GROUPBOX,
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);	
	free(s);

	wcs->addWndCtrl(hwnd, WndButton, NULL, NULL, wndID);
	return hwnd;
}

// --------------------------------------
// ctrl - edit
// --------------------------------------
#if 0
HWND createWndEdit(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, WndEditStyle style, const char* str, int strLengthMax)
{
	int wndID = wcs->wndCtrlCount;

	wchar_t* s = utf8_to_utf16(str);
	HWND hwnd = CreateWindow(WC_EDIT, s,
		WS_TABSTOP |
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_CENTER,
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);		
	free(s);

	//EM_LIMITTEXT : 글자 수를 제한
	if (strLengthMax)
		SendMessage(hwnd, (UINT)EM_LIMITTEXT, (WPARAM)strLengthMax, (LPARAM)0);

	wcs->addWndCtrl(hwnd, WndEdit, methodColor, methodUpdate, wndID);

	SetWindowLongPtr(hwnd, GWLP_USERDATA, GetWindowLongPtr(hwnd, GWLP_WNDPROC)); //윈도우 속성 설정
	EDIT_METHOD method[3] = { editAllWndProc, editIntWndProc, editFloatWndProc };
	SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)method[style]);

	return hwnd;
}
HWND createWndEditMultiline(int posx, int posy, int width, int height,
	WND_COLOR_METHOD methodColor, WND_UPDATE_METHOD methodUpdate, const char* str, int strLengthMax)
{
	int wndID = wcs->wndCtrlCount;

	wchar_t* s = utf8_to_utf16(str);
	HWND hwnd = CreateWindow(WC_EDIT, s,
		WS_TABSTOP |
		WS_CHILD | WS_VISIBLE | WS_BORDER |
		ES_MULTILINE | ES_WANTRETURN | ES_NOHIDESEL | ES_AUTOVSCROLL | WS_VSCROLL,
		posx, posy, width, height, g_hWnd, (HMENU)wndID, g_hInst, NULL);
	free(s);

	if (strLengthMax)
		SendMessage(hwnd, (UINT)EM_LIMITTEXT, (WPARAM)strLengthMax, (LPARAM)0);

	wcs->addWndCtrl(hwnd, WndEdit, methodColor, methodUpdate, wndID);

	SetWindowLongPtr(hwnd, GWLP_USERDATA, GetWindowLongPtr(hwnd, GWLP_WNDPROC));
	SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)editMultilineWndProc);

	return hwnd;
}

LRESULT CALLBACK editAllWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return editWndProc(hwnd, msg, wParam, lParam, WndEditAll);
}
LRESULT CALLBACK editIntWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return editWndProc(hwnd, msg, wParam, lParam, WndEditInt);
}
LRESULT CALLBACK editFloatWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	return editWndProc(hwnd, msg, wParam, lParam, WndEditFloat);
}

LRESULT CALLBACK editWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, WndEditStyle style)
{
	WNDPROC wpOld = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (wpOld)
	{
		switch (msg) {

		// 에딧 컨트롤 꾸미기(굳이 할 필요없음. 그냥 공부용)
		case WM_ERASEBKGND:
		{
			RECT rc;
			if (GetClientRect(hwnd, &rc))
			{
				INT nW = (rc.right - rc.left);
				INT nH = (rc.bottom - rc.top);
				TRIVERTEX triVertex[5] = {
					{  0, nH,    0xFF00, 0xFF00, 0xFF00, 0x0000 },
					{ nW, nH / 2, 0xFF00, 0xFF00, 0xFF00, 0x0000 },
					{ nW, nH,    0xBB00, 0xDD00, 0xF700, 0x0000 },
				};
				GRADIENT_TRIANGLE triMesh = { 0, 1, 2 };
				HDC hdc = (HDC)wParam;
				INT ndc = SaveDC(hdc);
				SetBkColor(hdc, RGB(255, 255, 255));
				ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
				GradientFill(hdc, triVertex, 3, &triMesh, 1, GRADIENT_FILL_TRIANGLE);
				RestoreDC(hdc, ndc);
				return 0;
			}
			break;
		}

		case WM_NCDESTROY:
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)wpOld);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
			break;

		case WM_CHAR:
			// VK_DELETE 먹히지 않는다.
			if (style == WndEditAll)
			{
				// ok
			}
			else if (style == WndEditInt)
			{
				if (wParam == VK_RETURN || wParam == VK_BACK ||
					wParam == '-' || (wParam >= '0' && wParam <= '9'))
				{
					// ok
				}
				else
					return 0;/////////////////////////////
			}
			else// if (style == WndEditFloat)
			{
				if (wParam == '.')
				{
					wchar_t str[1024];
					int length = GetWindowTextLength(hwnd) + 1;
					GetWindowText(hwnd, str, length);
					for (int i = 0; i < length; i++)
					{
						if (str[i] == '.')
							return 0;////////////////////////
					}
				}
				else if (wParam == VK_RETURN || wParam == VK_BACK ||
					wParam == '-' || (wParam >= '0' && wParam <= '9'))
				{
					// ok
				}
				else
					return 0;/////////////////////////////
			}
			break;
		}
	}

	return CallWindowProc(wpOld, hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK editMultilineWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, WndEditStyle style)
{
	WNDPROC wpOld = (WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (wpOld)
	{
		switch (msg) {

			// 에딧 컨트롤 꾸미기(굳이 할 필요없음. 그냥 공부용)
		case WM_ERASEBKGND:
		{
			RECT rc;
			if (GetClientRect(hwnd, &rc))
			{
				INT nW = (rc.right - rc.left);
				INT nH = (rc.bottom - rc.top);
				TRIVERTEX triVertex[5] = {
					{  0, nH,    0xFF00, 0xFF00, 0xFF00, 0x0000 },
					{ nW, nH / 2, 0xFF00, 0xFF00, 0xFF00, 0x0000 },
					{ nW, nH,    0xBB00, 0xDD00, 0xF700, 0x0000 },
				};
				GRADIENT_TRIANGLE triMesh = { 0, 1, 2 };
				HDC hdc = (HDC)wParam;
				INT ndc = SaveDC(hdc);
				SetBkColor(hdc, RGB(255, 255, 255));
				ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
				GradientFill(hdc, triVertex, 3, &triMesh, 1, GRADIENT_FILL_TRIANGLE);
				RestoreDC(hdc, ndc);
				return 0;
			}
			break;
		}

		case WM_NCDESTROY:
			SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)wpOld);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
			break;

		case WM_GETDLGCODE:
		{
			LRESULT ret = CallWindowProc(wpOld, hwnd, msg, wParam, lParam);
			ret &= ~(DLGC_HASSETSEL | DLGC_WANTTAB);
			if (lParam &&
				((LPMSG)lParam)->message == WM_KEYDOWN &&
				((LPMSG)lParam)->wParam == VK_TAB)
				ret &= ~DLGC_WANTMESSAGE;
			return ret;
			//break;
		}
		}
	}

	return CallWindowProc(wpOld, hwnd, msg, wParam, lParam);
}
#endif

// --------------------------------------
// common(모든 HWND에 공통적으로 사용)
// --------------------------------------
void enableWnd(HWND hwnd, bool enable)
{
	EnableWindow(hwnd, enable);
}

void setWndText(HWND hwnd, const char* szFormat, ...)
{
	char szText[1024];
	va_list args;

	va_start(args, szFormat);
	_vsnprintf(szText, sizeof(szText), szFormat, args);
	va_end(args);

	wchar_t* s = utf8_to_utf16(szText);
	SetWindowText(hwnd, s);
	free(s);
}

char* getWndText(HWND hwnd)
{
	wchar_t str[1024];
	int length = GetWindowTextLength(hwnd) + 1;
	GetWindowText(hwnd, str, length);
	return utf16_to_utf8(str);
}

int getWndInt(HWND hwnd)
{
	char* txt = getWndText(hwnd);
	int n = atoi(txt);
	free(txt);
	return n;
}

float getWndFloat(HWND hwnd)
{
	char* txt = getWndText(hwnd);
	float f = atof(txt);
	free(txt);
	return f;
}

//***************************
// utf
//***************************
wchar_t* utf8_to_utf16(const char* szFormat, ...)
{
	char szText[1024];
	va_list args;

	va_start(args, szFormat);
	_vsnprintf(szText, sizeof(szText), szFormat, args);
	va_end(args);

	int length = MultiByteToWideChar(CP_ENCODE, 0, szText, -1, NULL, 0);
	wchar_t* ws = (wchar_t*)malloc(sizeof(wchar_t)*length);
	MultiByteToWideChar(CP_ENCODE, 0, szText, strlen(szText) + 1, ws, length);

	return ws;
}

char* utf16_to_utf8(const wchar_t* str)
{
	int length = WideCharToMultiByte(CP_ENCODE, 0, str, lstrlenW(str), NULL, 0, NULL, NULL);
	char* s = (char*)calloc(sizeof(char), 1 + length);
	WideCharToMultiByte(CP_ENCODE, 0, str, lstrlenW(str), s, length, NULL, NULL);

	return s;
}