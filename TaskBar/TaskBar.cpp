// TaskBar.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "TaskBar.h"
#include "Log.h"

#pragma comment(lib, "shlwapi.lib")

#define WM_TASKBARNOTIFY WM_USER+20
#define WM_TASKBARNOTIFY_MENUITEM_SHOW (WM_USER + 21)
#define WM_TASKBARNOTIFY_MENUITEM_HIDE (WM_USER + 22)
#define WM_TASKBARNOTIFY_MENUITEM_RELOAD (WM_USER + 23)
#define WM_TASKBARNOTIFY_MENUITEM_ABOUT (WM_USER + 24)
#define WM_TASKBARNOTIFY_MENUITEM_EXIT (WM_USER + 25)
#define WM_TASKBARNOTIFY_MENUITEM_REG (WM_USER + 26)
#define WM_TASKBARNOTIFY_MENUITEM_UNREG (WM_USER + 27)
#define WM_TASKBARNOTIFY_MENUITEM_AUTORUN (WM_USER + 28)

constexpr auto NID_UID = 123;
constexpr auto MAX_LOADSTRING = 100;

const WCHAR* APP_NAME = L"Aria2 Manager";
const WCHAR* SCHEME_START = L"aria2://start/";
const WCHAR* SCHEME_BROWSE = L"aria2://browse/path=";


// 全局变量:
HINSTANCE hInst;                                // 当前实例
HWND hWnd;
HWND hConsole;
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名
WCHAR szCommandLine[1024] = L"";
WCHAR szTooltip[512] = L"";
WCHAR szBalloon[512] = L"";
WCHAR szFullPath[MAX_PATH] = L"";
WCHAR szEnvironment[1024] = L"";
volatile DWORD dwChildrenPid;


static DWORD MyGetProcessId(HANDLE hProcess) {
	// https://gist.github.com/kusma/268888
	typedef DWORD(WINAPI* pfnGPI)(HANDLE);
	typedef ULONG(WINAPI* pfnNTQIP)(HANDLE, ULONG, PVOID, ULONG, PULONG);

	static int first = 1;
	static pfnGPI pfnGetProcessId;
	static pfnNTQIP ZwQueryInformationProcess;
	if (first) {
		first = 0;
		pfnGetProcessId = (pfnGPI)GetProcAddress(
			GetModuleHandleW(L"KERNEL32.DLL"), "GetProcessId");
		if (!pfnGetProcessId)
			ZwQueryInformationProcess = (pfnNTQIP)GetProcAddress(
				GetModuleHandleW(L"NTDLL.DLL"),
				"ZwQueryInformationProcess");
	}
	if (pfnGetProcessId)
		return pfnGetProcessId(hProcess);
	if (ZwQueryInformationProcess) {
		struct
		{
			PVOID Reserved1;
			PVOID PebBaseAddress;
			PVOID Reserved2[2];
			ULONG UniqueProcessId;
			PVOID Reserved3;
		} pbi;
		ZwQueryInformationProcess(hProcess, 0, &pbi, sizeof(pbi), 0);
		return pbi.UniqueProcessId;
	}
	return 0;
}


static BOOL MyEndTask(DWORD pid) {
	DWORD DELAYTIME = 2000;
	HANDLE hPrc;

	if (0 == pid) return FALSE;

	hPrc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);  // Opens handle to the process.  

	if (!TerminateProcess(hPrc, 0)) { // Terminates a process.  

		CloseHandle(hPrc);
		return FALSE;
	}
	else {
		WaitForSingleObject(hPrc, DELAYTIME); // At most ,wait 2000  millisecond.  
	}

	CloseHandle(hPrc);
	return TRUE;
}

static BOOL ShowTrayIcon(DWORD dwMessage) {
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = NID_UID;
	nid.uFlags = NIF_ICON | NIF_MESSAGE;
	nid.dwInfoFlags = NIIF_INFO;
	nid.uCallbackMessage = WM_TASKBARNOTIFY;
	nid.hIcon = LoadIcon(hInst, (LPCTSTR)IDI_SMALL);
	nid.uTimeout = 3 * 1000 | NOTIFYICON_VERSION;
	lstrcpy(nid.szInfoTitle, szTitle);
	nid.uFlags |= NIF_INFO | NIF_TIP;
	lstrcpy(nid.szInfo, szBalloon);
	lstrcpy(nid.szTip, szTooltip);
	Shell_NotifyIcon(dwMessage ? dwMessage : NIM_ADD, &nid);
	return TRUE;
}

