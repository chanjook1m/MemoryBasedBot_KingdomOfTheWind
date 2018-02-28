#pragma once
#include "stdafx.h"
#include "stdio.h"
#include "windows.h"
#include "tchar.h"
#include "windows.h"
#include "tlhelp32.h"
#include "tchar.h"
#include "psapi.h"
#include <string.h>

// process name
#define DEF_PROC_NAME L"your_process_name.exe"

// dll path + name
#define DLL_NAME L"C:\\your_dll_name.dll"

DWORD StartNewClient(const char* Directory, char *Application);
DWORD FindPID();
BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath);