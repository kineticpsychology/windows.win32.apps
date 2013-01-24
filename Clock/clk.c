#include "C:\dev\lcc\include\windows.h"
#include "C:\dev\lcc\include\CommCtrl.h"
#include "C:\dev\lcc\include\stdio.h"
#include "C:\dev\lcc\include\string.h"
#include "C:\dev\lcc\include\stdlib.h"
#include "C:\dev\lcc\include\time.h"
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
void fnRelateSegments(int, int, HWND);
void fnSeconds(int, int, HWND);
void fnParseTime(HWND);
void TmrProc(HWND, UINT, UINT_PTR, DWORD);
void fnMoveWnd(HWND);
int fnRandom(int);
void MessageL(long, char*);
void APIError(char*);


COLORREF crWndColor;
UINT_PTR nIDTmr = 0xAA;
BOOL MASKED = FALSE;
BOOL HELP = TRUE;
COLORREF crDigitColor = RGB(0xC0, 0xC0, 0xC0);
COLORREF crColonColor = RGB(0xC0, 0xC0, 0xC0);
long lDeskWidth, lDeskHeight, lWinWidth, lWinHeight, lOpacity;
int POSX = 0;
int POSY = 0;
char cAMPM = 'P';
HWND hLblHelp;
const COLORREF PRESETS[9] = {
                                RGB(0xC0, 0xC0, 0xC0), // Faded White
                                RGB(0x00, 0x00, 0xFF), // Deep Blue
                                RGB(0x00, 0xFF, 0xFF), // Cyan
                                RGB(0xC0, 0xFF, 0x00), // Lime
                                RGB(0x00, 0x80, 0x00), // Deep Green
                                RGB(0xFF, 0xFF, 0x00), // Bright Yellow
                                RGB(0xAB, 0xCD, 0xEF), // Blue Cream/Lavendar
                                RGB(0xFF, 0xFF, 0xFF), // Bright White
                                RGB(0xF6, 0xCC, 0x9D) // Yellow Cream
                             };

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpCmdLine, int nCmdShow)
{
    HWND hWnd;
    WNDCLASSEX wcex;
    MSG nMsg;
    HFONT hfHelp;

    long lStyle;
    const char* lpWndClass = "CClock";
    const char* lpTitle = "";

    lWinWidth = 450;
    lWinHeight = 135;
    lDeskWidth = GetDeviceCaps(GetDC(GetDesktopWindow()), HORZRES); 
    lDeskHeight = GetDeviceCaps(GetDC(GetDesktopWindow()), VERTRES);
    crWndColor = RGB(0, 0, 0);
    srand((unsigned)time(NULL));

    // The WNDCLASSEX details    
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_DROPSHADOW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(crWndColor);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = lpWndClass;
    wcex.hIconSm = NULL;

    RegisterClassEx(&wcex);

    hWnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, lpWndClass, lpTitle,
                          WS_POPUP, (lDeskWidth - lWinWidth)/2,
                          (lDeskHeight - lWinHeight)/2, lWinWidth, lWinHeight,
                          NULL, NULL, hInstance, NULL);

    hLblHelp = CreateWindowEx(WS_EX_TOOLWINDOW, "STATIC", "?",
                          WS_CHILD | SS_NOTIFY, 440, -2, 25, 20,
                          hWnd, NULL, hInstance, NULL);

    hfHelp = CreateFont(0, 6, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");

    SendMessage(hLblHelp, WM_SETFONT, (WPARAM)hfHelp, (LPARAM)TRUE);

    lOpacity = 255;
    lStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
    SetWindowLong(hWnd, GWL_EXSTYLE, lStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, crWndColor, lOpacity, LWA_ALPHA);
    
    ShowWindow(hWnd, SW_SHOWNORMAL);
    ShowWindow(hLblHelp, SW_SHOWNORMAL);

    UpdateWindow(hWnd);

    SetTimer(hWnd, nIDTmr, 1000, TmrProc);
    fnParseTime(hWnd);

    while(GetMessage(&nMsg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&nMsg);
        DispatchMessage(&nMsg);
    }
    
    DeleteObject(hfHelp);
    return nMsg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hDC, hGenDC;
    HBRUSH hBrush;
    BOOL bNeedsMovement;
    const long SROLLUP = 120;
    const long SROLLDOWN = 65416;
    COLORREF crRandom;

    switch(nMsg)
    {
        case WM_CTLCOLORSTATIC:
            hGenDC = (HDC) wParam;
            SetTextColor(hGenDC, RGB(0, 0xFF, 0xFF));
            SetBkColor(hGenDC, crWndColor);
            hBrush = CreateSolidBrush(crWndColor);
            return (INT_PTR)hBrush;

        case WM_CLOSE:
            ReleaseDC(hLblHelp, hGenDC);
            DeleteObject(hBrush);
            KillTimer(hWnd, nIDTmr);
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            return TRUE;

        case WM_SETCURSOR:
            if(wParam == (WPARAM)hLblHelp)
                SetCursor(LoadCursor(NULL, IDC_HAND));
            else
                SetCursor(LoadCursor(NULL, IDC_ARROW));
            return TRUE;

        case WM_COMMAND:
        {
            if((HWND)lParam == hLblHelp)
                DialogBox(NULL, MAKEINTRESOURCE(IDD_DLG), hWnd, DlgProc);
            return TRUE;
        }
        case WM_KEYDOWN:
        {
            bNeedsMovement = FALSE;
            if(wParam == VK_X && (GetKeyState(VK_CONTROL ) & 8000))
            {
                ReleaseDC(hLblHelp, hGenDC);
                DeleteObject(hBrush);
                //KillTimer(hWnd, nIDTmr);
                DestroyWindow(hWnd);
                PostQuitMessage(0);
                return TRUE;
            }
            if(wParam == VK_UP && (GetKeyState(VK_CONTROL ) & 8000))
            {
                POSY = POSY > -1 ? POSY - 1 : POSY;
                bNeedsMovement = TRUE;
            }
            if(wParam == VK_DOWN && (GetKeyState(VK_CONTROL ) & 8000))
            {
                POSY = POSY < 1 ? POSY + 1 : POSY;
                bNeedsMovement = TRUE;
            }
            if(wParam == VK_LEFT && (GetKeyState(VK_CONTROL ) & 8000))
            {
                POSX = POSX > -1 ? POSX - 1 : POSX;
                bNeedsMovement = TRUE;
            }
            if(wParam == VK_RIGHT && (GetKeyState(VK_CONTROL ) & 8000))
            {
                POSX = POSX < 1 ? POSX + 1 : POSX;
                bNeedsMovement = TRUE;
            }
            if(GetKeyState(VK_LCONTROL ) & 8000 && (wParam > 48 && wParam < 58))
            {
                crDigitColor = PRESETS[(long)wParam - 49];
                crColonColor = PRESETS[(long)wParam - 49];
                fnParseTime(hWnd);
            }
            if(GetKeyState(VK_RCONTROL ) & 8000 && (wParam > 48 && wParam < 58))
            {
                crColonColor = PRESETS[(long)wParam - 49];
                fnParseTime(hWnd);
            }
            
            if(GetKeyState(VK_LCONTROL ) & 8000 && wParam == VK_0)
            {
                crRandom = RGB(fnRandom(255), fnRandom(255), fnRandom(255));
                crDigitColor = crRandom;
                crColonColor = crRandom;
                fnParseTime(hWnd);
            }
            if(GetKeyState(VK_RCONTROL ) & 8000 && wParam == VK_0)
            {
                crColonColor = RGB(fnRandom(255), fnRandom(255), fnRandom(255));
                fnParseTime(hWnd);
            }
            if(GetKeyState(VK_CONTROL ) & 8000 && wParam == VK_H)
            {
                HELP = !HELP;
                if(HELP)
                    ShowWindow(hLblHelp, SW_SHOWNORMAL);
                else
                    ShowWindow(hLblHelp, SW_HIDE);
            }
            if(bNeedsMovement) fnMoveWnd(hWnd);
            return TRUE;
        }
        case WM_MOUSEWHEEL:
        {
            if((long)HIWORD(wParam) == SROLLUP && (GetKeyState(VK_CONTROL ) & 8000))
                if(lOpacity < 255)
                {
                    lOpacity += 5;
                    SetLayeredWindowAttributes(hWnd, crWndColor, lOpacity, LWA_ALPHA);
                }
            if((long)HIWORD(wParam) == SROLLDOWN && (GetKeyState(VK_CONTROL ) & 8000))
                if(lOpacity > 20)
                {
                    lOpacity -= 5;
                    SetLayeredWindowAttributes(hWnd, crWndColor, lOpacity, LWA_ALPHA);
                }
            return TRUE;
        }
        case WM_MBUTTONDOWN:
        {
            if(GetKeyState(VK_CONTROL ) & 8000)
            {
                lOpacity = 255;
                MASKED = !MASKED;
                if(MASKED)
                    SetLayeredWindowAttributes(hWnd, crWndColor, lOpacity, LWA_COLORKEY);
                else
                    SetLayeredWindowAttributes(hWnd, crWndColor, lOpacity, LWA_ALPHA);
            }
            return TRUE;
        }
        case WM_TIMER:
        {
            return TRUE;
        }
        
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

void fnRelateSegments(int iDigit, int iPlace, HWND hWnd)
{
    int iTranslate, iIdx;
    BOOL bOnIndex[7];

    POINT PT1[6];
    POINT PT2[6];
    POINT PT3[6];
    POINT PT4[6];
    POINT PT5[6];
    POINT PT6[6];
    POINT PT7[6];
    POINT CN1[4];
    POINT CN2[4];

    POINT *PT[7];

    HDC hDC;
    HBRUSH hDigitBrush[7], hColonBrush;
    HANDLE hDigitPen[7], hColonPen;
    HFONT hfAMPM;

    hDC = 0;

    for(iIdx = 0; iIdx < 7; iIdx++)
        bOnIndex[iIdx] = FALSE;

    switch (iDigit)
    {
        case 0:
            if (iPlace != 3)
            {
                bOnIndex[0] = TRUE;
                bOnIndex[1] = TRUE;
                bOnIndex[2] = TRUE;
                bOnIndex[3] = TRUE;
                bOnIndex[4] = TRUE;
                bOnIndex[5] = TRUE;
            }
            break;
        case 1:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            break;
        case 2:
            bOnIndex[0] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[3] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 3:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 4:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 5:
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 6:
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[3] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 7:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[5] = TRUE;
            break;
        case 8:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[3] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 9:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
    }

    for(iIdx = 0; iIdx < 7; iIdx++)
    {
        if(bOnIndex[iIdx])
        {
            hDigitBrush[iIdx] = CreateSolidBrush(crDigitColor);
            hDigitPen[iIdx] = CreatePen(PS_SOLID, 0, crDigitColor);
        }
        else
        {
            hDigitBrush[iIdx] = CreateSolidBrush(crWndColor);
            hDigitPen[iIdx] = CreatePen(PS_SOLID, 0, crWndColor);
        }
    }
    hDC = GetDC(hWnd);

    PT[0] = PT1;
    PT[1] = PT2;
    PT[2] = PT3;
    PT[3] = PT4;
    PT[4] = PT5;
    PT[5] = PT6;
    PT[6] = PT7;
    

    switch (iPlace)
    {
        case 0:
            iTranslate = 260;
            break;
        case 1:
            iTranslate = 185;
            break;
        case 2:
            iTranslate = 75;
            break;
        case 3:
            iTranslate = 0;
            break;
    }
    PT1[0].x = iTranslate + 64; PT1[0].y = 13;
    PT1[1].x = iTranslate + 69; PT1[1].y = 18;
    PT1[2].x = iTranslate + 69; PT1[2].y = 58;
    PT1[3].x = iTranslate + 64; PT1[3].y = 63;
    PT1[4].x = iTranslate + 59; PT1[4].y = 58;
    PT1[5].x = iTranslate + 59; PT1[5].y = 18;
    
    PT2[0].x = iTranslate + 64; PT2[0].y = 67;
    PT2[1].x = iTranslate + 69; PT2[1].y = 72;
    PT2[2].x = iTranslate + 69; PT2[2].y = 112;
    PT2[3].x = iTranslate + 64; PT2[3].y = 117;
    PT2[4].x = iTranslate + 59; PT2[4].y = 112;
    PT2[5].x = iTranslate + 59; PT2[5].y = 72;
    
    PT3[0].x = iTranslate + 62; PT3[0].y = 119;
    PT3[1].x = iTranslate + 57; PT3[1].y = 124;
    PT3[2].x = iTranslate + 17; PT3[2].y = 124;
    PT3[3].x = iTranslate + 12; PT3[3].y = 119;
    PT3[4].x = iTranslate + 17; PT3[4].y = 114;
    PT3[5].x = iTranslate + 57; PT3[5].y = 114;
    
    PT4[0].x = iTranslate + 10; PT4[0].y = 67;
    PT4[1].x = iTranslate + 15; PT4[1].y = 72;
    PT4[2].x = iTranslate + 15; PT4[2].y = 112;
    PT4[3].x = iTranslate + 10; PT4[3].y = 117;
    PT4[4].x = iTranslate + 05; PT4[4].y = 112;
    PT4[5].x = iTranslate + 05; PT4[5].y = 72;

    PT5[0].x = iTranslate + 10; PT5[0].y = 13;
    PT5[1].x = iTranslate + 15; PT5[1].y = 18;
    PT5[2].x = iTranslate + 15; PT5[2].y = 58;
    PT5[3].x = iTranslate + 10; PT5[3].y = 63;
    PT5[4].x = iTranslate + 05; PT5[4].y = 58;
    PT5[5].x = iTranslate + 05; PT5[5].y = 18;

    PT6[0].x = iTranslate + 62; PT6[0].y = 11;
    PT6[1].x = iTranslate + 57; PT6[1].y = 16;
    PT6[2].x = iTranslate + 17; PT6[2].y = 16;
    PT6[3].x = iTranslate + 12; PT6[3].y = 11;
    PT6[4].x = iTranslate + 17; PT6[4].y = 06;
    PT6[5].x = iTranslate + 57; PT6[5].y = 06;

    PT7[0].x = iTranslate + 62; PT7[0].y = 65;
    PT7[1].x = iTranslate + 57; PT7[1].y = 70;
    PT7[2].x = iTranslate + 17; PT7[2].y = 70;
    PT7[3].x = iTranslate + 12; PT7[3].y = 65;
    PT7[4].x = iTranslate + 17; PT7[4].y = 60;
    PT7[5].x = iTranslate + 57; PT7[5].y = 60;
    
    CN1[0].x = 170; CN1[0].y = 26;
    CN1[1].x = 180; CN1[1].y = 36;
    CN1[2].x = 170; CN1[2].y = 46;
    CN1[3].x = 160; CN1[3].y = 36;

    CN2[0].x = 170; CN2[0].y = 90;
    CN2[1].x = 180; CN2[1].y = 100;
    CN2[2].x = 170; CN2[2].y = 110;
    CN2[3].x = 160; CN2[3].y = 100;
        
    for(iIdx = 0; iIdx < 7; iIdx++)
    {
        SelectObject(hDC, hDigitBrush[iIdx]);
        SelectObject(hDC, hDigitPen[iIdx]);
        Polygon(hDC, PT[iIdx], 6);
    }

    hColonBrush = CreateSolidBrush(crColonColor);
    hColonPen = CreatePen(PS_SOLID, 0, crColonColor);
    hfAMPM = CreateFont(0, 15, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                        DEFAULT_QUALITY, DEFAULT_PITCH, "Tahoma");
    SelectObject(hDC, hColonBrush);
    SelectObject(hDC, hColonPen);
    SelectObject(hDC, hfAMPM);

    Polygon(hDC, CN1, 4);
    Polygon(hDC, CN2, 4);

    SetBkColor(hDC, crWndColor);
    SetTextColor(hDC, crColonColor);
    if(cAMPM == 'A')
        TextOut(hDC, 370, 20, "AM", 2);
    else
        TextOut(hDC, 370, 20, "PM", 2);
    
    DeleteObject(hfAMPM);
    DeleteObject(hColonBrush);
    CloseHandle(hColonPen);
    DeleteObject(hColonPen);

    for(iIdx = 0; iIdx < 7; iIdx++)
    {
        DeleteObject(hDigitBrush[iIdx]);
        CloseHandle(hDigitPen[iIdx]);
        DeleteObject(hDigitPen[iIdx]);
    }

    ReleaseDC(hWnd, hDC);
    return;
}

void fnSeconds(int iDigit, int iPlace, HWND hWnd)
{
    int iTranslate, iIdx;
    BOOL bOnIndex[7];

    POINT PT1[6];
    POINT PT2[6];
    POINT PT3[6];
    POINT PT4[6];
    POINT PT5[6];
    POINT PT6[6];
    POINT PT7[6];
    POINT CN1[4];
    POINT CN2[4];

    POINT *PT[7];

    HDC hDC;
    HBRUSH hDigitBrush[7];
    HANDLE hDigitPen[7];
    
    hDC = 0;

    for(iIdx = 0; iIdx < 7; iIdx++)
        bOnIndex[iIdx] = FALSE;

    switch (iDigit)
    {
        case 0:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[3] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            break;
        case 1:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            break;
        case 2:
            bOnIndex[0] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[3] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 3:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 4:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 5:
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 6:
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[3] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 7:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[5] = TRUE;
            break;
        case 8:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[3] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
        case 9:
            bOnIndex[0] = TRUE;
            bOnIndex[1] = TRUE;
            bOnIndex[2] = TRUE;
            bOnIndex[4] = TRUE;
            bOnIndex[5] = TRUE;
            bOnIndex[6] = TRUE;
            break;
    }

    for(iIdx = 0; iIdx < 7; iIdx++)
    {
        if(bOnIndex[iIdx])
        {
            hDigitBrush[iIdx] = CreateSolidBrush(crDigitColor);
            hDigitPen[iIdx] = CreatePen(PS_SOLID, 0, crDigitColor);
        }
        else
        {
            hDigitBrush[iIdx] = CreateSolidBrush(crWndColor);
            hDigitPen[iIdx] = CreatePen(PS_SOLID, 0, crWndColor);
        }
    }
    hDC = GetDC(hWnd);

    PT[0] = PT1;
    PT[1] = PT2;
    PT[2] = PT3;
    PT[3] = PT4;
    PT[4] = PT5;
    PT[5] = PT6;
    PT[6] = PT7;
    

    switch (iPlace)
    {
        case 0:
            iTranslate = 400;
            break;
        case 1:
            iTranslate = 350;
            break;
    }
    PT1[0].x = iTranslate + 28; PT1[0].y = 73;
    PT1[1].x = iTranslate + 31; PT1[1].y = 76;
    PT1[2].x = iTranslate + 31; PT1[2].y = 91;
    PT1[3].x = iTranslate + 28; PT1[3].y = 94;
    PT1[4].x = iTranslate + 25; PT1[4].y = 91;
    PT1[5].x = iTranslate + 25; PT1[5].y = 76;
    
    PT2[0].x = iTranslate + 28; PT2[0].y = 98;
    PT2[1].x = iTranslate + 31; PT2[1].y = 101;
    PT2[2].x = iTranslate + 31; PT2[2].y = 116;
    PT2[3].x = iTranslate + 28; PT2[3].y = 119;
    PT2[4].x = iTranslate + 25; PT2[4].y = 116;
    PT2[5].x = iTranslate + 25; PT2[5].y = 101;

    PT3[0].x = iTranslate + 26; PT3[0].y = 121;
    PT3[1].x = iTranslate + 23; PT3[1].y = 124;
    PT3[2].x = iTranslate + 8; PT3[2].y = 124;
    PT3[3].x = iTranslate + 5; PT3[3].y = 121;
    PT3[4].x = iTranslate + 8; PT3[4].y = 118;
    PT3[5].x = iTranslate + 23; PT3[5].y = 118;

    PT4[0].x = iTranslate + 3; PT4[0].y = 98;
    PT4[1].x = iTranslate + 6; PT4[1].y = 101;
    PT4[2].x = iTranslate + 6; PT4[2].y = 116;
    PT4[3].x = iTranslate + 3; PT4[3].y = 119;
    PT4[4].x = iTranslate + 0; PT4[4].y = 116;
    PT4[5].x = iTranslate + 0; PT4[5].y = 101;    

    PT5[0].x = iTranslate + 3; PT5[0].y = 73;
    PT5[1].x = iTranslate + 6; PT5[1].y = 76;
    PT5[2].x = iTranslate + 6; PT5[2].y = 91;
    PT5[3].x = iTranslate + 3; PT5[3].y = 94;
    PT5[4].x = iTranslate + 0; PT5[4].y = 91;
    PT5[5].x = iTranslate + 0; PT5[5].y = 76;

    PT6[0].x = iTranslate + 26; PT6[0].y = 71;
    PT6[1].x = iTranslate + 23; PT6[1].y = 74;
    PT6[2].x = iTranslate + 8; PT6[2].y = 74;
    PT6[3].x = iTranslate + 5; PT6[3].y = 71;
    PT6[4].x = iTranslate + 8; PT6[4].y = 68;
    PT6[5].x = iTranslate + 23; PT6[5].y = 68;

    PT7[0].x = iTranslate + 26; PT7[0].y = 96;
    PT7[1].x = iTranslate + 23; PT7[1].y = 99;
    PT7[2].x = iTranslate + 8; PT7[2].y = 99;
    PT7[3].x = iTranslate + 5; PT7[3].y = 96;
    PT7[4].x = iTranslate + 8; PT7[4].y = 93;
    PT7[5].x = iTranslate + 23; PT7[5].y = 93;

    for(iIdx = 0; iIdx < 7; iIdx++)
    {
        SelectObject(hDC, hDigitBrush[iIdx]);
        SelectObject(hDC, hDigitPen[iIdx]);
        Polygon(hDC, PT[iIdx], 6);
    }
    
    for(iIdx = 0; iIdx < 7; iIdx++)
    {
        DeleteObject(hDigitBrush[iIdx]);
        CloseHandle(hDigitPen[iIdx]);
        DeleteObject(hDigitPen[iIdx]);
    }

    ReleaseDC(hWnd, hDC);
    return;
}


void fnParseTime(HWND hWnd)
{
    int iH, iH10, iH0, iM10, iM0, iS10, iS0;
    SYSTEMTIME st;

    GetLocalTime(&st);
    iH = st.wHour;
    iH = st.wHour >= 13 ? st.wHour - 12 : st.wHour;
    cAMPM = st.wHour >= 12 ? 'P' : 'A';
    iH0 = iH % 10;
    iH10 = iH / 10;
    iM0 = st.wMinute % 10;
    iM10 = st.wMinute / 10;
    iS0 = st.wSecond % 10;
    iS10 = st.wSecond / 10;
    fnRelateSegments(iM0, 0, hWnd);
    fnRelateSegments(iM10, 1, hWnd);
    fnRelateSegments(iH0, 2, hWnd);
    fnRelateSegments(iH10, 3, hWnd);
    fnSeconds(iS10, 1, hWnd);
    fnSeconds(iS0, 0, hWnd);
}

void TmrProc(HWND hWnd, UINT nMsg, UINT_PTR upID, DWORD dwTime)
{
    fnParseTime(hWnd);
    return;
}

void fnMoveWnd(HWND hWnd)
{
    long lx, ly;
    switch(POSX)
    {
        case 0:
            lx = (lDeskWidth - lWinWidth)/2;
            break;
        case -1:
            lx = 0;
            break;
        case 1:
            lx = (lDeskWidth - lWinWidth);
            break;
    }

    switch(POSY)
    {
        case 0:
            ly = (lDeskHeight - lWinHeight)/2;
            break;
        case -1:
            ly = 0;
            break;
        case 1:
            ly = (lDeskHeight - lWinHeight);
            break;
    }
    SetWindowPos(hWnd, HWND_TOPMOST, lx, ly, 0, 0, SWP_NOSIZE);
    return;
}

int fnRandom(int iLimit)
{
    return (int)(float)rand()/(float)RAND_MAX * (float)iLimit;
}

void MessageL(long lData, char* lpTitle)
{
    char cAny[MAX_PATH];
    ltoa(lData, (char*)cAny, 10);
    MessageBox(0, cAny, lpTitle, MB_OK);
    return;    
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


BOOL CALLBACK DlgProc(HWND hWndDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    switch(nMsg)
    {
        case WM_COMMAND:
        {
            case IDOK:
            {
                EndDialog(hWndDlg, 0);
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
}
