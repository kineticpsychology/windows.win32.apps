#include "C:\dev\lcc\include\windows.h"
#include "C:\dev\lcc\include\time.h"
#include "C:\dev\lcc\include\math.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void TmrProc(HWND, UINT, UINT_PTR, DWORD);
void fnDemoPixels(void);
void fnDemoLines(void);
void fnDemoTriangles(void);
void fnDemoRectangles(void);
void fnDemoCircles(void);
long random(int);

HWND hTxtDraw;
HWND hCmdPixels, hCmdLines, hCmdTriangles, hCmdRectangles, hCmdCircles;
HWND hCmdClear, hCmdExit;
HDC hDC;
HBRUSH hBrush, hbrDraw;
LOGFONT lgfTahoma;
LOGBRUSH lgb;

const long lCanvasWidth = 400;
const long lCanvasHeight = 373;
const COLORREF crBackground = RGB(0, 0, 0);
const long DELAY = 50;
const UINT_PTR nIDTmr = 0xAA;

enum _DEMOMODE
{
    STOP,
    CLEAR,
    PIXELS,
    LINES,
    TRIANGLES,
    RECTANGLES,
    CIRCLES
} DEMOMODE;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR szCmdLine, int nCmdShow)
{
    HWND hWnd;
    HDC hGenDC;
    WNDCLASSEX wcex;
    MSG nMsg;
    const long lWinWidth = 500;
    const long lWinHeight = 400;
    const char* lpClass = "CGDIApp";
    const char* lpTitle = "GDI Demo";
    long lDeskWidth;
    long lDeskHeight;

    lDeskWidth = GetDeviceCaps(GetDC(GetDesktopWindow()), HORZRES); 
    lDeskHeight = GetDeviceCaps(GetDC(GetDesktopWindow()), VERTRES);
    lgb.lbStyle = BS_HOLLOW;
    hBrush = CreateBrushIndirect(&lgb);
    hbrDraw = CreateSolidBrush(crBackground);
    srand(time(NULL));

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
                          (long)(lDeskWidth - lWinWidth)/2, (long)(lDeskHeight - lWinHeight)/2,
                          lWinWidth, lWinHeight, NULL, NULL, hInstance, NULL);

    hTxtDraw = CreateWindow("STATIC", "", WS_CHILD | SS_SUNKEN,
                            1, 1, lCanvasWidth, lCanvasHeight, hWnd, NULL, hInstance, NULL);

    hCmdPixels = CreateWindow("BUTTON", "&Pixels", WS_CHILD | BS_FLAT,
                              410, 10, 80, 25, hWnd, NULL, hInstance, NULL);

    hCmdLines = CreateWindow("BUTTON", "&Lines", WS_CHILD | BS_FLAT,
                              410, 45, 80, 25, hWnd, NULL, hInstance, NULL);

    hCmdTriangles = CreateWindow("BUTTON", "&Triangles", WS_CHILD | BS_FLAT,
                                 410, 80, 80, 25, hWnd, NULL, hInstance, NULL);

    hCmdRectangles = CreateWindow("BUTTON", "&Rectangles", WS_CHILD | BS_FLAT,
                                  410, 115, 80, 25, hWnd, NULL, hInstance, NULL);

    hCmdCircles = CreateWindow("BUTTON", "&Circles", WS_CHILD | BS_FLAT,
                               410, 150, 80, 25, hWnd, NULL, hInstance, NULL);

    hCmdClear = CreateWindow("BUTTON", "C&lear", WS_CHILD | BS_FLAT,
                             410, 308, 80, 25, hWnd, NULL, hInstance, NULL);

    hCmdExit = CreateWindow("BUTTON", "E&xit", WS_CHILD | BS_FLAT,
                             410, 343, 80, 25, hWnd, NULL, hInstance, NULL);

    hDC = GetDC(hTxtDraw);
    hGenDC = GetDC(hCmdPixels);
    lgfTahoma.lfHeight = -MulDiv(8, GetDeviceCaps(hGenDC, LOGPIXELSY), 72);
    lgfTahoma.lfWeight = FW_NORMAL;
    lgfTahoma.lfCharSet = ANSI_CHARSET;
    strcpy(lgfTahoma.lfFaceName, "Tahoma");
    ReleaseDC(hCmdPixels, hGenDC);

    SendMessage(hCmdPixels, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdLines, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdTriangles, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdRectangles, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdCircles, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdClear, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdExit, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);

    DEMOMODE = STOP;
    SelectObject(hDC, hBrush);

    ShowWindow(hWnd, SW_SHOWNORMAL);
    ShowWindow(hTxtDraw, SW_SHOWNORMAL);
    ShowWindow(hCmdPixels, SW_SHOWNORMAL);
    ShowWindow(hCmdLines, SW_SHOWNORMAL);
    ShowWindow(hCmdTriangles, SW_SHOWNORMAL);
    ShowWindow(hCmdRectangles, SW_SHOWNORMAL);
    ShowWindow(hCmdCircles, SW_SHOWNORMAL);
    ShowWindow(hCmdClear, SW_SHOWNORMAL);
    ShowWindow(hCmdExit, SW_SHOWNORMAL);

    UpdateWindow(hWnd);
    SetTimer(hWnd, nIDTmr, DELAY, TmrProc);
    
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
    const int nBuffer = 255;
    char cCmdCaption[nBuffer];
    switch (nMsg)
    {
        case WM_CTLCOLORSTATIC:
            if((HWND)lParam == hTxtDraw)
            {
                SetBkColor(hDC, crBackground);
                return (INT_PTR)hbrDraw;
            }
        case WM_CLOSE:
            KillTimer(hWnd, nIDTmr);
            DeleteObject(hBrush);
            DeleteObject(hbrDraw);
            ReleaseDC(hTxtDraw, hDC);
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            return FALSE;

        case WM_COMMAND:
            if((HWND)lParam == hCmdExit)
                SendMessage(hWnd, WM_CLOSE, 0, 0);
            if((HWND)lParam == hCmdPixels)
            {
                DEMOMODE = PIXELS;
                SendMessage(hCmdClear, WM_SETTEXT, 0, (LPARAM)"Stop");
            }
            if((HWND)lParam == hCmdLines)
            {
                DEMOMODE = LINES;
                SendMessage(hCmdClear, WM_SETTEXT, 0, (LPARAM)"Stop");
            }
            if((HWND)lParam == hCmdTriangles)
            {
                DEMOMODE = TRIANGLES;
                SendMessage(hCmdClear, WM_SETTEXT, 0, (LPARAM)"Stop");
            }
            if((HWND)lParam == hCmdRectangles)
            {
                DEMOMODE = RECTANGLES;
                SendMessage(hCmdClear, WM_SETTEXT, 0, (LPARAM)"Stop");
            }
            if((HWND)lParam == hCmdCircles)
            {
                DEMOMODE = CIRCLES;
                SendMessage(hCmdClear, WM_SETTEXT, 0, (LPARAM)"Stop");
            }
            if((HWND)lParam == hCmdClear)
            {
                if(SendMessage(hCmdClear, WM_GETTEXT, nBuffer, (LPARAM)cCmdCaption) == 4)
                {
                    DEMOMODE = STOP;
                    SendMessage(hCmdClear, WM_SETTEXT, 0, (LPARAM)"Clear");
                }
                else
                    DEMOMODE = CLEAR;
            }
            return FALSE;
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

void TmrProc(HWND hWnd, UINT nMsg, UINT_PTR upID, DWORD dwTime)
{
    RECT rDrawArea;
    switch (DEMOMODE)
    {
        case STOP:
            break;
        case CLEAR:
            GetClientRect(hTxtDraw, &rDrawArea);
            FillRect(hDC, &rDrawArea, hbrDraw);
            break;
        case PIXELS:
            fnDemoPixels();
            break;
        case LINES:
            fnDemoLines();
            break;
        case TRIANGLES:
            fnDemoTriangles();
            break;
        case RECTANGLES:
            fnDemoRectangles();
            break;
        case CIRCLES:
            fnDemoCircles();
            break;
    }
    return;
}


void fnDemoPixels()
{
    COLORREF crColor;
    crColor = RGB(random(255), random(255), random(255));
    SetPixel(hDC, random(lCanvasWidth), random(lCanvasHeight), crColor);
    return;
}

void fnDemoLines()
{
    COLORREF crColor;
    POINT p[2];
    HPEN hPen;

    crColor = RGB(random(255), random(255), random(255));
    p[0].x = random(lCanvasWidth);
    p[0].y = random(lCanvasHeight);
    p[1].x = random(lCanvasWidth);
    p[1].y = random(lCanvasHeight);
    hPen = CreatePen(PS_SOLID, 0, crColor);
    SelectObject(hDC, hPen);
    Polygon(hDC, p, 2);
    DeleteObject(hPen);
    return;
}

void fnDemoTriangles()
{
    COLORREF crColor;
    POINT p[3];
    HPEN hPen;

    crColor = RGB(random(255), random(255), random(255));
    p[0].x = random(lCanvasWidth);
    p[0].y = random(lCanvasHeight);
    p[1].x = random(lCanvasWidth);
    p[1].y = random(lCanvasHeight);
    p[2].x = random(lCanvasWidth);
    p[2].y = random(lCanvasHeight);
    hPen = CreatePen(PS_SOLID, 0, crColor);
    SelectObject(hDC, hPen);
    Polygon(hDC, p, 3);
    DeleteObject(hPen);
    return;
}

void fnDemoRectangles()
{
    COLORREF crColor;
    POINT p[4];
    HPEN hPen;

    crColor = RGB(random(255), random(255), random(255));
    p[0].x = random(lCanvasWidth);
    p[0].y = random(lCanvasHeight);
    p[2].x = random(lCanvasWidth);
    p[2].y = random(lCanvasHeight);

    p[1].x = p[0].x;
    p[1].y = p[2].y;
    p[3].x = p[2].x;
    p[3].y = p[0].y;

    hPen = CreatePen(PS_SOLID, 0, crColor);
    SelectObject(hDC, hPen);
    Polygon(hDC, p, 4);
    DeleteObject(hPen);
    return;
}

void fnDemoCircles()
{
    int iX, iY, iRadius;
    const float DEGTORAD = 3.14159265358979 / (float)90;
    float fPerp, fBase, fDeg;
    COLORREF crColor;

    iX = random(lCanvasWidth);
    iY = random(lCanvasHeight);
    crColor = RGB(random(255), random(255), random(255));
    iRadius = random((int)lCanvasHeight/2);
        
    for(fDeg=0;fDeg<=359;fDeg++)
    {
        fPerp = (float)iRadius * sin(fDeg * DEGTORAD);
        fBase = (float)iRadius * cos(fDeg * DEGTORAD);
        if((long)fBase + iX >= 0 && (long)fBase + iX < lCanvasWidth && 
            (long)fPerp + iY >= 0 && (long)fPerp + iY < lCanvasHeight)
            SetPixel(hDC, (long)fBase + iX, (long)fPerp + iY, crColor);
    }
    return;
}

long random(int iLimit)
{
    return (long) ((float)rand()/(float)RAND_MAX * (float)iLimit);
}
