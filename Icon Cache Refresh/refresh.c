#include "C:\dev\lcc\include\windows.h"
#include "C:\dev\lcc\include\shlobj.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void APIError(char*);


HWND     hCmdRefresh, hCmdExit, hLblCaption;
LOGFONT  lgfTahoma;
LOGBRUSH lgb;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR szCmdLine, int nCmdShow)
{
    HWND hWnd;
    HDC hGenDC;
    WNDCLASSEX wcex;
    MSG nMsg;
    const long lWinWidth = 157;
    const long lWinHeight = 60;
    const char* lpClass = "CIcoRefresh";
    const char* lpTitle = "Icon Cache Refresh";
    long lDeskWidth;
    long lDeskHeight;

    lDeskWidth = GetDeviceCaps(GetDC(GetDesktopWindow()), HORZRES); 
    lDeskHeight = GetDeviceCaps(GetDC(GetDesktopWindow()), VERTRES);

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DROPSHADOW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = lpClass;
    wcex.hIconSm = NULL;

    RegisterClassEx(&wcex);
    
    hWnd = CreateWindowEx(0, lpClass, lpTitle, WS_POPUP | WS_BORDER,
                          (long)(lDeskWidth - lWinWidth)/2, (long)(lDeskHeight - lWinHeight)/2,
                          lWinWidth, lWinHeight, NULL, NULL, hInstance, NULL);
    hLblCaption = CreateWindow("STATIC", "Refresh the Icon Cache?", WS_CHILD,
                               17, 5, 120, 20, hWnd, NULL, hInstance, NULL);
    hCmdRefresh = CreateWindow("BUTTON", "R&efresh", WS_CHILD | BS_FLAT,
                               5, 30, 70, 20, hWnd, NULL, hInstance, NULL);
    hCmdExit = CreateWindow("BUTTON", "&Never Mind", WS_CHILD | BS_FLAT,
                            80, 30, 70, 20, hWnd, NULL, hInstance, NULL);
    hGenDC = GetDC(hCmdRefresh);
    lgfTahoma.lfHeight = -MulDiv(8, GetDeviceCaps(hGenDC, LOGPIXELSY), 72);
    lgfTahoma.lfWeight = FW_NORMAL;
    lgfTahoma.lfCharSet = ANSI_CHARSET;
    strcpy(lgfTahoma.lfFaceName, "Tahoma");
    ReleaseDC(hCmdRefresh, hGenDC);

    SendMessage(hCmdRefresh, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdExit, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hLblCaption, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);

    ShowWindow(hWnd, SW_SHOWNORMAL);
    ShowWindow(hCmdRefresh, SW_SHOWNORMAL);
    ShowWindow(hCmdExit, SW_SHOWNORMAL);
    ShowWindow(hLblCaption, SW_SHOWNORMAL);

    UpdateWindow(hWnd);
    
    while(GetMessage(&nMsg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&nMsg);
        DispatchMessage(&nMsg);
    }
    
    UnregisterClass(lpClass, hInstance);
    return nMsg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    switch (nMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
            return TRUE;

        case WM_COMMAND:
            if((HWND)lParam == hCmdRefresh)
                SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
            if((HWND)lParam == hCmdExit)
                SendMessage(hWnd, WM_CLOSE, 0, 0);
            return FALSE;

        case WM_KEYDOWN:
            if(wParam == VK_ESCAPE)
                SendMessage(hWnd, WM_CLOSE, 0, 0);
            return FALSE; 
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

void APIError(char* lpTitle)
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
