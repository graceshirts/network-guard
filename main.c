#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "resource.h"

#define __DEBUG

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define MIN_ARGC 5
#define MAX_ARGC 9
#define MAX_ENTRY_NAME 256
#define MAX_APP_NAME 256
#define MAX_APP_PARAMS 256
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

	if (params.UpdateInterval == 0)
		params.UpdateInterval = UPDATE_INTERVAL;
		
	
	
	return 0;
}