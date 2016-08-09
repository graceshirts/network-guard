#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "resource.h"
#include "rashelper.h"

#define __DEBUG

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define MIN_ARGC 5
#define MAX_ARGC 9
#define MAX_ENTRY_NAME 256
#define MAX_APP_NAME 256
#define MAX_APP_PARAMS 1024
#define UPDATE_INTERVAL 500

typedef struct tagPARAMS {
	unsigned int argc;
	char EntryName[MAX_ENTRY_NAME + 1];
	char AppName[MAX_APP_NAME + 1];
	char AppParams[MAX_APP_PARAMS + 1];
	unsigned int UpdateInterval;
} PARAMS, *LPPARAMS;

int UnpackParams(LPPARAMS lpParams, char paramName[], char paramValue[]) {
	int iRet;
	
	if (strcmp(paramName, "-c") == 0) {
		iRet = strncpy_s(lpParams->EntryName, MAX_ENTRY_NAME + 1, paramValue, MAX_ENTRY_NAME);
		return iRet; // if success return 0;
	}
	else if (strcmp(paramName, "-a") == 0) {
		iRet = strncpy_s(lpParams->AppName, MAX_APP_NAME + 1, paramValue, MAX_APP_NAME);
		return iRet;
	}
	else if (strcmp(paramName, "-p") == 0) {
		iRet = strncpy_s(lpParams->AppParams, MAX_APP_PARAMS + 1, paramValue, MAX_APP_PARAMS);
		return iRet;
	}
	else if (strcmp(paramName, "-t") == 0) {
		lpParams->UpdateInterval = atoi(paramValue);
		return 0;
	}
	
	return 1;
}

BOOL FileExists(char* szPath) {
	DWORD dwAttrib = GetFileAttributes(szPath);
	
	if (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == 0)
		return TRUE;
	
	return FALSE;
}

int main(int argc, char * argv[]) {
	
	if (argc < MIN_ARGC || argc > MAX_ARGC ) {
		printf("Invalid numbers of parameters!\n");
		return EXIT_FAILURE;
	}
	
#ifdef __DEBUG
	printf("argv=%s\n", argv[0]);
#endif
	
	int i;
	int iRet;
	PARAMS params;
	params.argc = argc;
	strcpy(params.EntryName, "");
	strcpy(params.AppName, "");
	strcpy(params.AppParams, "");
	params.UpdateInterval = UPDATE_INTERVAL;
	
	for (i = 1; i < argc; i+=2) {
		iRet = UnpackParams(&params, argv[i], argv[i+1]);
		if (iRet != 0) {
			printf("Unpacking parameters failed!\n");
			return EXIT_FAILURE;
		}
	}
	
#ifdef __DEBUG
	printf("params.argc=%d\n", params.argc);
	printf("params.EntryName=%s\n", params.EntryName);
	printf("params.AppName=%s\n", params.AppName);
	printf("params.UpdateInterval=%d\n", params.UpdateInterval);
#endif

	if (params.UpdateInterval == 0) {
		printf("UpdateInterval cannot be zero!\n");
		return EXIT_FAILURE;
	}
	
	if (strcmp(params.EntryName, "") == 0) {
		printf("Invalid EntryName!\n");
		return EXIT_FAILURE;
	}
	
	if (strcmp(params.AppName, "") == 0) {
		printf("Invalid AppName!\n");
		return EXIT_FAILURE;
	}
		
	BOOL fPassword;
	RASDIALPARAMS rasdialparams;
	HRASCONN hRasConn;
	
	iRet = RasHlpGetEntryDialParams(&rasdialparams, params.EntryName, &fPassword);
	
	if (iRet != ERROR_SUCCESS) {
		printf("Get entry dial parameters failed!\nError code: %d\n", iRet);
		return EXIT_FAILURE;
	}
	
	rasdialparams.dwSize = sizeof(RASDIALPARAMS);
	hRasConn = NULL;
	
	iRet = RasHlpDial(&rasdialparams, &hRasConn);
	
	if (iRet != ERROR_SUCCESS) {
		printf("Connection failed!\nError code: %d\n", iRet);
		return EXIT_FAILURE;
	}
	
	iRet = FileExists(params.AppName);
	
	if (iRet == FALSE) {
		printf("Specified file does not exist!\n");
		return EXIT_FAILURE;
	}
	
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset(&si, 0, sizeof(si));
	memset(&si, 0, sizeof(pi));

	iRet = CreateProcess(params.AppName, params.AppParams, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	
	if (iRet == FALSE) {
		printf("Cannot create process!\n");
		return EXIT_FAILURE;
	}
	
#ifdef __DEBUG
	printf("pi.PID=%d\n", pi.dwProcessId);
	printf("pi.TID=%d\n", pi.dwThreadId);
#endif
	
	while (RasHlpConnected(hRasConn)) {
		Sleep(params.UpdateInterval);
	}
	
	if (!TerminateProcess(pi.hProcess, 1)) {
		printf("Process termination failed!\nError code:%d\n", GetLastError());
		return EXIT_FAILURE;
	}
	
	if (!TerminateThread(pi.hThread, 1)) {
		printf("Thread termination failed!\nError code:%d\n", GetLastError());
		return EXIT_FAILURE;
	}
	
	if (!CloseHandle(pi.hProcess)) {
		printf("Cannot close process handle!\n");
		return EXIT_FAILURE;
	}
	
	if (!CloseHandle(pi.hThread)) {
		printf("Cannot close thread handlhe!\n");
		return EXIT_FAILURE;
	}
	
	return 0;
}