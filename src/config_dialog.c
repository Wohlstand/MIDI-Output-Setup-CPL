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
#include <commctrl.h>
#include <commdlg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>

#include "config_dialog.h"
#include "resource.h"

static HINSTANCE s_hModule = NULL;

static void s_loadMidiSynths(HWND hwnd)
{
    HKEY hKey, hSubKey;
    LONG lResult;
    WCHAR device_name[MAX_PATH];
    DWORD dwSize;
    DWORD dwType = REG_SZ;
    int i, device_count, index;
    HWND device_list;

    MIDIOUTCAPSW caps;
    MMRESULT error;

    ZeroMemory(device_name, sizeof(device_name));
    device_count = midiOutGetNumDevs();
    device_list = GetDlgItem(hwnd, IDC_SYNTHLIST);
    lResult  = RegCreateKeyW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Multimedia", &hKey);
    lResult = RegCreateKeyW(hKey, L"MIDIMap", &hSubKey);
    RegQueryValueExW(hSubKey, L"szPname", NULL, &dwType, (LPBYTE)device_name, &dwSize);
    RegCloseKey(hKey);

    for(i = 0; i < device_count; ++i)
    {
        ZeroMemory(&caps, sizeof(caps));
        error = midiOutGetDevCapsW(i, &caps, sizeof(caps));
        if(error == MMSYSERR_NOERROR)
            SendMessageW(device_list, CB_ADDSTRING, 0, (LPARAM) caps.szPname);
    }

    index = SendMessageW(device_list, CB_FINDSTRINGEXACT, -1, (LPARAM)device_name);
    SendMessageW(device_list, CB_SETCURSEL, 0, (LPARAM) index);
}


static void s_setMidiSynth(HWND hwnd)
{
    HKEY hKey, hSubKey;
    long lRet;
    TCHAR device_name[MAX_PATH];
    HWND device_list;
    int selection;
    int text_len;

    ZeroMemory(device_name, MAX_PATH);
    device_list = GetDlgItem(hwnd, IDC_SYNTHLIST);
    selection = SendMessageW(device_list, CB_GETCURSEL, 0, (LPARAM) 0);
    SendMessageW(device_list, CB_GETLBTEXT, selection, (LPARAM)&device_name);
    text_len = (wcslen(device_name) + 1) * sizeof(TCHAR);
    lRet = RegCreateKeyW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Multimedia", &hKey);
    lRet = RegDeleteKeyW(hKey, L"MIDIMap");
    lRet = RegCreateKeyW(hKey, L"MIDIMap", &hSubKey);
    lRet = RegSetValueExW(hSubKey, L"szPname", 0, REG_SZ, (const BYTE *)device_name, (wcslen(device_name) + 1) * sizeof(WCHAR));
    RegCloseKey(hKey);

    if(lRet != ERROR_SUCCESS)
        MessageBoxW(NULL, L"Can't set MIDI registry key.", L"Error!", MB_ICONSTOP);
}

static void s_setApplyEnable(HWND hwnd, int enable)
{
    EnableWindow(GetDlgItem(hwnd, IDC_APPLY), enable);
}


INT_PTR CALLBACK ToolDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    switch(Message)
    {
    case WM_INITDIALOG:
        s_setApplyEnable(hwnd, 0);
        s_loadMidiSynths(hwnd);
        return TRUE;

    case WM_COMMAND:
        switch(LOWORD(wParam))
        {
        case IDC_SYNTHLIST:
            if(HIWORD(wParam) == CBN_SELCHANGE)
                s_setApplyEnable(hwnd, 1);
            break;

        case IDC_APPLY:
            s_setApplyEnable(hwnd, 0);
            s_setMidiSynth(hwnd);
            break;

        case IDOK:
            s_setMidiSynth(hwnd);
            EndDialog(hwnd, IDOK);
            break;

        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;
        }
        break;

    default:
        return FALSE;
    }

    return TRUE;

    UNREFERENCED_PARAMETER(lParam);
}


BOOL runConfigBox(HINSTANCE hModule, HWND hwnd)
{
    s_hModule = hModule;

    DialogBoxW(hModule, MAKEINTRESOURCEW(IDD_MIDISETUP), hwnd, ToolDlgProc);

    s_hModule = NULL;

    return TRUE;
}

BOOL initConfigBox(HINSTANCE hModule, HWND hwnd)
{
    InitCommonControls();
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(hwnd);
    return TRUE;
}

BOOL cleanUpConfigBox(HINSTANCE hModule, HWND hwnd)
{
    UNREFERENCED_PARAMETER(hModule);
    UNREFERENCED_PARAMETER(hwnd);
    return TRUE;
}
