#include <windows.h>
#include <Ras.h>

BOOL RasHlpConnected(HRASCONN hrasconn) {
	RASCONNSTATUS rasconnstatus;
	rasconnstatus.dwSize = sizeof(RASCONNSTATUS);
	
	int dwRet = RasGetConnectStatus(hrasconn, &rasconnstatus);
	
	if (dwRet != ERROR_SUCCESS)
		return FALSE;
	
	if (rasconnstatus.rasconnstate == RASCS_Connected)
		return TRUE;
		
	return FALSE;
}

DWORD RasHlpGetEntryDialParams(LPRASDIALPARAMS lprasdialparam, char szEntryName[], LPBOOL fPassword) {
	DWORD dwRet;
	
	lprasdialparam->dwSize = sizeof(RASDIALPARAMS);
	strncpy_s(lprasdialparam->szEntryName, RAS_MaxEntryName + 1, szEntryName, RAS_MaxEntryName);
	
	dwRet = RasGetEntryDialParams(NULL, lprasdialparam, fPassword);
	
	if (dwRet != ERROR_SUCCESS)
		return dwRet;
		
	return 0;
}

DWORD RasHlpDial(LPRASDIALPARAMS lprasdialparams, LPHRASCONN lphrasconn) {
	int dwRet;
	
	dwRet = RasDial(NULL, NULL, lprasdialparams, NULL, NULL, lphrasconn);
	
	if (dwRet != ERROR_SUCCESS)
		return dwRet;
		
	return 0;
}