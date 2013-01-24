#include "C:\dev\lcc\include\windows.h"
#include "C:\dev\lcc\include\shellapi.h"
#include "C:\dev\lcc\include\commdlg.h"
#include "C:\dev\lcc\include\commctrl.h"
#include "resource.h"
#define  MAX_CHARS 65500
#define  ofn_OFN_ENABLESIZING 0x00800000

const long lWinWidth = 420;
const long lWinHeight = 310;
long       lDeskWidth, lDeskHeight;
char       sFileName[MAX_CHARS];

HWND       hWndMain;
HWND       hWndTxtCreated, hWndTxtAccessed, hWndTxtModified;
HWND       hWndTxtFileName, hWndTxtAppType, hWndTxtAssocProg, hWndTxtDraw;
HWND       hLblFileName, hLblAppType, hLblAssocProg;
HWND       hWndDateCreated, hWndDateAccessed, hWndDateModified;
HWND       hWndTimeCreated, hWndTimeAccessed, hWndTimeModified;
HWND       hCmdBrowse, hCmdUpdate, hCmdOK, hCmdAbout;
HWND       hWndGrp1, hWndGrp2;
HINSTANCE  gAppInst;
HDC        hGenDC;
HICON      hAssocIcon = 0;
LOGFONT    lgfTahoma;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int     fnGetFileNameFromCommandLine(void);
BOOL    fnBrowseFile(HWND);
int     fnGetFileDetails(void);
int     fnGetTimes(void);
int     fnSetTimes(void);
void    APIError(char*);
void    MessageL(long, char*);
BOOL    CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
            LPSTR lpszCmdLine, int nCmdShow)
{
    HWND            hWnd;
    WNDCLASSEX      wcex;
    MSG             nMsg;
    const char*     lpWndClass = "cDateChanger";
    const char*     lpTitle = "File Date/Time Changer";

    gAppInst = hInstance;
    lDeskWidth = GetDeviceCaps(GetDC(GetDesktopWindow()), HORZRES);
    lDeskHeight = GetDeviceCaps(GetDC(GetDesktopWindow()), VERTRES);

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = lpWndClass;
    wcex.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APP));

    RegisterClassEx(&wcex);

    hWnd = CreateWindowEx(WS_EX_WINDOWEDGE, lpWndClass, lpTitle,
                          WS_BORDER | WS_SYSMENU,
                          (lDeskWidth - lWinWidth)/2, (lDeskHeight - lWinHeight)/2,
                          lWinWidth, lWinHeight, NULL, NULL, hInstance, NULL);

    hWndTxtDraw = CreateWindow("STATIC", "", WS_CHILD | SS_SUNKEN,
                               25, 20, 40, 40, hWnd, NULL, hInstance, NULL);
    hLblFileName = CreateWindow("STATIC", "File:", WS_CHILD, 70, 20, 25, 15,
                                hWnd, NULL, hInstance, NULL);
    hLblAppType = CreateWindow("STATIC", "Type:", WS_CHILD, 70, 45, 28, 15,
                               hWnd, NULL, hInstance, NULL);
    hLblAssocProg = CreateWindow("STATIC", "Opens With:", WS_CHILD,
                                 25, 70, 65, 15, hWnd, NULL, hInstance, NULL);

    hWndTxtFileName = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                                     WS_CHILD | ES_READONLY, 100, 20, 260, 17,
                                     hWnd, NULL, hInstance, NULL);
    hWndTxtAppType = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                                    WS_CHILD | ES_READONLY, 100, 43, 290, 17,
                                    hWnd, NULL, hInstance, NULL);
    hWndTxtAssocProg = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", "",
                                      WS_CHILD | ES_READONLY, 90, 69, 300, 17,
                                      hWnd, NULL, hInstance, NULL);
                                 
    hWndTxtCreated = CreateWindow("STATIC", "Created:",
                                  WS_CHILD, 25, 123, 50,
                                  22, hWnd, NULL, hInstance, NULL);
    hWndTxtModified = CreateWindow("STATIC", "Modified:",
                                  WS_CHILD, 25, 163, 50,
                                  22, hWnd, NULL, hInstance, NULL);
    hWndTxtAccessed = CreateWindow("STATIC", "Accessed:",
                                  WS_CHILD, 25, 203, 50,
                                  22, hWnd, NULL, hInstance, NULL);
    hGenDC = GetDC(hWndTxtDraw);
    lgfTahoma.lfHeight = -MulDiv(8, GetDeviceCaps(hGenDC, LOGPIXELSY), 72);
    lgfTahoma.lfWeight = FW_NORMAL;
    lgfTahoma.lfCharSet = ANSI_CHARSET;
    strcpy(lgfTahoma.lfFaceName, "Tahoma");

    hWndDateCreated = CreateWindow(DATETIMEPICK_CLASS, "",
                                   WS_CHILD | DTS_LONGDATEFORMAT, 80, 120, 200,
                                   22, hWnd, NULL, hInstance, NULL);
    hWndDateModified = CreateWindow(DATETIMEPICK_CLASS, "",
                                    WS_CHILD | DTS_LONGDATEFORMAT, 80, 160, 200,
                                    22, hWnd, NULL, hInstance, NULL);
    hWndDateAccessed = CreateWindow(DATETIMEPICK_CLASS, "",
                                    WS_CHILD | DTS_LONGDATEFORMAT, 80, 200, 200,
                                    22, hWnd, NULL, hInstance, NULL);

    hWndTimeCreated = CreateWindow(DATETIMEPICK_CLASS, "",
                                   WS_CHILD | DTS_TIMEFORMAT, 290, 120, 100, 22,
                                   hWnd, NULL, hInstance, NULL);
    hWndTimeModified = CreateWindow(DATETIMEPICK_CLASS, "",
                                    WS_CHILD | DTS_TIMEFORMAT, 290, 160, 100, 22,
                                    hWnd, NULL, hInstance, NULL);
    hWndTimeAccessed = CreateWindow(DATETIMEPICK_CLASS, "",
                                    WS_CHILD | DTS_TIMEFORMAT, 290, 200, 100, 22,
                                    hWnd, NULL, hInstance, NULL);

    hWndGrp1 = CreateWindow("BUTTON", "",
                            WS_CHILD | BS_GROUPBOX, 10, 0, 395, 95,
                            hWnd, NULL, hInstance, NULL);
    hWndGrp2 = CreateWindow("BUTTON", "",
                            WS_CHILD | BS_GROUPBOX, 10, 100, 395, 140,
                            hWnd, NULL, hInstance, NULL);

    hCmdBrowse = CreateWindow("BUTTON", ". . .",
                              WS_CHILD | BS_FLAT, 368, 17, 22, 22,
                              hWnd, NULL, hInstance, NULL);
    hCmdUpdate = CreateWindow("BUTTON", "&Update",
                              WS_CHILD | BS_FLAT, 274, 250, 60, 25,
                              hWnd, NULL, hInstance, NULL);
    hCmdOK = CreateWindow("BUTTON", "&OK",
                          WS_CHILD | BS_FLAT, 344, 250, 60, 25,
                          hWnd, NULL, hInstance, NULL);
    hCmdAbout = CreateWindow("BUTTON", "?",
                          WS_CHILD | BS_FLAT, 10, 255, 15, 15,
                          hWnd, NULL, hInstance, NULL);

    SendMessage(hWndTxtCreated, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hLblFileName, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hLblAppType, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hLblAssocProg, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hWndTxtFileName, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hWndTxtAppType, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hWndTxtAssocProg, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hWndTxtModified, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hWndTxtAccessed, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hWndGrp2, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdUpdate, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdOK, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    lgfTahoma.lfWeight = FW_BOLD;
    SendMessage(hCmdBrowse, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
    SendMessage(hCmdAbout, WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);


    ShowWindow(hWnd, SW_SHOWNORMAL);
    ShowWindow(hWndTxtDraw, SW_SHOWNORMAL);
    ShowWindow(hLblFileName, SW_SHOWNORMAL);
    ShowWindow(hLblAppType, SW_SHOWNORMAL);
    ShowWindow(hLblAssocProg, SW_SHOWNORMAL);
    ShowWindow(hWndTxtFileName, SW_SHOWNORMAL);
    ShowWindow(hWndTxtAppType, SW_SHOWNORMAL);
    ShowWindow(hWndTxtAssocProg, SW_SHOWNORMAL);
    ShowWindow(hWndTxtCreated, SW_SHOWNORMAL);
    ShowWindow(hWndTxtModified, SW_SHOWNORMAL);
    ShowWindow(hWndTxtAccessed, SW_SHOWNORMAL);
    ShowWindow(hWndDateCreated, SW_SHOWNORMAL);
    ShowWindow(hWndDateModified, SW_SHOWNORMAL);
    ShowWindow(hWndDateAccessed, SW_SHOWNORMAL);
    ShowWindow(hWndTimeCreated, SW_SHOWNORMAL);
    ShowWindow(hWndTimeModified, SW_SHOWNORMAL);
    ShowWindow(hWndTimeAccessed, SW_SHOWNORMAL);
    ShowWindow(hWndGrp1, SW_SHOWNORMAL);
    ShowWindow(hWndGrp2, SW_SHOWNORMAL);
    ShowWindow(hCmdBrowse, SW_SHOWNORMAL);
    ShowWindow(hCmdUpdate, SW_SHOWNORMAL);
    ShowWindow(hCmdOK, SW_SHOWNORMAL);
    ShowWindow(hCmdAbout, SW_SHOWNORMAL);

    fnGetFileNameFromCommandLine();
    fnGetFileDetails();
    EnableWindow(hCmdUpdate, FALSE);

    ShowWindow(hWnd, SW_SHOWNORMAL);
    UpdateWindow(hWnd);

    SendMessage(hWnd, WM_PAINT, 0, 0);

    while(GetMessage(&nMsg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&nMsg);
        DispatchMessage(&nMsg);
    }
    return nMsg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR     lpHDR = (LPNMHDR)lParam;
    PAINTSTRUCT ps;
    RECT        rctDraw;

    switch(nMsg)
    {

        case WM_PAINT:
            BeginPaint(hWnd, &ps);
            UpdateWindow(hWndTxtDraw);
            GetClientRect(hWndTxtDraw, &rctDraw);
            FillRect(hGenDC, &rctDraw, CreateSolidBrush(GetSysColor(COLOR_BTNFACE)));
            DrawIconEx(hGenDC, 3, 3, hAssocIcon, 32, 32, 0, NULL, DI_COMPAT | DI_NORMAL);
            EndPaint(hWnd, &ps);
            return TRUE;

        case WM_CLOSE:
            ReleaseDC(hWndTxtDraw, hGenDC);
            DestroyWindow(hWnd);
            PostQuitMessage(0);
            return TRUE;

        case WM_NOTIFY:
            switch(lpHDR->code)
            {
                case DTN_DATETIMECHANGE:
                    if(lpHDR->hwndFrom == hWndDateCreated  ||
                       lpHDR->hwndFrom == hWndDateModified ||
                       lpHDR->hwndFrom == hWndDateAccessed ||
                       lpHDR->hwndFrom == hWndTimeCreated  ||
                       lpHDR->hwndFrom == hWndTimeModified ||
                       lpHDR->hwndFrom == hWndTimeAccessed)
                        EnableWindow(hCmdUpdate, TRUE);
                    break;
            }
            return TRUE;

        case WM_COMMAND:
        {
            if((HWND)lParam == hCmdOK)
                SendMessage(hWnd, WM_CLOSE, 0, 0);

            if((HWND)lParam == hCmdBrowse)
            {
                if(fnBrowseFile(hWnd))
                {
                    fnGetFileDetails();
                    EnableWindow(hCmdUpdate, FALSE);
                    UpdateWindow(hWnd);
                    SendMessage(hWnd, WM_PAINT, 0, 0);
                }
            }

            if((HWND)lParam == hCmdUpdate)
            {
                fnSetTimes();
                EnableWindow(hCmdUpdate, FALSE);
            }

            if((HWND)lParam == hCmdAbout)
                DialogBox(NULL, MAKEINTRESOURCE(IDD_ABOUT), hWnd, DlgProc);
            return TRUE;
        }
    }
    return DefWindowProc(hWnd, nMsg, wParam, lParam);
}

int fnGetFileNameFromCommandLine()
{
    char*   lpCmd;
    char    sExeName[1024];
    int     idx, iLen, iArgStart=0, iArgEnd=0;
    BOOL    bMultArgs = FALSE;

    lpCmd = GetCommandLine();
    iLen = strlen(lpCmd);

    GetModuleFileName(NULL, sExeName, 1024);

    if(strlen(sExeName) + 3 == strlen(lpCmd) || strlen(sExeName) + 1 == strlen(lpCmd))
    {
        bMultArgs = FALSE;
        sFileName[0] = 0;
    }
    else
    {
        if(lpCmd[0] == 34)
            iArgStart = strlen(sExeName) + 3;
        else
            iArgStart = strlen(sExeName) + 1;

        // Quoted Argument
        if(lpCmd[iArgStart] == 34)
        {
            for(idx=iArgStart+1; idx<iLen; idx++)
            {
                if(lpCmd[idx] == 34)
                {
                    iArgEnd = idx+1;
                    break;
                }
            }
        }

        // Non-Quoted Argument
        else
        {
            for(idx=iArgStart; idx<iLen; idx++)
            {
                if(idx == strlen(lpCmd)-1) // 1 Argument
                {
                    iArgEnd = idx+1;
                    break;
                }
                if(lpCmd[idx] == 32) // 1+ Arguments
                {
                    iArgEnd = idx;
                    break;
                }
            }
        }
        for(idx=iArgStart; idx<iArgEnd; idx++)
            sFileName[idx-iArgStart] = lpCmd[idx];
        sFileName[idx-iArgStart] = 0;
    }
    return 0;
}

BOOL fnBrowseFile(HWND hWnd)
{
    BOOL                bFileSelected;
    char                lpBuffer[MAX_CHARS];
    static OPENFILENAME ofn = {0};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFilter = "All Files (*.*)\0*.*\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = lpBuffer;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = MAX_CHARS;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = MAX_CHARS;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Open File for modification";
    ofn.Flags = ofn_OFN_ENABLESIZING | OFN_EXPLORER |
                OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    bFileSelected = GetOpenFileName(&ofn);
    if(bFileSelected)
        strcpy(sFileName, ofn.lpstrFile);
    return bFileSelected;
}


int fnGetFileDetails()
{
    char       sFileWithoutQuotes[MAX_CHARS], sAssocProg[MAX_PATH];
    char       sFileNameForIcon[MAX_CHARS];
    int        idx, jdx;
    SHFILEINFO sfi;
    UINT       cbInfoSize;

    if(strlen(sFileName) <= 0)
    {
        EnableWindow(hWndDateCreated, FALSE);
        EnableWindow(hWndDateAccessed, FALSE);
        EnableWindow(hWndDateModified, FALSE);
        EnableWindow(hWndTimeCreated, FALSE);
        EnableWindow(hWndTimeAccessed, FALSE);
        EnableWindow(hWndTimeModified, FALSE);
        return -1;
    }

    EnableWindow(hWndDateCreated, TRUE);
    EnableWindow(hWndDateAccessed, TRUE);
    EnableWindow(hWndDateModified, TRUE);
    EnableWindow(hWndTimeCreated, TRUE);
    EnableWindow(hWndTimeAccessed, TRUE);
    EnableWindow(hWndTimeModified, TRUE);
    if(sFileName[0] == 34)
    {
        for(idx=1, jdx=0; idx<strlen(sFileName)-1; idx++, jdx++)
            sFileWithoutQuotes[idx-1] = sFileName[idx];
        sFileWithoutQuotes[idx-1] = 0;
        SendMessage(hWndTxtFileName, WM_SETTEXT, 0, (LPARAM)sFileWithoutQuotes);
    }
    else
        SendMessage(hWndTxtFileName, WM_SETTEXT, 0, (LPARAM)sFileName);
    FindExecutable(sFileName, NULL, sAssocProg);
    SendMessage(hWndTxtAssocProg, WM_SETTEXT, 0, (LPARAM)sAssocProg);

    cbInfoSize = sizeof(SHFILEINFO);
    if(sFileName[0] == 34)
    {    
        for(idx=1; idx<strlen(sFileName)-1; idx++)
            sFileNameForIcon[idx-1] = sFileName[idx];
        sFileNameForIcon[idx] = '\0';
        SHGetFileInfo(sFileNameForIcon, FILE_ATTRIBUTE_NORMAL, &sfi, cbInfoSize, SHGFI_ICON);
    }
    else
        SHGetFileInfo(sFileName, FILE_ATTRIBUTE_NORMAL, &sfi, cbInfoSize, SHGFI_ICON);
    
    hAssocIcon = sfi.hIcon;
    SHGetFileInfo(sFileName, FILE_ATTRIBUTE_NORMAL, &sfi, cbInfoSize, SHGFI_TYPENAME);
    SendMessage(hWndTxtAppType, WM_SETTEXT, 0, (LPARAM)sfi.szTypeName);
    fnGetTimes();
    return 0;
}

int fnGetTimes()
{
    HANDLE      hFile;
    FILETIME    ftCreated, ftModified, ftAccessed;
    FILETIME    fltCreated, fltModified, fltAccessed;
    SYSTEMTIME  stCreated, stModified, stAccessed;

    hFile = 0;
    hFile = CreateFile(sFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    GetFileTime(hFile, &ftCreated, &ftAccessed, &ftModified);

    FileTimeToLocalFileTime(&ftCreated, &fltCreated);
    FileTimeToLocalFileTime(&ftModified, &fltModified);
    FileTimeToLocalFileTime(&ftAccessed, &fltAccessed);

    FileTimeToSystemTime(&fltCreated, &stCreated);
    FileTimeToSystemTime(&fltModified, &stModified);
    FileTimeToSystemTime(&fltAccessed, &stAccessed);

    SendMessage(hWndDateCreated, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&stCreated);
    SendMessage(hWndDateModified, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&stModified);
    SendMessage(hWndDateAccessed, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&stAccessed);

    SendMessage(hWndTimeCreated, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&stCreated);
    SendMessage(hWndTimeModified, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&stModified);
    SendMessage(hWndTimeAccessed, DTM_SETSYSTEMTIME, GDT_VALID, (LPARAM)&stAccessed);
    CloseHandle(hFile);
    return 0;
}

int fnSetTimes()
{
    HANDLE      hFile;
    FILETIME    ftCreated, ftModified, ftAccessed;
    FILETIME    fltCreated, fltModified, fltAccessed;
    SYSTEMTIME  sdtCreated, sdtModified, sdtAccessed;
    SYSTEMTIME  sttCreated, sttModified, sttAccessed;
    SYSTEMTIME  stCreated, stModified, stAccessed;

    hFile = 0;
    hFile = CreateFile(sFileName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    SendMessage(hWndDateCreated, DTM_GETSYSTEMTIME, 0, (LPARAM)&sdtCreated);
    SendMessage(hWndDateModified, DTM_GETSYSTEMTIME, 0, (LPARAM)&sdtModified);
    SendMessage(hWndDateAccessed, DTM_GETSYSTEMTIME, 0, (LPARAM)&sdtAccessed);
    SendMessage(hWndTimeCreated, DTM_GETSYSTEMTIME, 0, (LPARAM)&sttCreated);
    SendMessage(hWndTimeModified, DTM_GETSYSTEMTIME, 0, (LPARAM)&sttModified);
    SendMessage(hWndTimeAccessed, DTM_GETSYSTEMTIME, 0, (LPARAM)&sttAccessed);

    stCreated.wYear = sdtCreated.wYear;
    stCreated.wMonth = sdtCreated.wMonth;
    stCreated.wDayOfWeek = sdtCreated.wDayOfWeek;
    stCreated.wDay = sdtCreated.wDay;
    stCreated.wHour = sttCreated.wHour;
    stCreated.wMinute = sttCreated.wMinute;
    stCreated.wSecond = sttCreated.wSecond;
    stCreated.wMilliseconds = sttCreated.wMilliseconds;

    stModified.wYear = sdtModified.wYear;
    stModified.wMonth = sdtModified.wMonth;
    stModified.wDayOfWeek = sdtModified.wDayOfWeek;
    stModified.wDay = sdtModified.wDay;
    stModified.wHour = sttModified.wHour;
    stModified.wMinute = sttModified.wMinute;
    stModified.wSecond = sttModified.wSecond;
    stModified.wMilliseconds = sttModified.wMilliseconds;

    stAccessed.wYear = sdtAccessed.wYear;
    stAccessed.wMonth = sdtAccessed.wMonth;
    stAccessed.wDayOfWeek = sdtAccessed.wDayOfWeek;
    stAccessed.wDay = sdtAccessed.wDay;
    stAccessed.wHour = sttAccessed.wHour;
    stAccessed.wMinute = sttAccessed.wMinute;
    stAccessed.wSecond = sttAccessed.wSecond;
    stAccessed.wMilliseconds = sttAccessed.wMilliseconds;

    SystemTimeToFileTime(&stCreated, &fltCreated);
    SystemTimeToFileTime(&stModified, &fltModified);
    SystemTimeToFileTime(&stAccessed, &fltAccessed);

    LocalFileTimeToFileTime(&fltCreated, &ftCreated);
    LocalFileTimeToFileTime(&fltModified, &ftModified);
    LocalFileTimeToFileTime(&fltAccessed, &ftAccessed);

    SetFileTime(hFile, &ftCreated, &ftAccessed, &ftModified);

    CloseHandle(hFile);
    return 0;
}

BOOL CALLBACK DlgProc(HWND hWndDlg, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    HBRUSH          hbrBlack;
    HDC             hDCDlg;
    PAINTSTRUCT     ps;
    HICON           hIconAbt;
    const COLORREF  crBLACK = RGB(0x00, 0x00, 0x00);
    const COLORREF  crWHITE = RGB(0xFF, 0xFF, 0xFF);
    const COLORREF  crCYAN  = RGB(0x00, 0xFF, 0xFF);
    int             iLimit  = MAX_CHARS;
    long            iCount;
    char            sExeFileNameW7[iLimit], sExeFileName[iLimit];


    switch(nMsg)
    {
        case WM_CTLCOLORDLG:
            hbrBlack = CreateSolidBrush(crBLACK);
            return (LRESULT)hbrBlack;

        case WM_INITDIALOG:
            lgfTahoma.lfHeight = -MulDiv(10, GetDeviceCaps(hGenDC, LOGPIXELSY), 72);
            SendMessage(GetDlgItem(hWndDlg, IDC_ABTTITLE), WM_SETFONT, (WPARAM)CreateFontIndirect(&lgfTahoma), (LPARAM)TRUE);
            return FALSE;

        case WM_CTLCOLORSTATIC:
            hbrBlack = CreateSolidBrush(crBLACK);
            if(GetDlgItem(hWndDlg, IDC_ABTTITLE) == (HWND)lParam)
                SetTextColor((HDC)wParam, crCYAN);
            else
                SetTextColor((HDC)wParam, crWHITE);
            SetBkMode((HDC)wParam, TRANSPARENT);
            return (LRESULT)hbrBlack;

        case WM_PAINT:
            BeginPaint(hWndDlg, &ps);
            hDCDlg = GetDC(hWndDlg);
            GetModuleFileName(NULL, sExeFileNameW7, MAX_CHARS);
            strncpy(sExeFileName, sExeFileNameW7, iLimit);
            ExtractIconEx(sExeFileName, 1, &hIconAbt, NULL, 1);
            DrawIconEx(hDCDlg, 25, 45, hIconAbt, 32, 32, 0, NULL, DI_NORMAL);
            ReleaseDC(hWndDlg, hDCDlg);
            EndPaint(hWndDlg, &ps);
            return TRUE;

        case WM_COMMAND:
        {
            case IDC_BTNOK:
            {
                CloseHandle(hbrBlack);
                DeleteObject(hbrBlack);
                EndDialog(hWndDlg, 0);
                return TRUE;
            }
        }
        break;
    }
    return FALSE;
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

void MessageL(long lData, char* lpTitle)
{
    char cAny[MAX_PATH];
    ltoa(lData, (char*)cAny, 10);
    MessageBox(0, cAny, lpTitle, MB_OK);
    return;    
}