static BOOL DeleteTrayIcon() {
	NOTIFYICONDATA nid;
	nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA);
	nid.hWnd = hWnd;
	nid.uID = NID_UID;
	Shell_NotifyIcon(NIM_DELETE, &nid);
	return TRUE;
}

static BOOL ShowPopupMenu() {
	POINT pt;
	UINT lcid = GetSystemDefaultLCID();
	BOOL isZHCHS = lcid == 0x0004 || lcid == 0x804 || lcid == 0x1004;
	BOOL isZHCHT = lcid == 0x0404 || lcid == 0x1404 || lcid == 0x0C04 || lcid == 0x7C04;
	BOOL isAutoStart = isAutoStartupSet(APP_NAME);

	HMENU hMenu = CreatePopupMenu();
	AppendMenu(hMenu, MF_STRING, WM_TASKBARNOTIFY_MENUITEM_ABOUT, (isZHCHS ? L"关于" : (isZHCHT ? L"關於" : L"About")));
	if (!IsWindowVisible(hConsole)) {
		AppendMenu(hMenu, MF_STRING, WM_TASKBARNOTIFY_MENUITEM_SHOW, (isZHCHS ? L"显示" : (isZHCHT ? L"顯示" : L"Show")));
	}
	else {
		AppendMenu(hMenu, MF_STRING, WM_TASKBARNOTIFY_MENUITEM_HIDE, (isZHCHS ? L"隐藏" : (isZHCHT ? L"隱藏" : L"Hide")));
	}
	if (!isUrlSchemeReged()) {
		AppendMenu(hMenu, MF_STRING, WM_TASKBARNOTIFY_MENUITEM_REG, (isZHCHS ? L"注册" : (isZHCHT ? L"注冊" : L"Register")));
	}
	else {
		AppendMenu(hMenu, MF_STRING, WM_TASKBARNOTIFY_MENUITEM_UNREG, (isZHCHS ? L"注销" : (isZHCHT ? L"注銷" : L"UnRegister")));
	}
	AppendMenu(hMenu, isAutoStart * MF_CHECKED, WM_TASKBARNOTIFY_MENUITEM_AUTORUN, (isZHCHS ? L"开机启动" : (isZHCHT ? L"開啓啓動" : L"Start on Boot")));
	AppendMenu(hMenu, MF_STRING, WM_TASKBARNOTIFY_MENUITEM_RELOAD, (isZHCHS ? L"重新载入" : (isZHCHT ? L"重新載入" : L"Reload")));
	AppendMenu(hMenu, MF_STRING, WM_TASKBARNOTIFY_MENUITEM_EXIT, (isZHCHS ? L"退出" : (isZHCHT ? L"退出" : L"Exit")));
	GetCursorPos(&pt);
	TrackPopupMenu(hMenu, TPM_LEFTALIGN, pt.x, pt.y, 0, hWnd, NULL);
	PostMessage(hWnd, WM_NULL, 0, 0);
	DestroyMenu(hMenu);
	return TRUE;
}

static BOOL CDCurrentDirectory() {
	GetModuleFileName(NULL, szFullPath, MAX_PATH);
	wchar_t* lastBackslash = wcsrchr(szFullPath, L'\\');
	if (lastBackslash != NULL) {
		*lastBackslash = L'\0';
		SetCurrentDirectory(szFullPath);
		SetEnvironmentVariableW(L"CWD", szFullPath);
		*lastBackslash = L'\\';
		return true;
	}
	else {
		Log::Error(L"Failed to set Current Directory.");
	}
	return false;
}

