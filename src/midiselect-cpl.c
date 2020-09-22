/*
 * Simple MIDI Output setup utility and control panel applet for Windows Vista/7/8/8.1/10
 * Copyright (c) 2020-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cpl.h>

#include "resource.h"

#include "config_dialog.h"

static HANDLE  hModule = NULL;

BOOL WINAPI DllMain(
    PVOID hmod,
    ULONG ulReason,
    PCONTEXT pctx OPTIONAL
)
{
    if(ulReason != DLL_PROCESS_ATTACH)
    {
        return TRUE;
    }
    else
    {
        hModule = hmod;
    }

    return TRUE;

    UNREFERENCED_PARAMETER(pctx);
}

LONG APIENTRY CPlApplet(
    HWND hwndCPL,       // handle of Control Panel window
    UINT uMsg,          // message
    LONG_PTR lParam1,   // first message parameter
    LONG_PTR lParam2    // second message parameter
)
{
    LPCPLINFO lpCPlInfo;
    LPNEWCPLINFO lpNewCPlInfo;
    LONG retCode = 0;

    switch (uMsg)
    {
    // first message, sent once
    case CPL_INIT:
        initConfigBox(hModule, hwndCPL);
        return TRUE;

    // second message, sent once
    case CPL_GETCOUNT:
        return 1L;

    // third message, sent once per app
    case CPL_INQUIRE:
        lpCPlInfo = (LPCPLINFO)lParam2;
        lpCPlInfo->idIcon = IDI_ICON1;
        lpCPlInfo->idName = IDC_DRIVERNAME;
        lpCPlInfo->idInfo = IDC_DRIVERDESC;
        lpCPlInfo->lData = 0L;
        break;

    // third message, sent once per app
    case CPL_NEWINQUIRE:
        lpNewCPlInfo = (LPNEWCPLINFO)lParam2;
        lpNewCPlInfo->dwSize = (DWORD) sizeof(NEWCPLINFO);
        lpNewCPlInfo->dwFlags = 0;
        lpNewCPlInfo->dwHelpContext = 0;
        lpNewCPlInfo->lData = 0;
        lpNewCPlInfo->hIcon = LoadIconW(hModule, (LPCTSTR)MAKEINTRESOURCEW(IDI_ICON1));
        lpNewCPlInfo->szHelpFile[0] = '\0';

        LoadStringW(hModule, IDC_DRIVERNAME, lpNewCPlInfo->szName, 32);
        LoadStringW(hModule, IDC_DRIVERDESC, lpNewCPlInfo->szInfo, 64);
        break;

    // application icon selected
    case CPL_SELECT:
        break;

    // application icon double-clicked
    case CPL_DBLCLK:
        runConfigBox(hModule, hwndCPL);
        break;

    case CPL_STOP:
        break;

    case CPL_EXIT:
        cleanUpConfigBox(hModule, hwndCPL);
        break;

    default:
        break;
    }

    return retCode;

    UNREFERENCED_PARAMETER(lParam1);
}
