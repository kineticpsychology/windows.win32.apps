#define _WIN32_WINNT 0x0501 //WindowsXP

#include "C:\dev\lcc\include\windows.h"
#include "C:\dev\lcc\include\stdlib.h"
#include "C:\dev\lcc\include\math.h"
#include "C:\dev\lcc\include\Commdlg.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void fnCreateStaticBlock(void);
void fnCreateLegend(void);
void fnParsePositionAndColor(int, int, COLORREF);
void fnParseANDXOR(void);
int fnBinArr2Dec(INT* pArr, INT iLen);
void fnGetSysCursorFile(LPSTR, unsigned long*);
void fnRestoreSysCursor(void);
void MessageL(long, LPCSTR);
void APIError(LPCSTR);

HINSTANCE gInstance;
LOGFONT lgf;
HWND hWndMain, hCmdOK, hCmdSysCur, hCmdExit, hLblInfo;
HWND hLblBlack, hLblWhite, hLblScreen, hLblRevScreen; 
const COLORREF crMain = RGB(0xDD, 0xDD, 0xDD);
const COLORREF crBlack = RGB(0x00, 0x00, 0x00);
const COLORREF crWhite = RGB(0xFF, 0xFF, 0xFF);
const COLORREF crScreen = RGB(0xDD, 0xDD, 0xDD);
const COLORREF crRevScreen = RGB(0x00, 0x00, 0x80);
HBRUSH hbrBlack, hbrWhite, hbrScreen, hbrRevScreen;
const SHORT iBLOCKWIDTH = 10;
const SHORT iBLOCKHEIGHT = 10;
BOOL bNoDialog = TRUE;
COLORREF crBLOCKS[1024];
BYTE mAND[128];
BYTE mXOR[128];

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR szCmdLine, int nCmdShow)
{
    LPCSTR lpWndClass = "CCur";
    LPCSTR lpTitle = "Cursor";
    WNDCLASSEX wcex;
    MSG nMsg;
    HDC hGenDC, hDC;
    CURSORINFO ci;
    
    long lDeskWidth, lDeskHeight;
    long lWinWidth = 450;
    long lWinHeight = 367;
    int idx = 0;

    lDeskWidth = GetDeviceCaps(GetDC(GetDesktopWindow()), HORZRES); 
    lDeskHeight = GetDeviceCaps(GetDC(GetDesktopWindow()), VERTRES);

    hbrBlack = CreateSolidBrush(crBlack);
    hbrWhite = CreateSolidBrush(crWhite);
    hbrScreen = CreateSolidBrush(crScreen);
    hbrRevScreen = CreateSolidBrush(crRevScreen);
    hDC = GetDC(hWndMain);

    gInstance = hInstance;
    ZeroMemory(mAND, sizeof(INT)*128);
    ZeroMemory(mXOR, sizeof(INT)*128);
    
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DROPSHADOW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = hbrScreen;
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = lpWndClass;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));

    RegisterClassEx(&wcex);

    hWndMain = CreateWindowEx(WS_EX_WINDOWEDGE, lpWndClass, lpTitle,
                              WS_DLGFRAME | WS_SYSMENU, (lDeskWidth - lWinWidth)/2,
                              (lDeskHeight - lWinHeight)/2, lWinWidth, lWinHeight,
                              NULL, NULL, hInstance, NULL);

    
    hCmdOK = CreateWindow("BUTTON", "&OK", WS_CHILD | WS_TABSTOP | BS_FLAT,
                          350, 247, 70, 25, hWndMain, NULL, hInstance, NULL);

    hCmdSysCur = CreateWindow("BUTTON", "&Restore", WS_CHILD | WS_TABSTOP | BS_FLAT,
                              350, 277, 70, 25, hWndMain, NULL, hInstance, NULL);

    hCmdExit = CreateWindow("BUTTON", "E&xit", WS_CHILD | WS_TABSTOP | BS_FLAT,
                            350, 307, 70, 25, hWndMain, NULL, hInstance, NULL);
    
    hLblInfo = CreateWindow("STATIC", "More Info...", WS_CHILD | SS_NOTIFY | SS_CENTER,
                            360, 140, 70, 13, hWndMain, NULL, hInstance, NULL);
    hGenDC = GetDC(hCmdOK);
    lgf.lfHeight = -MulDiv(8, GetDeviceCaps(hGenDC, LOGPIXELSY), 72);
    lgf.lfWeight = FW_NORMAL;
    lgf.lfCharSet = ANSI_CHARSET;
    strcpy(lgf.lfFaceName, "Tahoma");
    ReleaseDC(hCmdOK, hGenDC);
    
    for(idx=0; idx<1024; idx++)
        crBLOCKS[idx] = crScreen;

    SendMessage(hCmdOK, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgf), (LPARAM)TRUE);
    SendMessage(hCmdSysCur, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgf), (LPARAM)TRUE);
    SendMessage(hCmdExit, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgf), (LPARAM)TRUE);
    lgf.lfUnderline = TRUE;
    SendMessage(hLblInfo, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgf), (LPARAM)TRUE);

    ShowWindow(hCmdOK, SW_SHOWNORMAL);
    ShowWindow(hCmdSysCur, SW_SHOWNORMAL);
    ShowWindow(hCmdExit, SW_SHOWNORMAL);
    ShowWindow(hLblInfo, SW_SHOWNORMAL);
    ShowWindow(hWndMain, SW_SHOWNORMAL);
    EnableWindow(hCmdSysCur, FALSE);

    fnCreateStaticBlock();
    fnCreateLegend();

    UpdateWindow(hWndMain);

    while(GetMessage(&nMsg, NULL, 0, 0) > 0)
    {
        if(!IsDialogMessage(hWndMain, &nMsg))
        {
            TranslateMessage(&nMsg);
            DispatchMessage(&nMsg);
        }
    }
    ReleaseDC(hWndMain, hDC);
    return nMsg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    BOOL bHand = FALSE;
    PAINTSTRUCT ps;
    int idx, lX, lY;

    switch(nMsg)
    {
        case WM_CTLCOLORSTATIC:
            if((HWND)lParam == hLblInfo)
            {
                SetTextColor((HDC)wParam, RGB(0x00, 0x00, 0xFF));
                SetBkColor((HDC)wParam, crScreen);
                return (LONG_PTR)hbrScreen;
            }
            return TRUE;

        case WM_PAINT:
            BeginPaint(hWnd, &ps);
            fnCreateStaticBlock();
            fnCreateLegend();
            EndPaint(hWnd, &ps);
            return TRUE;

        case WM_CLOSE:
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            return TRUE;

        case WM_COMMAND:
            if((HWND)lParam == hCmdOK)
            {
                fnParseANDXOR();
                EnableWindow(hCmdSysCur, TRUE);
                return FALSE;
            }
            if((HWND)lParam == hCmdSysCur)
            {
                fnRestoreSysCursor();
                EnableWindow(hCmdSysCur, FALSE);
                return FALSE;
            }
            if((HWND)lParam == hLblInfo && bNoDialog)
            {
                CreateDialog(gInstance, MAKEINTRESOURCE(IDD_DIALOG), hWndMain, DlgProc);
                bNoDialog = FALSE;
                return FALSE;
            }
            if((HWND)lParam == hCmdExit)
            {
                SendMessage(hWnd, WM_CLOSE, 0, 0);
                return TRUE;
            }

        case WM_LBUTTONDOWN:
            if ((long)LOWORD(lParam) < 10 + iBLOCKWIDTH * 32 && (long)LOWORD(lParam) >= 10 &&
                (long)HIWORD(lParam) < 10 + iBLOCKHEIGHT * 32 && (long)HIWORD(lParam) >= 10)
            {
                lX = (INT)LOWORD(lParam);
                lY = (INT)HIWORD(lParam);
                if (GetKeyState(VK_SHIFT) & 8000)
                    fnParsePositionAndColor(lX, lY, crWhite);
                else
                    fnParsePositionAndColor(lX, lY, crBlack);
                return TRUE;
            }
        case WM_RBUTTONDOWN:
            if ((long)LOWORD(lParam) < 10 + iBLOCKWIDTH * 32 && (long)LOWORD(lParam) >= 10 &&
                (long)HIWORD(lParam) < 10 + iBLOCKHEIGHT * 32 && (long)HIWORD(lParam) >= 10)
            {
                lX = (INT)LOWORD(lParam);
                lY = (INT)HIWORD(lParam);
                if (GetKeyState(VK_SHIFT) & 8000)
                    fnParsePositionAndColor(lX, lY, crRevScreen);
                else
                    fnParsePositionAndColor(lX, lY, crScreen);
                return TRUE;
            }
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

BOOL CALLBACK DlgProc(HWND hWndDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    switch(nMsg)
    {
        case WM_COMMAND:
        {
            case ID_OK:
            {
                bNoDialog = TRUE;
                EndDialog(hWndDlg, 0);
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
}
void fnCreateStaticBlock()
{
    int idx, lX, lY;
    POINT pRect[4];
    HDC hDC;
    HPEN hBoundary;
    HBRUSH hbrTmp;

    hDC = GetDC(hWndMain);
    hBoundary = CreatePen(PS_SOLID, 0, RGB(0x00, 0x00, 0x00));

    for(idx=0; idx<1024; idx++)
    {
        hbrTmp = CreateSolidBrush(crBLOCKS[idx]);
        lX = 10 + (idx%32) * 10;
        lY = 10 + (idx/32) * 10;
        pRect[0].x = lX;
        pRect[0].y = lY;
        pRect[1].x = lX + iBLOCKWIDTH;
        pRect[1].y = lY;
        pRect[2].x = lX + iBLOCKWIDTH;
        pRect[2].y = lY + iBLOCKHEIGHT;
        pRect[3].x = lX;
        pRect[3].y = lY + iBLOCKHEIGHT;
        SelectObject(hDC, hbrTmp);
        SelectObject(hDC, hBoundary);
        Polygon(hDC, pRect, 4);
        DeleteObject(hbrTmp);
    }
    DeleteObject(hBoundary);
    ReleaseDC(hWndMain, hDC);
    return;
}

void fnCreateLegend()
{
    HDC hDC;
    HPEN hBoundary;
    POINT pBlocks[4];
    INT idx;
    HFONT hfText;

    hDC = GetDC(hWndMain);
    hfText = CreateFont(13, -MulDiv(4, GetDeviceCaps(hDC, LOGPIXELSY), 72), 0, 0,
                        FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
                        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                        DEFAULT_PITCH, "Tahoma");

    hBoundary = CreatePen(PS_SOLID, 0, RGB(0x00, 0x00, 0x00));
    pBlocks[0].x = 345;
    pBlocks[0].y = 10;
    pBlocks[1].x = 435;
    pBlocks[1].y = 10;
    pBlocks[2].x = 435;
    pBlocks[2].y = 165;
    pBlocks[3].x = 345;
    pBlocks[3].y = 165;

    SetBkMode(hDC, TRANSPARENT);
    SelectObject(hDC, hbrScreen);
    SelectObject(hDC, hBoundary);
    SelectObject(hDC, hfText);
    Polygon(hDC, pBlocks, 4);

    for(idx=0; idx<4; idx++)
    {
        pBlocks[0].x = 350;
        pBlocks[0].y = 20 + idx*30;
        pBlocks[1].x = 365;
        pBlocks[1].y = pBlocks[0].y;
        pBlocks[2].x = 365;
        pBlocks[2].y = pBlocks[0].y + 15;
        pBlocks[3].x = 350;
        pBlocks[3].y = pBlocks[0].y + 15;
        switch(idx)
        {
            case 0:
                SelectObject(hDC, hbrBlack);
                TextOut(hDC, 375, pBlocks[0].y, "Black", 5);
                break;
            case 1:
                hbrWhite = CreateSolidBrush(crWhite);
                SelectObject(hDC, hbrWhite);
                TextOut(hDC, 375, pBlocks[0].y, "White", 5);
                break;
            case 2:
                SelectObject(hDC, hbrScreen);
                TextOut(hDC, 375, pBlocks[0].y, "Screen", 6);
                break;
            case 3:
                SelectObject(hDC, hbrRevScreen);
                TextOut(hDC, 375, pBlocks[0].y, "Inv. Screen", 11);
                break;
        }
        Polygon(hDC, pBlocks, 4);
        DeleteObject(hBoundary);
    }
    ReleaseDC(hWndMain, hDC);
    return;
}


void fnParsePositionAndColor(int lX, int lY, COLORREF crColor)
{
    int iIndex;
    int iRow, iCol;

    iCol = (lX - 10)/(iBLOCKWIDTH);
    iRow = (lY - 10)/(iBLOCKHEIGHT);
    iIndex = iRow * 32 + iCol;
    crBLOCKS[iIndex] = crColor;

    fnCreateStaticBlock();
    
}

void fnParseANDXOR()
{
    DWORD crColor = 0;
    HCURSOR hCur;
    unsigned long ulLen = 1024;
    char lpCurFile[ulLen];
    char lpExpCurFile[ulLen];
    int idx, jdx, ndx, ANDVal[8], XORVal[8];

    for(idx=0; idx<1024; idx+=8)
    {
        for(jdx=0; jdx<8; jdx++)
        {
            if(crBLOCKS[idx + jdx] == crBlack)
            {
                ANDVal[jdx] = 0;
                XORVal[jdx] = 0;
            }
            else if(crBLOCKS[idx + jdx] == crWhite)
            {
                ANDVal[jdx] = 0;
                XORVal[jdx] = 1;
            }
            else if(crBLOCKS[idx + jdx] == crScreen)
            {
                ANDVal[jdx] = 1;
                XORVal[jdx] = 0;
            }
            else if(crBLOCKS[idx + jdx] == crRevScreen)
            {
                ANDVal[jdx] = 1;
                XORVal[jdx] = 1;
            }
        }
        mAND[idx/8] = fnBinArr2Dec(ANDVal, 8);
        mXOR[idx/8] = fnBinArr2Dec(XORVal, 8);
    }
    hCur = CreateCursor(gInstance, 0, 0, 32, 32, mAND, mXOR);
    SetSystemCursor(CopyCursor(hCur), OCR_NORMAL);
    return;
}

int fnBinArr2Dec(INT* pArr, INT iLen)
{
    int idx, lDec = 0;
    
    for(idx=0; idx<=iLen-1; idx++)
        lDec += pow(2, (iLen-idx-1)) * *(pArr+idx);
    return lDec;
}

void fnGetSysCursorFile(LPSTR lpCurFile, unsigned long* ulLen)
{
    HKEY hRegCur;
    DWORD REG_EXP_SZ = 2;

    RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Cursors", 0, KEY_READ, &hRegCur);
    RegQueryValueEx(hRegCur, "Arrow", NULL, &REG_EXP_SZ, (LPBYTE)lpCurFile, ulLen);
    RegCloseKey(hRegCur);
    return;
}

void fnRestoreSysCursor()
{
    HCURSOR hSysCursor;
    unsigned long ulLen = 1024;
    char lpCurFile[ulLen], lpExpCurFile[ulLen];

    fnGetSysCursorFile(lpCurFile, &ulLen);
    ExpandEnvironmentStrings(lpCurFile, lpExpCurFile, ulLen);
    hSysCursor = LoadCursorFromFile(lpExpCurFile);
    SetSystemCursor(hSysCursor, OCR_NORMAL);

}

// -- DEBUGGING FUNCTIONS
void MessageL(long lData, LPCSTR lpTitle)
{
    char cAny[MAX_PATH];
    ltoa(lData, (char*)cAny, 10);
    MessageBox(0, cAny, lpTitle, MB_OK);
    return;    
}

void APIError(LPCSTR lpTitle)
{
    DWORD lErr;
    char lpErr[MAX_PATH];

    lErr = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, lErr, 0,
                  lpErr, MAX_PATH, NULL);
    if (lErr != 0)
        MessageBox(0, lpErr, lpTitle, MB_OK | MB_ICONERROR);
    else
        MessageBox(0, lpErr, lpTitle, MB_OK | MB_ICONINFORMATION);
}