static BOOL SetEenvironment() {
	BOOL isZHCN = GetSystemDefaultLCID() == 2052;
	if (lstrlenW(szCommandLine) == 0)
		LoadString(hInst, IDS_CMDLINE, szCommandLine, sizeof(szCommandLine) / sizeof(szCommandLine[0]) - 1);
	LoadString(hInst, IDS_ENVIRONMENT, szEnvironment, sizeof(szEnvironment) / sizeof(szEnvironment[0]) - 1);

	const WCHAR* sep = L"\n";
	WCHAR* pos = NULL;
	WCHAR* context = NULL;
	WCHAR* token = wcstok_s(szEnvironment, sep, &context);
	while (token != NULL) {
		if ((pos = wcschr(token, L'=')) != NULL) {
			*pos = 0;
			SetEnvironmentVariableW(token, pos + 1);
			//wprintf(L"[%s] = [%s]\n", token, pos+1);
		}
		token = wcstok_s(NULL, sep, &context);
	}

	GetEnvironmentVariableW(L"TASKBAR_TITLE", szTitle, sizeof(szTitle) / sizeof(szTitle[0]) - 1);
	GetEnvironmentVariableW(L"TASKBAR_TOOLTIP", szTooltip, sizeof(szTooltip) / sizeof(szTooltip[0]) - 1);
	GetEnvironmentVariableW(isZHCN ? L"TASKBAR_BALLOON_CN" : L"TASKBAR_BALLOON", szBalloon, sizeof(szBalloon) / sizeof(szBalloon[0]) - 1);

	return TRUE;
}

static BOOL WINAPI ConsoleHandler(DWORD CEvent) {
	switch (CEvent) {
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_CLOSE_EVENT:
		SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;
	}
	return TRUE;
}

static BOOL CreateConsole() {
	WCHAR szVisible[BUFSIZ] = L"";

	AllocConsole();
	FILE* stream;
	_wfreopen_s(&stream, L"CONIN$", L"r+t", stdin);
	_wfreopen_s(&stream, L"CONOUT$", L"w+t", stdout);

	hConsole = GetConsoleWindow();

	if (GetEnvironmentVariableW(L"TASKBAR_VISIBLE", szVisible, BUFSIZ - 1) && szVisible[0] == L'0') {
		ShowWindow(hConsole, SW_HIDE);
	}
	else {
		SetForegroundWindow(hConsole);
	}

	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE) == FALSE) {
		Log::Error(L"Unable to install handler!\n");
		return FALSE;
	}

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi)) {
		COORD size = csbi.dwSize;
		if (size.Y < 2048) {
			size.Y = 2048;
			if (!SetConsoleScreenBufferSize(GetStdHandle(STD_ERROR_HANDLE), size)) {
				Log::Error(L"Unable to set console screen buffer size!\n");
			}
		}
	}

	HANDLE hStdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	if (hStdHandle != INVALID_HANDLE_VALUE && GetConsoleMode(hStdHandle, &dwMode)) {
		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		SetConsoleMode(hStdHandle, dwMode);
	}

	return TRUE;
}

BOOL ExecCmdline() {
	SetWindowText(hConsole, szTitle);
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = TRUE;
	BOOL bRet = CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi);
	if (bRet) {
		dwChildrenPid = MyGetProcessId(pi.hProcess);
	}
	else {
		Log::Error(L"Execute \"%s\" failed!\n", szCommandLine);
		MessageBox(NULL, L"Error: File 'aria2c.exe' not found.", szCommandLine, MB_OK | MB_ICONERROR);
		ExitProcess(0);
	}
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return TRUE;
}

static BOOL ReloadCmdline() {
	//HANDLE hProcess = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE, FALSE, dwChildrenPid);
	//if (hProcess)
	//{
	//	TerminateProcess(hProcess, 0);
	//}
	ShowWindow(hConsole, SW_SHOW);
	SetForegroundWindow(hConsole);
	wprintf(L"\n\n");
	MyEndTask(dwChildrenPid);
	wprintf(L"\n\n");
	Sleep(200);
	ExecCmdline();
	return TRUE;
}


