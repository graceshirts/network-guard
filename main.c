#include <windows.h>
#include <CommCtrl.h>

#include "resource.h"
#include "rasapi.h"

const char g_szClassName[] = "network-guard";

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		case WM_CREATE:
		{
			HWND hListView;
			hListView = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				WC_LISTVIEW,
				"Network Guard",
				WS_CHILD | WS_VISIBLE | LVS_REPORT,
				0, 0, 560, 350, // x, y, width, height
				hWnd, (HMENU)IDC_LISTVIEW, GetModuleHandle(NULL), NULL);
				
			if (!hListView) {
				MessageBox(NULL, "ListView creation failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
				return 0;
			}
			
			if (!InitListViewColumns(hListView))
				return 0;
			if (!EnumEntries(hListView))
				return 0;
			
		}
		break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
		break;
		case WM_DESTROY:
			PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASSEX wc;
	HWND hWnd;
	MSG msg;
	
	// Registering the window class
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_APPICON));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)1;
	wc.lpszMenuName = MAKEINTRESOURCE(IDR_APPMENU);
	wc.lpszClassName = g_szClassName;
	wc.hIconSm = 0;
	
	if (!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Window registeration failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
	
	hWnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		g_szClassName,
		"Network Guard",
		WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
		400, 200, 560, 350, // x, y, width, height
		NULL, NULL, hInstance, NULL);
		
	if (!hWnd) {
		MessageBox(NULL, "Window creation failed!", "Error", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}
		
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}