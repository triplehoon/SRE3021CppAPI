#include <stdarg.h>
#include <string.h>
#include <tchar.h>
#include "pGnuplotU.h"

#pragma warning(disable: 4996)

#define PARENTCLASS _T("wgnuplot_parent")
#define TEXTCLASS   _T("wgnuplot_text")
#define GRAPHCLASS  _T("wgnuplot_graph")

BOOL CALLBACK cbGetTextWindow(HWND  hWnd, LPARAM  lParam)
{
	CpGnuplotU* plot = (CpGnuplotU*)lParam;

	HWND hWndFinded = FindWindowEx(hWnd, NULL, TEXTCLASS, NULL);
	if (hWnd && hWndFinded) {
		plot->_hWndParent = hWnd;
		plot->_hWndText = hWndFinded;
	}
	return TRUE;
}

BOOL CALLBACK cbGetGraphWindow(HWND  hWnd, LPARAM  lParam)
{
	CpGnuplotU* plot = (CpGnuplotU*)lParam;

	HWND hWndFinded = FindWindowEx(hWnd, NULL, GRAPHCLASS, NULL);
	if (hWndFinded) {
		plot->_hWndGraph = hWndFinded;
	}
	return TRUE;
}

CpGnuplotU::CpGnuplotU(const WCHAR* gnuplotName)
{
	_hWndParent = NULL;
	_hWndText = NULL;
	_hWndGraph = NULL;

	STARTUPINFO si;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

	PROCESS_INFORMATION pi;
	ZeroMemory(&pi, sizeof(pi));

	WCHAR path[MAX_PATH + 1];
	wcsncpy(path, gnuplotName, MAX_PATH);

	BOOL bSuccess = CreateProcess(NULL, path, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!bSuccess) return;

	for (int i = 0; i < 100 && !_hWndText; ++i) {
		EnumThreadWindows(pi.dwThreadId, cbGetTextWindow, (LPARAM)this);
		Sleep(10);
	}

	cmd(_T("clear"));

	for (int i = 0; i < 100 && !_hWndGraph; ++i) {
		EnumThreadWindows(pi.dwThreadId, cbGetGraphWindow, (LPARAM)this);
		Sleep(10);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (_hWndParent) {
		::ShowWindow(_hWndParent, SW_HIDE);
	}
}

CpGnuplotU::~CpGnuplotU()
{
	if (_hWndGraph)	::SendMessage(_hWndGraph, WM_CLOSE, 0, 0);
	if (_hWndText)	::SendMessage(_hWndText, WM_CLOSE, 0, 0);
	if (_hWndParent) ::SendMessage(_hWndParent, WM_CLOSE, 0, 0);
}

void CpGnuplotU::cmd(const WCHAR* format, ...)
{
	va_list ap;
	WCHAR cmd[1024];

	va_start(ap, format);
	vswprintf_s(cmd, sizeof(cmd) / sizeof(WCHAR), format, ap);
	va_end(ap);

	for (WCHAR* p = cmd; *p; ++p) {
		PostMessage(_hWndText, WM_CHAR, *p, 1);
	}
	PostMessage(_hWndText, WM_CHAR, '\n', 1);
}