//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance) {
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TASKBAR));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = (LPCTSTR)NULL;
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
	hInst = hInstance; // 将实例句柄存储在全局变量中

	hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_SYSMENU,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) {
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	//StringCchCopyW(szCommandLine, sizeof(szCommandLine) / sizeof(szCommandLine[0]) - 1, lpCmdLine);
	hInst = hInstance;
	CDCurrentDirectory();
	SetEenvironment();

	// 初始化全局字符串
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TASKBAR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Process URL scheme
	if (wcsstr(lpCmdLine, SCHEME_BROWSE) == lpCmdLine) {// lpCmdLine starts with SCHEME_BROWSE	
		WCHAR path[MAX_PATH];
		StringCchCopyW(path, MAX_PATH, lpCmdLine + lstrlen(SCHEME_BROWSE));
		UrlUnescapeW(path, NULL, NULL, URL_UNESCAPE_INPLACE | URL_UNESCAPE_AS_UTF8);
		if (PathIsDirectoryW(path)) {
			ShellExecute(NULL, L"open", path, NULL, NULL, SW_SHOWNORMAL);
		}
		else {
			HRESULT hr = CoInitialize(NULL);
			PIDLIST_ABSOLUTE pidl = ILCreateFromPathW(path);
			if (SUCCEEDED(hr) && pidl) {
				SHOpenFolderAndSelectItems(pidl, 0, NULL, 0);
				ILFree(pidl);
			}
		}
		exit(0);
	}
	else if (lstrlen(lpCmdLine) != 0 && lstrcmp(lpCmdLine, SCHEME_START) != 0) {
		exit(0);
	}

	// 执行应用程序初始化:
	if (!InitInstance(hInstance, SW_HIDE)) {
		return FALSE;
	}

	CreateConsole();
	ExecCmdline();
	Log::Info(L"Aria2 is starting...\n");
	ShowTrayIcon(NIM_ADD);

	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TASKBAR));
	// 主消息循环:
	while (GetMessage(&msg, nullptr, 0, 0)) {
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	static UINT WM_TASKBARCREATED = 0;
	if (WM_TASKBARCREATED == 0)
		WM_TASKBARCREATED = RegisterWindowMessage(L"TaskbarCreated");

	switch (message) {
	case WM_TASKBARNOTIFY:
		if (lParam == WM_LBUTTONUP) {
			ShowWindow(hConsole, !IsWindowVisible(hConsole));
			SetForegroundWindow(hConsole);
		}
		else if (lParam == WM_RBUTTONUP) {
			SetForegroundWindow(hWnd);
			ShowPopupMenu();
		}
		break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case WM_TASKBARNOTIFY_MENUITEM_SHOW:
			ShowWindow(hConsole, SW_SHOW);
			SetForegroundWindow(hConsole);
			break;
		case WM_TASKBARNOTIFY_MENUITEM_HIDE:
			ShowWindow(hConsole, SW_HIDE);
			break;
		case WM_TASKBARNOTIFY_MENUITEM_REG:
			if (RegUrlScheme()) {
				MessageBox(nullptr, L"Register Aria2 to system successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
			}
			else {
				MessageBox(nullptr, L"Register Aria2 to system failed, please run as administrator!", L"Failure", MB_OK | MB_ICONERROR);
			}
			break;
		case WM_TASKBARNOTIFY_MENUITEM_UNREG:
			if (UnRegUrlScheme()) {
				MessageBox(nullptr, L"Unregister Aria2 from system successfully!", L"Success", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(nullptr, L"Unregister Aria2 from system failed, please run as administrator!", L"Failure", MB_OK | MB_ICONERROR);
			}
			break;
		case WM_TASKBARNOTIFY_MENUITEM_AUTORUN:
			if (isAutoStartupSet(APP_NAME)) {
				disableAutoStartup(APP_NAME);
			}
			else {
				enableAutoStartup(APP_NAME);
			}
			break;
		case WM_TASKBARNOTIFY_MENUITEM_RELOAD:
			ReloadCmdline();
			break;
		case WM_TASKBARNOTIFY_MENUITEM_ABOUT:
			DialogBoxW(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case WM_TASKBARNOTIFY_MENUITEM_EXIT:
			DeleteTrayIcon();
			PostMessage(hConsole, WM_CLOSE, 0, 0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_CLOSE:
		DeleteTrayIcon();
		PostQuitMessage(0);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		if (message == WM_TASKBARCREATED) {
			ShowTrayIcon(NIM_ADD);
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(lParam);
	HWND hwndLink = GetDlgItem(hDlg, IDC_LINK);
	HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);

	HDC hdcStatic = (HDC)wParam;
	HWND hwndStatic = (HWND)lParam;

	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDC_LINK && HIWORD(wParam) == STN_CLICKED) {
			ShellExecute(NULL, L"open", L"https://www.aria2e.com/", NULL, NULL, SW_SHOWNORMAL);
		}
		break;

	case WM_CTLCOLORSTATIC:
		if (hwndStatic == hwndLink) {
			SetTextColor(hdcStatic, RGB(0, 0, 255));  // 设置字体颜色为蓝色
			SetBkMode(hdcStatic, TRANSPARENT);       // 设置背景透明
			return (INT_PTR)hBrush;                           // 返回空画刷
		}
		break;
	case WM_SETCURSOR:
		if (mouseInControl(hDlg, hwndLink)) {
			SetCursor(LoadCursorW(NULL, IDC_HAND));
			SetWindowText(hDlg, L"Click Me");
		}
		else {
			SetCursor(LoadCursorW(NULL, IDC_ARROW));
			SetWindowText(hDlg, L"About");
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL mouseInControl(HWND hDlg, HWND hwndCtrl) {
	POINT pt;
	RECT rect;
	GetCursorPos(&pt);  // 获取当前鼠标的屏幕坐标
	GetWindowRect(hwndCtrl, &rect);  // 获取 hwndCtrl 控件的客户区域
	return PtInRect(&rect, pt);     // 判断鼠标是否在控件区域内
}

BOOL RegUrlScheme() {
	HKEY hKey = nullptr;
	LSTATUS status;
	BOOL result = false;

	// 创建或打开 HKEY_CLASSES_ROOT\aria2 键，失败则创建HKEY_CURRENT_USER\Software\Classes\aria2
	status = RegCreateKeyExW(HKEY_CLASSES_ROOT, L"aria2", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
	if (status != ERROR_SUCCESS) {
		status = RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Classes\\aria2", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKey, nullptr);
	}
	if (status != ERROR_SUCCESS) {
		// 错误处理
		return false;
	}

	// 设置默认值为 "URL:aria2 Protocol"
	status = RegSetValueExW(hKey, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(L"URL:Aria2 Protocol"), (wcslen(L"URL:Aria2 Protocol") + 1) * sizeof(WCHAR));
	if (status != ERROR_SUCCESS) {
		// 错误处理
		RegCloseKey(hKey);
		return false;
	}

	// 创建"URL Protocol"字符串
	status = RegSetValueExW(hKey, L"URL Protocol", 0, REG_SZ, 0, 0);
	if (status != ERROR_SUCCESS) {
		// 错误处理
		RegCloseKey(hKey);
		return false;
	}

	// 创建或打开 .\aria2\shell\open\command 键
	HKEY hKeyCommand = nullptr;
	status = RegCreateKeyExW(hKey, L"shell\\open\\command", 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_WRITE, nullptr, &hKeyCommand, nullptr);
	if (status != ERROR_SUCCESS) {
		// 错误处理
		RegCloseKey(hKey);
		return false;
	}

	// 设置Command值
	WCHAR command[MAX_PATH] = L"";
	GetModuleFileName(NULL, command, sizeof(command) / sizeof(command[0]) - 1);
	StringCchCatW(command, sizeof(command) / sizeof(command[0]) - 1, L" %1");
	status = RegSetValueExW(hKeyCommand, nullptr, 0, REG_SZ, reinterpret_cast<const BYTE*>(command), (lstrlen(command) + 1) * sizeof(WCHAR));
	if (status != ERROR_SUCCESS) {
		// 错误处理
		RegCloseKey(hKeyCommand);
		RegCloseKey(hKey);
		return false;
	}

	// 关闭键
	RegCloseKey(hKeyCommand);
	RegCloseKey(hKey);

	return true;
}

BOOL UnRegUrlScheme() {
	BOOL result = RegDeleteTree(HKEY_CLASSES_ROOT, L"aria2") == ERROR_SUCCESS;
	if (!result) {
		result = RegDeleteTree(HKEY_CURRENT_USER, L"Software\\Classes\\aria2") == ERROR_SUCCESS;
	}

	return result;
}

BOOL isUrlSchemeReged() {
	HKEY hKey;
	BOOL result = false;
	LSTATUS status = RegOpenKeyEx(HKEY_CLASSES_ROOT, L"aria2\\shell\\open\\command", 0, KEY_READ, &hKey);
	if (status != ERROR_SUCCESS) {
		status = RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Classes\\aria2\\shell\\open\\command", 0, KEY_READ, &hKey);
	}
	if (status == ERROR_SUCCESS) {
		DWORD dataSize = 0;
		status = RegQueryValueExW(hKey, NULL, NULL, NULL, NULL, &dataSize);
		if (status == ERROR_SUCCESS) {
			wchar_t* valueData = new wchar_t[dataSize / sizeof(wchar_t)];
			status = RegQueryValueEx(hKey, NULL, NULL, NULL, (BYTE*)valueData, &dataSize);
			if (status == ERROR_SUCCESS) {
				WCHAR command[MAX_PATH] = L"";
				GetModuleFileName(NULL, command, sizeof(command) / sizeof(command[0]) - 1);
				StringCchCatW(command, sizeof(command) / sizeof(command[0]) - 1, L" %1");
				result = wcscmp(valueData, command) == 0;
			}
			delete[] valueData;
		}
		RegCloseKey(hKey);
	}
	return result;
}

BOOL enableAutoStartup(const wchar_t* appName) {
	HKEY hKey;
	BOOL success = false;

	LSTATUS status = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_SET_VALUE,
		&hKey
	);
	if (status == ERROR_SUCCESS) {
		status = RegSetValueEx(
			hKey,
			appName,
			0,
			REG_SZ,
			(BYTE*)szFullPath,
			(wcslen(szFullPath) + 1) * sizeof(wchar_t)
		);

		if (status == ERROR_SUCCESS) {
			Log::Info(L"Start-on-Boot is enabled.\n");
			success = true;
		}
		else {
			Log::Error(L"Failed to enable Start-on-Boot.\n");
		}

		RegCloseKey(hKey);
	}
	else {
		Log::Error(L"Failed to open Registry.");
	}
	return success;
}

BOOL disableAutoStartup(const wchar_t* appName)
{
	HKEY hKey;
	BOOL success = false;

	LSTATUS status = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_SET_VALUE,
		&hKey
	);

	if (status == ERROR_SUCCESS) {
		status = RegDeleteValue(hKey, appName);

		if (status == ERROR_SUCCESS) {
			Log::Info(L"Start-on-Boot is disabled.\n");
			success = true;
		}
		else {
			Log::Error(L"Failed to disable Start-on-Boot.\n");
		}

		RegCloseKey(hKey);
	}
	else {
		Log::Error(L"Failed to open Registry.");
	}
	return success;
}

BOOL isAutoStartupSet(const wchar_t* appName) {
	BOOL result = false;
	HKEY hKey;

	LSTATUS status = RegOpenKeyEx(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_QUERY_VALUE,
		&hKey
	);

	if (status == ERROR_SUCCESS) {
		DWORD dataSize = 0;
		status = RegQueryValueExW(hKey, appName, NULL, NULL, NULL, &dataSize);
		if (status == ERROR_SUCCESS) {
			wchar_t* valueData = new wchar_t[dataSize / sizeof(wchar_t)];

			status = RegQueryValueEx(hKey, appName, NULL, NULL, (BYTE*)valueData, &dataSize);

			if (status == ERROR_SUCCESS) {
				result = wcscmp(valueData, szFullPath) == 0;
			}
			else {
				Log::Error(L"Failed to get the auto-run data from registry\n");
			}
			delete[] valueData;
			RegCloseKey(hKey);
		}
	}
	return result;
}