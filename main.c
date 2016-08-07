#include <windows.h>
#include <CommCtrl.h>
#include <Ras.h>

#include "resource.h"
#include "rasapi.h"

const char g_szClassName[] = "network-guard";
HWND hWnd;
HWND hListView;

typedef struct tagEntry {
	int i;
	char szEntryName[RAS_MaxEntryName + 1];
	HRASCONN hRasConn;
} Entry, *LPEntry;

Entry monitoredEntry;

void SwitchRASMenu(BOOL RunEnabled) {
	if (RunEnabled) {
		EnableMenuItem(GetMenu(hWnd), ID_DETECT_STOP, MF_GRAYED);
		EnableMenuItem(GetMenu(hWnd), ID_DETECT_RUN, MF_ENABLED);
	}
	else {
		EnableMenuItem(GetMenu(hWnd), ID_DETECT_RUN, MF_GRAYED);
		EnableMenuItem(GetMenu(hWnd), ID_DETECT_STOP, MF_ENABLED);
	}
}

int GetSelectedItem(HWND hListView) {
	int iCount = ListView_GetItemCount(hListView);
	int i;
	int iRet;
	
	for (i = 0; i < iCount; i++) {
		iRet = ListView_GetItemState(hListView, i, LVIS_SELECTED);
		if (iRet == LVIS_SELECTED)
			return i;
	}
	return -1;
}

BOOL UpdateListView(HWND hListView) {
	SendMessage(hListView, LVM_DELETEALLITEMS, 0, 0);
	if (!EnumEntries(hListView))
		return 0;
	if (!EnumConnections(hListView))
		return 0;
}

void CleanUpDetection() {
	KillTimer(hWnd, IDT_TIMER);
	MessageBox(NULL, "The network is down!", "Error", MB_ICONEXCLAMATION | MB_OK);
	
	UpdateListView(hListView);
	monitoredEntry.i = -1;
	monitoredEntry.szEntryName[0] = '\0';
	monitoredEntry.hRasConn = NULL;
	
	SwitchRASMenu(TRUE);
}

#define DETECTION_INTERVAL 500

void StartDetection() {
	DWORD iRet;
	iRet = GetConnectStatus(monitoredEntry.hRasConn);
	
	if (iRet != 1) {
		CleanUpDetection();
		return;
	}
}

void CALLBACK RasDialFunc1(HRASCONN hrasconn, UINT unMsg, RASCONNSTATE rascs, DWORD dwError, DWORD dwExtendedError)  {
	if (unMsg != WM_RASDIALEVENT)
		return;
		
	if (dwError != 0) {
		SwitchRASMenu(TRUE);
		ListView_SetItemText(hListView, monitoredEntry.i, 3, "Disconnected");
		return;
	}
	
	if (rascs == RASCS_Connected) {
		UpdateListView(hListView);
		SetTimer(hWnd, IDT_TIMER, DETECTION_INTERVAL, StartDetection);
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	switch(msg) {
		case WM_CREATE:
		{
			hListView = CreateWindowEx(
				WS_EX_CLIENTEDGE,
				WC_LISTVIEW,
				"Network Guard",
				WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
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
			if (!EnumConnections(hListView))
				return 0;
		}
		break;
		case WM_COMMAND:
		{
			switch LOWORD(wParam) {
				case ID_FILE_EXIT:
					PostQuitMessage(0);
				break;
				case ID_DETECT_RUN:
				{
					int i = GetSelectedItem(hListView);
					
					UpdateListView(hListView);
					
					if (i == -1) {
						MessageBox(NULL, "Please select a connection!", "Error", MB_ICONEXCLAMATION | MB_OK);
						return 0;
					}
					
					char szText[16];
					
					ListView_GetItemText(hListView, i, 3, szText, sizeof(szText)/sizeof(szText[0]));
					
					if (strcmp(szText, "Disconnected") != 0) {
						MessageBox(NULL, "Only disconnected connection can be selected!", "Error", MB_ICONEXCLAMATION | MB_OK);
						return 0;
					}
					
					monitoredEntry.i = i;
					ListView_GetItemText(hListView, i, 0, monitoredEntry.szEntryName, RAS_MaxEntryName + 1);
					monitoredEntry.hRasConn = NULL;
					
					BOOL Password;
					DWORD iRet;
					RASDIALPARAMS rasDialParams;
					
					rasDialParams.dwSize = sizeof(RASDIALPARAMS);
					strncpy_s(rasDialParams.szEntryName, RAS_MaxEntryName + 1, monitoredEntry.szEntryName, RAS_MaxEntryName);
					
					iRet = RasGetEntryDialParams(NULL, &rasDialParams, &Password);
					
					if (iRet != ERROR_SUCCESS)
						return 0;
						
					iRet = RasDial(NULL, NULL, &rasDialParams, 1, RasDialFunc1, &monitoredEntry.hRasConn);
	
					if (iRet != ERROR_SUCCESS)
						return 0;
						
					SwitchRASMenu(FALSE);
					ListView_SetItemText(hListView, monitoredEntry.i, 3, "Connecting");
				}
				break;
				case ID_DETECT_RELOAD:
					if (!UpdateListView(hListView))
						return 0;
				break;
			}
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
	
	SwitchRASMenu(TRUE);
	
	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return msg.wParam;
}