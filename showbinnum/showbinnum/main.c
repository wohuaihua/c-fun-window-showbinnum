/*
		用于将数字转化为:
				2进制数，
				8进制数，
				10进制数,
				16进制数，
				补码，
				反码
				ASCII等。
		hah
*/

#pragma comment  (lib,"User32.lib")
#pragma comment  (lib,"Gdi32.lib")

#include <windows.h>
#include <limits.h>
#include <math.h>
#include "resource.h"

#define ID_EDIT 1

#define ID_RADIO_DEC 10
#define ID_RADIO_BIN 2
#define ID_RADIO_OCT 8
#define ID_RADIO_HEX 16
#define ID_RADIO_ASCII 128

#define ID_MSG 250

#define ID_BIN_VALUE 20
#define ID_OCT_VALUE 80
#define ID_DEC_VALUE 100
#define ID_HEX_VALUE 160
#define ID_ONE_VALUE 200
#define ID_TWO_VALUE 201
#define ID_ASCII_VALUE 202

#define ID_BIN_FRAME 1002
#define ID_OCT_FRAME 1008
#define ID_DEC_FRAME 1010
#define ID_HEX_FRAME 1016
#define ID_ONE_FRAME 1020
#define ID_TWO_FRAME 1021
#define ID_ASCII_FRAME 1022

/**		GLOBAL PARAMS			*/
static int checkType = 10;
static HWND hwnd;
static HWND hwndEdit;

static HWND hwndBinRadio;
static HWND hwndDecRadio;
static HWND hwndHexRadio;
static HWND hwndOctRadio;
static HWND hwndAsciiRadio;

static HWND hwndMsg;

struct WindowInfo
{
	LPCTSTR lpName;
	int id;
} showTexts[] = {
	TEXT("二进制:"), ID_BIN_FRAME, 
	NULL, ID_BIN_VALUE, 
	TEXT("八进制:"), ID_OCT_FRAME,
	NULL, ID_OCT_VALUE, 
	TEXT("十进制:"), ID_DEC_FRAME, 
	NULL, ID_DEC_VALUE, 
	TEXT("十六进制:"), ID_HEX_FRAME, 
	NULL, ID_HEX_VALUE,
	TEXT("补码:"), ID_ONE_FRAME,
	NULL, ID_ONE_VALUE,
	TEXT("反码:"), ID_TWO_FRAME,
	NULL, ID_TWO_VALUE,
	TEXT("ASCII:"), ID_ASCII_FRAME,
	NULL, ID_ASCII_VALUE
};

#define SHOW_NUM (sizeof showTexts / sizeof showTexts[0])

TCHAR  szEditBuffer[128];
TCHAR  szEditBufferCpy[128];
TCHAR  szBinBuffer[128];
TCHAR  szOTCBuffer[128];
TCHAR  szDECBuffer[128];
TCHAR  szHEXBuffer[128];
TCHAR  szONEBuffer[128];
TCHAR  szTWOBuffer[128];
TCHAR  szASCIIBuffer[128];

TCHAR szBinMsg[30] = TEXT("2进制只能填1或0");
TCHAR szOtcMsg[30] = TEXT("8进制只能填[0,7]");
TCHAR szDecMsg[30] = TEXT("10进制只能填[0,9]");
TCHAR szHexMsg[50] = TEXT("16进制只能填[0,9]或者[A,F]或者[a,f]");
TCHAR szAsciiMsg[50] = TEXT("需要正确的ASCII范围");
TCHAR szMaxMsg[50] = TEXT("10进制数字范围[-2147483648 , 2147483647]");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void noCheckRadio();

void showBinFrame(HWND, int, int);
void showOtcFrame(HWND, int, int);
void showDecFrame(HWND, int, int);
void showHexFrame(HWND, int, int);
void showOneFrame(HWND, int, int);
void showTwoFrame(HWND, int, int);
void showAssicFrame(HWND);

int radixToDec(int, TCHAR *);
long long radixToDecLL(int, TCHAR *);
int checkInput(int, HWND);
void cleanFrames(HWND);

