#include "C:\dev\lcc\include\windows.h"
#include "C:\dev\lcc\include\string.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

HWND        hCmdPower, hCmdReady, hCmdExit, hGrpInfo;
HWND        hLblInfo;
HDC         hDC;
HINSTANCE   hAppInstance;
LOGFONT     lgfTahoma;
char        sFileName[MAX_CHARS];
const char* lpInfo = "This utility helps in switching off the monitor. This is "
"particularly helpful in situations when the monitor does not have a dedicated "
"power button. A classic example is Laptop.\n\nTo use this tool:\n1. Click on "
"the 'Turn Off' button.\n2. The monitor will turn off.\n3. The monitor can be "
"turned back on by either moving the mouse or pressing any keyboard key.";

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR szCmdLine, int nCmdShow)
{
    HWND        hWnd;
    HDC         hGenDC;
    WNDCLASSEX  wcex;
    MSG         nMsg;
    const long  lWinWidth = 352;
    const long  lWinHeight = 198;
    const char* lpClass = "CPWRApp";
    const char* lpTitle = " Turn off monitor";
    long        lDeskWidth;
    long        lDeskHeight;

    if(strlen(szCmdLine) > 0)
    {
        if(!strcmpi(szCmdLine, "/OFF") || !strcmpi(szCmdLine, "/ON")
           || !strcmpi(szCmdLine, "/?"))
        {
            if(!strcmpi(szCmdLine, "/OFF"))
            {
                SendMessage(HWND_BROADCAST, WM_SYSCOMMAND,
                            SC_MONITORPOWER, (LPARAM)SWITCHOFF);
            }
            if(!strcmpi(szCmdLine, "/ON"))
            {
                SendMessage(HWND_BROADCAST, WM_SYSCOMMAND,
                            SC_MONITORPOWER, (LPARAM)SWITCHON);
            }
            if(!strcmpi(szCmdLine, "/?"))
            {
                MessageBox(NULL, "Options:\n\n/OFF: Turn Off monitor.\n/ON: "
                "Turn On monitor.\n/?: To display this help.\n\nNOTE: Options "
                "are case insensitive.", "Options", MB_OK | MB_ICONINFORMATION);
            }
            return 0;
        }
        else
        {
            MessageBox(NULL, "Invalid Option! If you are running this from\n"
            "command line, then try these options:\n\n/OFF: Turn Off monitor."
            "\n/ON: Turn On monitor.\n/?: To display this help.\n\nNOTE: "
            "Options are case insensitive.","Options",
            MB_OK | MB_ICONINFORMATION);
            return 0;
        }
    }

    lDeskWidth = GetDeviceCaps(GetDC(GetDesktopWindow()), HORZRES); 
    lDeskHeight = GetDeviceCaps(GetDC(GetDesktopWindow()), VERTRES);
    hAppInstance = hInstance;

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

    hWnd = CreateWindowEx(WS_EX_APPWINDOW, lpClass, lpTitle, WS_SYSMENU,
                          (long)(lDeskWidth - lWinWidth)/2,
                          (long)(lDeskHeight - lWinHeight)/2, lWinWidth,
                          lWinHeight, NULL, NULL, hInstance, NULL);

    hGrpInfo = CreateWindow("BUTTON", "", WS_CHILD | BS_GROUPBOX,
                            10, 3, 242, 163, hWnd, NULL, hInstance, NULL);

    hLblInfo = CreateWindow("STATIC", lpInfo, WS_CHILD,
                            8, 12, 222, 143, hGrpInfo, NULL, hInstance, NULL);

    hCmdPower = CreateWindow("BUTTON", "&Turn Off",
                             WS_CHILD | BS_FLAT | WS_TABSTOP, 262, 110, 75, 25,
                             hWnd, NULL, hInstance, NULL);

    hCmdExit = CreateWindow("BUTTON", "E&xit", WS_CHILD | BS_FLAT | WS_TABSTOP,
                            262, 140, 75, 25, hWnd, NULL, hInstance, NULL);

    hGenDC = GetDC(hCmdPower);
    lgfTahoma.lfHeight = -MulDiv(8, GetDeviceCaps(hGenDC, LOGPIXELSY), 72);
    lgfTahoma.lfWeight = FW_NORMAL;
    lgfTahoma.lfCharSet = ANSI_CHARSET;
    strcpy(lgfTahoma.lfFaceName, "Tahoma");
    ReleaseDC(hCmdPower, hGenDC);

    SendMessage(hGrpInfo, WM_SETFONT,
                (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hLblInfo, WM_SETFONT,
                (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdPower, WM_SETFONT,
                (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdExit, WM_SETFONT,
                (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);

    ShowWindow(hWnd, SW_SHOWNORMAL);
    ShowWindow(hGrpInfo, SW_SHOWNORMAL);
    ShowWindow(hLblInfo, SW_SHOWNORMAL);
    ShowWindow(hCmdPower, SW_SHOWNORMAL);
    ShowWindow(hCmdExit, SW_SHOWNORMAL);
    hDC = GetDC(hWnd);

    UpdateWindow(hWnd);

    while(GetMessage(&nMsg, NULL, 0, 0) > 0)
    {
        if(!IsDialogMessage(hWnd, &nMsg))
        {
            TranslateMessage(&nMsg);
            DispatchMessage(&nMsg);
        }
    }
    
    UnregisterClass(lpClass, hInstance);
    return nMsg.wParam;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    switch (nMsg)
    {
        case WM_PAINT:
            BeginPaint(hWnd, &ps);
            DrawIcon(hDC, 285, 40, LoadIcon(hAppInstance,
                     MAKEINTRESOURCE(IDI_APP)));
            EndPaint(hWnd, &ps);
            return TRUE;

        case WM_CLOSE:
            ReleaseDC(hWnd, hDC);
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            return FALSE;

        case WM_COMMAND:
        {
            if((HWND)lParam == hCmdExit)
                SendMessage(hWnd, WM_CLOSE, 0, 0);

            if((HWND)lParam == hCmdPower)
            {
                EnableWindow(hCmdPower, FALSE);
                SendMessage(HWND_BROADCAST, WM_SYSCOMMAND,
                            SC_MONITORPOWER, (LPARAM)SWITCHOFF);
                Sleep(1000);
                EnableWindow(hCmdPower, TRUE);
            }

            return TRUE;
        }

        case WM_KEYUP:
        {
            if(wParam == VK_ESCAPE)
                SendMessage(hWnd, WM_CLOSE, 0, 0);
            return FALSE;
        }
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}
