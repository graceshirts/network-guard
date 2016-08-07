#include <windows.h>
#include <Ras.h>
#include <CommCtrl.h>

#include "resource.h"
#include "rasapi.h"

BOOL InitListViewColumns(HWND hListView) {
	char szText[128];
	LVCOLUMN lvc;
	int iCol;
	
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	for (iCol = 0; iCol < C_COLUMNS; iCol++) {
		lvc.iSubItem = iCol;
		lvc.pszText = szText;
		lvc.cx = 140;
		lvc.fmt = LVCFMT_LEFT;
		
		LoadString(GetModuleHandle(NULL), IDS_COLUMN_ENTRYNAME + iCol,
			szText,
			sizeof(szText)/sizeof(szText[0]));
			
		if (ListView_InsertColumn(hListView, iCol, &lvc) == -1)
			return FALSE;
	}
	
	return TRUE;
}

#define ERROR_BUFFER_TOO_SMALL 603

int GetConnectStatus(HRASCONN hRasConn) {
	int iRet;
	RASCONNSTATUS RasConnStatus;
	
	RasConnStatus.dwSize = sizeof(RASCONNSTATUS);
	iRet = RasGetConnectStatus(hRasConn, &RasConnStatus);
	
	if (iRet != ERROR_SUCCESS)
		return -1;
	
	if (RasConnStatus.rasconnstate == RASCS_Connected)
		return 1;
	
	return 0;
}

BOOL EnumConnections(HWND hListView) {
	DWORD dwCb = 0;
    DWORD dwRet = ERROR_SUCCESS;
    DWORD dwConnections = 0;
    LPRASCONN lpRasConn = NULL;
    
    // Call RasEnumConnections with lpRasConn = NULL. dwCb is returned with the required buffer size and 
    // a return code of ERROR_BUFFER_TOO_SMALL
    dwRet = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);

    if (dwRet == ERROR_BUFFER_TOO_SMALL){
        // Allocate the memory needed for the array of RAS structure(s).
        lpRasConn = (LPRASCONN) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
        if (lpRasConn == NULL){
            wprintf(L"HeapAlloc failed!\n");
            return 0;
        }
        // The first RASCONN structure in the array must contain the RASCONN structure size
        lpRasConn[0].dwSize = sizeof(RASCONN);
        
        // Call RasEnumConnections to enumerate active connections
        dwRet = RasEnumConnections(lpRasConn, &dwCb, &dwConnections);

        // If successful, print the names of the active connections.
        if (ERROR_SUCCESS == dwRet){
            for (DWORD i = 0; i < dwConnections; i++){
				int iRet;
				iRet = GetConnectStatus(lpRasConn[i].hrasconn);
				
				if (!iRet)
					continue;
				
				LVFINDINFO lvF;
				
				lvF.flags = LVFI_STRING;
				lvF.psz = lpRasConn[i].szEntryName;
				
				int lvRet = ListView_FindItem(hListView, -1, &lvF);
				
				if (lvRet != -1) {
					ListView_SetItemText(hListView, lvRet, 3, "Connected");
				}
			}
        }
        //Deallocate memory for the connection buffer
        HeapFree(GetProcessHeap(), 0, lpRasConn);
        lpRasConn = NULL;
        return 0;
    }

    // There was either a problem with RAS or there are no connections to enumerate    
    if(dwConnections >= 1){
        wprintf(L"The operation failed to acquire the buffer size.\n");
    }else{
        wprintf(L"There are no active RAS connections.\n");
    }

    return 0;
}

BOOL EnumEntries(HWND hListView) {
	DWORD dwCb = 0;
    DWORD dwRet = ERROR_SUCCESS;
    DWORD dwEntries = 0;
    LPRASENTRYNAME lpRasEntryName = NULL;
	
	dwRet = RasEnumEntries(NULL, NULL, lpRasEntryName, &dwCb, &dwEntries);
	
	if (dwRet == ERROR_BUFFER_TOO_SMALL) {
		lpRasEntryName = (LPRASENTRYNAME) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwCb);
		if (!lpRasEntryName)
			return 0;
			
		lpRasEntryName[0].dwSize = sizeof(RASENTRYNAME);
		
		dwRet = RasEnumEntries(NULL, NULL, lpRasEntryName, &dwCb, &dwEntries);
		if (ERROR_SUCCESS == dwRet) {
			for (DWORD i = 0; i < dwEntries; i++) {
				LVITEM lvI;
				lvI.pszText = lpRasEntryName[i].szEntryName;
				lvI.mask = LVIF_TEXT | LVIF_IMAGE |LVIF_STATE;
				lvI.stateMask = 0;
				lvI.iSubItem = 0;
				lvI.state = 0;
				
				if (ListView_InsertItem(hListView, &lvI) == -1)
					return FALSE;
					
				if (!GetEntryProperties(hListView, lpRasEntryName[i].szEntryName, i))
					return FALSE;
					
				ListView_SetItemText(hListView, i, 3, "Disconnected");
			}
		}
		
		HeapFree(GetProcessHeap(), 0, lpRasEntryName);
		lpRasEntryName = NULL;
		return TRUE;
	}
	
	if (dwEntries >= 1)
		MessageBox(NULL, "The operation failed to acquire the buffer size.", "Error", MB_ICONEXCLAMATION | MB_OK);
	else
		MessageBox(NULL, "There are no active RAS connections.", "Error", MB_ICONEXCLAMATION | MB_OK);
	return 0;

}

BOOL GetEntryProperties(HWND hListView, LPCTSTR lpszEntry, int i) {
	DWORD dwEnt = 0;
	DWORD dwRet = ERROR_SUCCESS;
	LPRASENTRY lpRasEntry;
	
	dwRet = RasGetEntryProperties(NULL, NULL, lpRasEntry, &dwEnt, NULL, NULL);
	
	if (dwRet == ERROR_BUFFER_TOO_SMALL) {
		lpRasEntry = (LPRASENTRY) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwEnt);
		
		if (!lpRasEntry)
			return 0;
		
		(*lpRasEntry).dwSize = sizeof(RASENTRY);
		dwRet = RasGetEntryProperties(NULL, lpszEntry, lpRasEntry, &dwEnt, NULL, NULL);
		
		if (ERROR_SUCCESS == dwRet) {
			ListView_SetItemText(hListView, i, 1, (*lpRasEntry).szDeviceType);
			ListView_SetItemText(hListView, i, 2, (*lpRasEntry).szDeviceName);
		}
		
		HeapFree(GetProcessHeap(), 0, lpRasEntry);
		lpRasEntry = NULL;
		return TRUE;
	}
	return FALSE;
}