//RELEASE WinMain 会有问题
int WINAPI wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
					PSTR szCmdLine,int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("calc bin");
	MSG msg;
	WNDCLASS wndclass;

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL,TEXT("This program requires Window NT"),szAppName,MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(szAppName,
						TEXT("hah"),
						WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX,
						CW_USEDEFAULT,
						CW_USEDEFAULT,
						450,
						600,
						NULL,
						NULL,
						hInstance,
						NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxChar, cyChar;
	static int cxClient, cyClient;
	static HBRUSH hBrushWhite,hBrushViolet;
	static HFONT hFont;
	HDC hdc;
	PAINTSTRUCT ps;
	RECT        rect;
	int i;
	static int isChangeType = FALSE;

	static HWND  hwndTexts[SHOW_NUM];
	int editId;
	switch (message)
	{
	case WM_CREATE:
		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());

		hBrushWhite = CreateSolidBrush(RGB(255, 255, 255));
		hBrushViolet = CreateSolidBrush(RGB(163, 73, 164));
		hFont = CreateFont(14, 0, 0, 0, 0, 0, 0, 0,
			GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH, "宋体");

		hwndEdit = CreateWindow(TEXT("edit"),
			NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT ,//ES_NUMBER，将编辑控件的输入限制为数字
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)ID_EDIT,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		SendMessage(hwndEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

		hwndBinRadio = CreateWindow(TEXT("button"),
			TEXT("b"),
			WS_CHILD|WS_VISIBLE|BS_RADIOBUTTON | BS_RIGHTBUTTON,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)ID_RADIO_BIN,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		SendMessage(hwndBinRadio, WM_SETFONT, (WPARAM)hFont, TRUE);

		hwndDecRadio = CreateWindow(TEXT("button"),
			TEXT("d"),
			WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_RIGHTBUTTON,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)ID_RADIO_DEC,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		SendMessage(hwndDecRadio, WM_SETFONT, (WPARAM)hFont, TRUE);

		hwndHexRadio = CreateWindow(TEXT("button"),
			TEXT("h"),
			WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_RIGHTBUTTON,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)ID_RADIO_HEX,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		SendMessage(hwndHexRadio, WM_SETFONT, (WPARAM)hFont, TRUE);

		hwndOctRadio = CreateWindow(TEXT("button"),
			TEXT("o"),
			WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_RIGHTBUTTON,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)ID_RADIO_OCT,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		SendMessage(hwndOctRadio, WM_SETFONT, (WPARAM)hFont, TRUE);

		hwndAsciiRadio = CreateWindow(TEXT("button"),
			TEXT("a"),
			WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | BS_RIGHTBUTTON,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)ID_RADIO_ASCII,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		SendMessage(hwndAsciiRadio, WM_SETFONT, (WPARAM)hFont, TRUE);

		hwndMsg = CreateWindow(TEXT("edit"),
			NULL,
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY,
			0,
			0,
			0,
			0,
			hwnd,
			(HMENU)ID_MSG,
			((LPCREATESTRUCT)lParam)->hInstance,
			NULL);
		SendMessage(hwndMsg, WM_SETFONT, (WPARAM)hFont, TRUE);
			
		for (i = 0; i < SHOW_NUM;i++)
		{
			hwndTexts[i] = CreateWindow(TEXT("edit"),
				showTexts[i].lpName,
				WS_CHILD | WS_VISIBLE | SS_LEFT | ES_READONLY,
				0, 0, 0, 0,
				hwnd,
				(HMENU)showTexts[i].id,
				((LPCREATESTRUCT)lParam)->hInstance,
				NULL);
			SendMessage(hwndTexts[i], WM_SETFONT, (WPARAM)hFont, TRUE);
		}

			
		switch (checkType)
		{
		case 10:
			SendMessage(hwndDecRadio, BM_SETCHECK, 1, 0);
			break;
		case 2:
			SendMessage(hwndBinRadio, BM_SETCHECK, 1, 0);
			break;
		case 8:
			SendMessage(hwndOctRadio, BM_SETCHECK, 1, 0);
			break;
			SendMessage(hwndHexRadio, BM_SETCHECK, 1, 0);
		case 16:
			break;
		}
		return 0;
	case WM_SETFOCUS:
		SetFocus(hwndEdit);
		return 0;
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(lParam);

		MoveWindow(hwndEdit,
			LOWORD(lParam)/24,
			10,
			LOWORD(lParam)/2,
			cxChar+12,
			TRUE);

		MoveWindow(hwndBinRadio,
			LOWORD(lParam)*14/24,
			10,
			LOWORD(lParam) / 12 - 10,
			cxChar + 12,
			TRUE);

		MoveWindow(hwndOctRadio,
			LOWORD(lParam) * 16 / 24,
			10,
			LOWORD(lParam) / 12 - 10,
			cxChar + 12,
			TRUE);
		
		MoveWindow(hwndDecRadio,
			LOWORD(lParam) * 18 / 24,
			10,
			LOWORD(lParam) / 12 - 10,
			cxChar + 12,
			TRUE);

		MoveWindow(hwndHexRadio,
			LOWORD(lParam) * 20 / 24,
			10,
			LOWORD(lParam) / 12 - 10,
			cxChar + 12,
			TRUE);

		MoveWindow(hwndAsciiRadio,
			LOWORD(lParam) * 22 / 24,
			10,
			LOWORD(lParam) / 12 - 10,
			cxChar + 12,
			TRUE);

		MoveWindow(hwndMsg,
			LOWORD(lParam) / 24,
			50,
			LOWORD(lParam) * 22 / 24,
			cxChar + 12,
			TRUE);

		for (i = 0; i < SHOW_NUM;i++)
		{
			if (i%2 != 0)
			{
				MoveWindow(hwndTexts[i],
					LOWORD(lParam) * 9 / 24,
					90 + (i-1) * cyChar,
					LOWORD(lParam) * 16 / 24,
					cxChar + 12,
					TRUE);
			}
			else
			{
				MoveWindow(hwndTexts[i],
					LOWORD(lParam) / 24,
					90 + i * cyChar,
					LOWORD(lParam) * 8 / 24,
					cxChar + 12,
					TRUE);
			}
		}

		return 0;
	case WM_CTLCOLOREDIT:
		// int editId ;  wrong!
		SetBkMode((HDC)wParam, TRANSPARENT);
		return (LRESULT)hBrushWhite;
	case WM_CTLCOLORSTATIC:
		//这样是有问题的
		//SetBkColor((HDC)wParam, RGB(255, 255, 255));
		SetBkMode((HDC)wParam, TRANSPARENT);
		SetTextColor((HDC)wParam, RGB(0, 0, 0));

		editId = (int)GetWindowLong((HWND)lParam, GWL_ID);
		if (editId == ID_MSG){
			return (LRESULT)hBrushViolet;
		}
		return (LRESULT)hBrushWhite;
	case WM_COMMAND:

		if (LOWORD(wParam) == ID_EDIT)
		{
			switch (HIWORD(wParam))
			{
			//EN_CHANGE 和 SetWindowText 会造成死循环
			case EN_CHANGE:
				//TODO 输入界限限定
				GetWindowText(hwndEdit, szEditBuffer, GetWindowTextLength(hwndEdit) + 1);
				if (lstrcmpW(&szEditBufferCpy,&szEditBuffer)!=0 && 
					GetWindowTextLength(hwndEdit)>0&&
					checkType == ID_RADIO_DEC && checkInput(NULL,hwndTexts))
				{	
					lstrcpyW(&szEditBufferCpy, &szEditBuffer);
					showBinFrame(hwndTexts, NULL, NULL);
					showOtcFrame(hwndTexts, NULL, NULL);
					showDecFrame(hwndTexts, NULL, NULL);
					showHexFrame(hwndTexts, NULL, NULL);
					showOneFrame(hwndTexts, NULL, NULL);
					showTwoFrame(hwndTexts, NULL, NULL);
					showAssicFrame(hwndTexts);
				}
				//TODO 输入界限限定
				if (lstrcmpW(&szEditBufferCpy, &szEditBuffer) != 0 &&
					GetWindowTextLength(hwndEdit)>0 &&
					checkType == ID_RADIO_BIN && checkInput(NULL, hwndTexts))
				{
					lstrcpyW(&szEditBufferCpy, &szEditBuffer);

					int decNum = radixToDec(2, szEditBuffer);
					showBinFrame(hwndTexts, decNum, NULL);
					showOtcFrame(hwndTexts, decNum, NULL);
					showDecFrame(hwndTexts, decNum, NULL);
					showHexFrame(hwndTexts, decNum, NULL);
					showOneFrame(hwndTexts, decNum, NULL);
					showTwoFrame(hwndTexts, decNum, NULL);
					showAssicFrame(hwndTexts);
					
				}

				if (lstrcmpW(&szEditBufferCpy, &szEditBuffer) != 0 &&
					GetWindowTextLength(hwndEdit)>0 &&
					checkType == ID_RADIO_OCT && checkInput(NULL, hwndTexts))
				{
					lstrcpyW(&szEditBufferCpy, &szEditBuffer);
					int decNum = radixToDec(8, &szEditBuffer);
					showBinFrame(hwndTexts, decNum, NULL);
					showOtcFrame(hwndTexts, decNum, NULL);
					showDecFrame(hwndTexts, decNum, NULL);
					showHexFrame(hwndTexts, decNum, NULL);
					showOneFrame(hwndTexts, decNum, NULL);
					showTwoFrame(hwndTexts, decNum, NULL);
					showAssicFrame(hwndTexts);
				}
				if (lstrcmpW(&szEditBufferCpy, &szEditBuffer) != 0 &&
					GetWindowTextLength(hwndEdit)>0 &&
					checkType == ID_RADIO_HEX && checkInput(NULL, hwndTexts))
				{
					lstrcpyW(&szEditBufferCpy, &szEditBuffer);
					int decNum = radixToDec(16, &szEditBuffer);
					showBinFrame(hwndTexts, decNum, NULL);
					showOtcFrame(hwndTexts, decNum, NULL);
					showDecFrame(hwndTexts, decNum, NULL);
					showHexFrame(hwndTexts, decNum, NULL);
					showOneFrame(hwndTexts, decNum, NULL);
					showTwoFrame(hwndTexts, decNum, NULL);
					showAssicFrame(hwndTexts);
				}

				if (lstrcmpW(&szEditBufferCpy, &szEditBuffer) != 0 &&
					GetWindowTextLength(hwndEdit)>0 &&
					checkType == ID_RADIO_ASCII && checkInput(NULL, hwndTexts))
				{
					cleanFrames(hwndTexts);
					showAssicFrame(hwndTexts);
				}

				if (lstrlenW(szEditBuffer) == 0)
				{
					lstrcpyW(&szEditBufferCpy, &szEditBuffer);
					cleanFrames(hwndTexts);
				}

			}
		}

		switch (LOWORD(wParam))
		{
		case ID_RADIO_BIN:
			if (checkType != ID_RADIO_BIN)
			{
				isChangeType = TRUE;
				if (isChangeType && GetWindowTextLength(hwndEdit)>0 && checkInput(2, hwndTexts))
				{
					isChangeType = FALSE;
					int decNum = radixToDec(2, szEditBuffer);
					showBinFrame(hwndTexts, decNum, 2);
					showOtcFrame(hwndTexts, decNum, 2);
					showDecFrame(hwndTexts, decNum, 2);
					showHexFrame(hwndTexts, decNum, 2);
					showOneFrame(hwndTexts, decNum, 2);
					showTwoFrame(hwndTexts, decNum, 2);
					showAssicFrame(hwndTexts);

				}
			}
			checkType = 2;
			break;
		case ID_RADIO_OCT:
			if (checkType != ID_RADIO_OCT)
			{
				isChangeType = TRUE;
				if (isChangeType && GetWindowTextLength(hwndEdit)>0 && checkInput(ID_RADIO_OCT, hwndTexts))
				{
					isChangeType = FALSE;
					int decNum = radixToDec(8, szEditBuffer);
					showBinFrame(hwndTexts, decNum, 8);
					showOtcFrame(hwndTexts, decNum, 8);
					showDecFrame(hwndTexts, decNum, 8);
					showHexFrame(hwndTexts, decNum, 8);
					showOneFrame(hwndTexts, decNum, 8);
					showTwoFrame(hwndTexts, decNum, 8);
					showAssicFrame(hwndTexts);

				}
			}
			checkType = ID_RADIO_OCT;
			break;
		case ID_RADIO_DEC:
			if (checkType != ID_RADIO_DEC)
			{
				isChangeType = TRUE;
				if (isChangeType && GetWindowTextLength(hwndEdit)>0 && checkInput(ID_RADIO_DEC, hwndTexts))
				{
					isChangeType = FALSE;
					int decNum = radixToDec(10, szEditBuffer);
					showBinFrame(hwndTexts, decNum, 10);
					showOtcFrame(hwndTexts, decNum, 10);
					showDecFrame(hwndTexts, decNum, 10);
					showHexFrame(hwndTexts, decNum, 10);
					showOneFrame(hwndTexts, decNum, 10);
					showTwoFrame(hwndTexts, decNum, 10);
					showAssicFrame(hwndTexts);

				}
			}
			checkType = ID_RADIO_DEC;
			break;
		case ID_RADIO_HEX:
			if (checkType != ID_RADIO_HEX)
			{
				isChangeType = TRUE;
				if (isChangeType && GetWindowTextLength(hwndEdit)>0 && checkInput(ID_RADIO_HEX, hwndTexts))
				{
					isChangeType = FALSE;
					int decNum = radixToDec(16, szEditBuffer);
					showBinFrame(hwndTexts, decNum, 16);
					showOtcFrame(hwndTexts, decNum, 16);
					showDecFrame(hwndTexts, decNum, 16);
					showHexFrame(hwndTexts, decNum, 16);
					showOneFrame(hwndTexts, decNum, 16);
					showTwoFrame(hwndTexts, decNum, 16);
					showAssicFrame(hwndTexts);

				}
			}
			checkType = ID_RADIO_HEX;
			break;

		case ID_RADIO_ASCII:
			if (checkType != ID_RADIO_ASCII)
			{
				isChangeType = TRUE;
				if (isChangeType && GetWindowTextLength(hwndEdit)>0 && checkInput(ID_RADIO_ASCII, hwndTexts))
				{
					cleanFrames(hwndTexts);
					showAssicFrame(hwndTexts);

				}
			}
			checkType = ID_RADIO_ASCII;
			break;
		}
		noCheckRadio();
		SendMessage((HWND)lParam, BM_SETCHECK, 1, 0);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		SelectObject(hdc,hBrushViolet);
		GetClientRect(hwnd, &rect);
		Rectangle(hdc, (rect.right - rect.left) / 24 - 5, 
						50 - 5, 
						(rect.right - rect.left) * 23 / 24 + 5, 
						50 + cxChar + 12) + 5;
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		DeleteObject(hBrushWhite);
		DeleteObject(hBrushViolet);
		DeleteObject(hFont);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

void noCheckRadio()
{
	switch (checkType)
	{
	case ID_RADIO_BIN:
		SendMessage(hwndOctRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndDecRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndHexRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndAsciiRadio, BM_SETCHECK, 0, 0);
		break;
	case ID_RADIO_OCT:
		SendMessage(hwndBinRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndDecRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndHexRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndAsciiRadio, BM_SETCHECK, 0, 0);
		break;
	case ID_RADIO_DEC:
 		SendMessage(hwndOctRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndBinRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndHexRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndAsciiRadio, BM_SETCHECK, 0, 0);
		break;
	case ID_RADIO_HEX:
		SendMessage(hwndOctRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndDecRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndBinRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndAsciiRadio, BM_SETCHECK, 0, 0);
		break;
	case ID_RADIO_ASCII:
		SendMessage(hwndBinRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndOctRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndDecRadio, BM_SETCHECK, 0, 0);
		SendMessage(hwndBinRadio, BM_SETCHECK, 0, 0);
		break;
	}
}

/**
	显示2进制Frame
	decNum 代表所表示的10进制
*/
void showBinFrame(HWND hwndTexts[], int decNum,int change)
{
	if ((checkType == 2 && change == NULL) || change == 2)
	{
		SetWindowText(hwndTexts[1], szEditBuffer);
	}
	if ((checkType == 8 && change == NULL) || change == 8)
	{
		_itow(decNum, &szBinBuffer, 2);
		SetWindowText(hwndTexts[1], szBinBuffer);
	}
	if ((checkType == 10 && change == NULL) || change == 10)
	{
		_itow(_wtoi(szEditBuffer), &szBinBuffer, 2);
		SetWindowText(hwndTexts[1], szBinBuffer);
	}
	if ((checkType ==16 && change == NULL) || change == 16)
	{
		_itow(decNum, &szBinBuffer, 2);
		SetWindowText(hwndTexts[1], szBinBuffer);
	}
}

/**
	显示8进制Frame
	decNum 代表所表示的10进制
*/
void showOtcFrame(HWND hwndTexts[], int decNum, int change)
{
	if ((checkType == 2 && change == NULL) || change == 2)
	{
		_itow(decNum, &szOTCBuffer, 8);
		SetWindowText(hwndTexts[3], szOTCBuffer);
	}
	if ((checkType == 8 && change == NULL) || change == 8)
	{
		SetWindowText(hwndTexts[3], szEditBuffer);
	}
	if ((checkType == 10 && change == NULL) || change == 10)
	{
		_itow(_wtoi(szEditBuffer), &szOTCBuffer, 8);
		SetWindowText(hwndTexts[3], szOTCBuffer);
	}
	if ((checkType == 16 && change == NULL) || change == 16)
	{
		_itow(decNum, &szOTCBuffer, 8);
		SetWindowText(hwndTexts[3], szOTCBuffer);
	}
}

/**
	显示10进制Frame
	decNum 代表所表示的10进制
*/
void showDecFrame(HWND hwndTexts[], int decNum, int change)
{
	if ((checkType == 2 && change == NULL) || change == 2)
	{
		_itow(decNum, &szDECBuffer, 10);
		SetWindowText(hwndTexts[5], szDECBuffer);
	}
	if ((checkType == 8 && change == NULL) || change == 8)
	{
		_itow(decNum, &szDECBuffer, 10);
		SetWindowText(hwndTexts[5], szDECBuffer);
	}
	if ((checkType == 10 && change == NULL) || change == 10)
	{
		SetWindowText(hwndTexts[5], szEditBuffer);
	}
	if ((checkType == 16 && change == NULL) || change == 16)
	{
		_itow(decNum, &szDECBuffer, 10);
		SetWindowText(hwndTexts[5], szDECBuffer);
	}
}

/**
	显示16进制Frame
	decNum 代表所表示的10进制
*/
void showHexFrame(HWND hwndTexts[], int decNum, int change)
{
	if ((checkType == 2 && change == NULL) || change == 2)
	{
		_itow(decNum, &szHEXBuffer, 16);
		SetWindowText(hwndTexts[7], szHEXBuffer);
	}
	if ((checkType == 8 && change == NULL) || change == 8)
	{
		_itow(decNum, &szHEXBuffer, 16);
		SetWindowText(hwndTexts[7], szHEXBuffer);
	}
	if ((checkType == 10 && change == NULL) || change == 10)
	{
		_itow(_wtoi(szEditBuffer), &szHEXBuffer, 16);
		SetWindowText(hwndTexts[7], szHEXBuffer);
	}
	if ((checkType ==16 && change == NULL) || change == 16)
	{
		SetWindowText(hwndTexts[7], szEditBuffer);
	}
}

/**
	显示补码Frame
	decNum 代表所表示的10进制
*/
void showOneFrame(HWND hwndTexts[], int decNum, int change)
{
	if ((checkType == 2 && change == NULL) || (checkType == 8 && change == NULL) || (checkType == 16 && change == NULL) || change == 2 || change == 8 || change == 16)
	{
		TCHAR twoTmp[128];
		wsprintf(twoTmp, TEXT("%d"), ((~decNum) + 1));
		_itow(_wtoi(twoTmp), &szONEBuffer, 2);
		SetWindowText(hwndTexts[9], szONEBuffer);
	}
	
	if ((checkType ==10 && change == NULL) || change == 10)
	{
		int editNum = _wtoi(szEditBuffer);
		TCHAR twoTmp[128];
		wsprintf(twoTmp, TEXT("%d"), ((~editNum) + 1));
		_itow(_wtoi(twoTmp), &szONEBuffer, 2);
		SetWindowText(hwndTexts[9], szONEBuffer);
	}

}

/**
	显示反码Frame
	decNum 代表所表示的10进制
*/
void showTwoFrame(HWND hwndTexts[], int decNum, int change)
{
	if ((checkType == 2 && change == NULL) || (checkType == 8 && change == NULL) || (checkType == 16 && change == NULL)  || change == 2 || change == 8 || change == 16)
	{
		TCHAR oneTmp[128];
		wsprintf(oneTmp, TEXT("%d"), ~decNum);
		_itow(_wtoi(oneTmp), &szTWOBuffer, 2);
		SetWindowText(hwndTexts[11], szTWOBuffer);
	}

	if ((checkType == 10 && change == NULL) || change == 10)
	{
		int editNum = _wtoi(szEditBuffer);
		TCHAR oneTmp[128];
		// 4个字节
		// 00000000 00000000 00000000 00000001
		// 11111111 11111111 11111111 11111110
		wsprintf(oneTmp, TEXT("%d"), ~editNum);
		_itow(_wtoi(oneTmp), &szTWOBuffer, 2);
		SetWindowText(hwndTexts[11], szTWOBuffer);
	}

}


/*
	显示ASCII的frame
*/
void showAssicFrame(HWND hwndTexts[])
{
	ZeroMemory(&szASCIIBuffer, 128 * sizeof(TCHAR));
	int index = 0;
	int i;
	for (i = 0; i<lstrlenW(szEditBuffer); i++)
	{
		TCHAR asciiOneTmp[4];
		ZeroMemory(&asciiOneTmp, 4 * sizeof(TCHAR));
		wsprintf(asciiOneTmp, TEXT("%d"), szEditBuffer[i]);
		int j;
		for (j = 0; j<lstrlenW(asciiOneTmp); j++)
		{
			szASCIIBuffer[index] = asciiOneTmp[j];
			index++;
		}
		szASCIIBuffer[index] = ' ';
		index++;
	}
	SetWindowText(hwndTexts[13], szASCIIBuffer);
}

/*
	将2,8,16进制转化为10进制
*/
int radixToDec(int radix, TCHAR * szEditBuffer)
{
	int binLength = lstrlenW(szEditBuffer);
	int decNum = 0;
	int i;
	for (i = 0; i < binLength; i++)
	{
		//如果是TCHAR prefixCharNum[1],末尾\0 可能被改写 。
		TCHAR prefixCharNum[2];
		ZeroMemory(prefixCharNum, 2 * sizeof(TCHAR));
		wsprintf(prefixCharNum, TEXT("%c"), szEditBuffer[i]);
		int prefixNum = _wtoi(&prefixCharNum);
		if (prefixCharNum[0] == 'a' || prefixCharNum[0] == 'A')
			prefixNum = 10;
		if (prefixCharNum[0] == 'b' || prefixCharNum[0] == 'B')
			prefixNum = 11;
		if (prefixCharNum[0] == 'c' || prefixCharNum[0] == 'C')
			prefixNum = 12;
		if (prefixCharNum[0] == 'd' || prefixCharNum[0] == 'D')
			prefixNum = 13;
		if (prefixCharNum[0] == 'e' || prefixCharNum[0] == 'E')
			prefixNum = 14;
		if (prefixCharNum[0] == 'f' || prefixCharNum[0] == 'F')
			prefixNum = 15;
		decNum += prefixNum * pow(radix, binLength - i - 1);
	}
	return decNum;
}

long long radixToDecLL(int radix, TCHAR * szEditBuffer)
{
	int binLength = lstrlenW(szEditBuffer);
	long long decNum = 0;
	int i;
	for (i = 0; i < binLength; i++)
	{
		//如果是TCHAR prefixCharNum[1],末尾\0 可能被改写 。
		TCHAR prefixCharNum[2];
		ZeroMemory(prefixCharNum, 2 * sizeof(TCHAR));
		wsprintf(prefixCharNum, TEXT("%c"), szEditBuffer[i]);
		int prefixNum = _wtoi(&prefixCharNum);
		if (prefixCharNum[0] == 'a' || prefixCharNum[0] == 'A')
			prefixNum = 10;
		if (prefixCharNum[0] == 'b' || prefixCharNum[0] == 'B')
			prefixNum = 11;
		if (prefixCharNum[0] == 'c' || prefixCharNum[0] == 'C')
			prefixNum = 12;
		if (prefixCharNum[0] == 'd' || prefixCharNum[0] == 'D')
			prefixNum = 13;
		if (prefixCharNum[0] == 'e' || prefixCharNum[0] == 'E')
			prefixNum = 14;
		if (prefixCharNum[0] == 'f' || prefixCharNum[0] == 'F')
			prefixNum = 15;
		decNum += prefixNum * pow(radix, binLength - i - 1);
	}
	return decNum;
}

/**
	用于检查输入
*/
int checkInput(int type, HWND hwndTexts[])
{
	int length = lstrlenW(szEditBuffer);
	int isOk = TRUE;
	int i;
	int record = checkType;

	if (type != NULL)
	{
		checkType = type;
	}

	if (checkType == 2)
	{
		long long decNum = radixToDecLL(2, szEditBuffer);
		//数字范围
		if (decNum > INT_MAX || decNum < INT_MIN)
		{
			isOk = FALSE;
			SetWindowText(hwndMsg, szMaxMsg);
		}
		for (i = 0; i < length;i++)
		{	
			//0,1 ascii
			if (!(48 <= (int)szEditBuffer[i] && (int)szEditBuffer[i] <= 49))
			{
				isOk = FALSE;
				SetWindowText(hwndMsg, szBinMsg);
			}
		}
	}

	if (checkType == 8)
	{	
		long long decNum = radixToDecLL(8, szEditBuffer);
		//数字范围
		if (decNum > INT_MAX || decNum < INT_MIN)
		{
			isOk = FALSE;
			SetWindowText(hwndMsg, szMaxMsg);
		}
		for (i = 0; i < length; i++)
		{
			if (!(48 <= (int)szEditBuffer[i] && (int)szEditBuffer[i] <= 55))
			{
				isOk = FALSE;
				SetWindowText(hwndMsg, szOtcMsg);
			}
		}
	}

	if (checkType == 10)
	{
		long long decNum = _wtoll(szEditBuffer);
		if (decNum > INT_MAX || decNum < INT_MIN)
		{
			isOk = FALSE;
			SetWindowText(hwndMsg, szMaxMsg);
		}
		for (i = 0; i < length; i++)
		{
			if (!(48 <= (int)szEditBuffer[i] && (int)szEditBuffer[i]  <= 57))
			{
				isOk = FALSE;
				SetWindowText(hwndMsg, szDecMsg);
			}
		}
	}

	if (checkType == 16)
	{
		for (i = 0; i < length; i++)
		{
			long long decNum = radixToDecLL(16, szEditBuffer);
			//数字范围
			if (decNum > INT_MAX || decNum < INT_MIN)
			{
				isOk = FALSE;
				SetWindowText(hwndMsg, szMaxMsg);
			}

			if ((!(48 <= (int)szEditBuffer[i] && (int)szEditBuffer[i] <= 57)) &&
				(!(65 <= (int)szEditBuffer[i] && (int)szEditBuffer[i] <= 70)) &&
				(!(97 <= (int)szEditBuffer[i] && (int)szEditBuffer[i] <= 102)))
			{
				isOk = FALSE;
				SetWindowText(hwndMsg, szHexMsg);
			}
		}
	}
	if (checkType == ID_RADIO_ASCII)
	{
		for (i = 0; i < length; i++)
		{
			

			if (!(32 <= (int)szEditBuffer[i] && (int)szEditBuffer[i] <= 126))
			{
				isOk = FALSE;
				SetWindowText(hwndMsg, szAsciiMsg);
			}
		}
	}
	if (isOk)
	{
		SetWindowText(hwndMsg, NULL);
	}
	if (!isOk)
	{
		SetWindowText(hwndTexts[1], NULL);
		SetWindowText(hwndTexts[3], NULL);
		SetWindowText(hwndTexts[5], NULL);
		SetWindowText(hwndTexts[7], NULL);
		SetWindowText(hwndTexts[9], NULL);
		SetWindowText(hwndTexts[11], NULL);
		SetWindowText(hwndTexts[13], NULL);
	}
	checkType = record;
	return isOk;
}

void cleanFrames(HWND hwndTexts[])
{
	SetWindowText(hwndTexts[1], NULL);
	SetWindowText(hwndTexts[3], NULL);
	SetWindowText(hwndTexts[5], NULL);
	SetWindowText(hwndTexts[7], NULL);
	SetWindowText(hwndTexts[9], NULL);
	SetWindowText(hwndTexts[11], NULL);
	SetWindowText(hwndTexts[13], NULL);
}

