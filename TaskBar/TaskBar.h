#pragma once

#include "resource.h"

// �˴���ģ���а����ĺ�����ǰ������:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
BOOL				mouseInControl(HWND hDlg, HWND hwndCtrl);
BOOL				RegUrlScheme();
BOOL				UnRegUrlScheme();
BOOL				isUrlSchemeReged();
BOOL				enableAutoStartup(const wchar_t*);
BOOL				disableAutoStartup(const wchar_t*);
BOOL				isAutoStartupSet(const wchar_t*);