#include "stdafx.h"
#include "dll_inject.h"

DWORD StartNewClient(const char* Directory, char *Application)
{
	char CommandLine[256];
	STARTUPINFOA *Startup = new STARTUPINFOA();
	PROCESS_INFORMATION *Process = new PROCESS_INFORMATION();
	sprintf_s(CommandLine, "%s%s", Directory, Application);
	if (!CreateProcessA(NULL, CommandLine, NULL, NULL, FALSE, 0, NULL, Directory, Startup, Process))
	{
		OutputDebugStringA(CommandLine);
		TerminateProcess(Process->hProcess, 0); exit(0);
	}
	InjectDll(Process->dwProcessId, DLL_NAME);
	return Process->dwProcessId;
}

DWORD FindPID()
{
	int i;

	DWORD aProcesses[1024], cbNeeded, cProcesses;
	DWORD dwPID = 0xFFFFFFFF;
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	HANDLE hProcess;
	TCHAR processname[] = DEF_PROC_NAME;

	//  모든 프로세스를 열거한다.
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
		return NULL;

	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for (i = 0; i < cProcesses; i++)
	{

		if (aProcesses[i] != 0)
		{
			//PrintProcessNameAndID(aProcesses[i]);
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, aProcesses[i]);
		}

		// Get the process name.

		if (NULL != hProcess)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
				&cbNeeded))
			{
				GetModuleBaseName(hProcess, hMod, szProcessName,
					sizeof(szProcessName) / sizeof(TCHAR));
			}
		}

		// Print the process name and identifier.


		if (_tcscmp(processname, szProcessName) == 0) {
			_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, aProcesses[i]);
			dwPID = aProcesses[i];
			CloseHandle(hProcess);
			return dwPID;
		}
		// Release the handle to the process.

		//CloseHandle(hProcess);
	}
	if (dwPID == 0xFFFFFFFF)
	{
		printf("There is no <%s> process!\n", DEF_PROC_NAME);
		getchar();
		return 1;
	}
}

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid;

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
		&hToken))
	{
		_tprintf(L"OpenProcessToken error: %u\n", GetLastError());
		return FALSE;
	}

	if (!LookupPrivilegeValue(NULL,           // lookup privilege on local system
		lpszPrivilege,  // privilege to lookup 
		&luid))        // receives LUID of privilege
	{
		_tprintf(L"LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.
	if (!AdjustTokenPrivileges(hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		_tprintf(L"AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		_tprintf(L"The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE hProcess = NULL, hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadProc;

	// #1. dwPID 를 이용하여 대상 프로세스(notepad.exe)의 HANDLE을 구한다.
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		_tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		return FALSE;
	}

	// #2. 대상 프로세스(예:notepad.exe) 메모리에 szDllName 크기만큼 메모리를 할당한다.
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	if (pRemoteBuf == NULL) printf("virtualalloc failed\n");
	// #3. 할당 받은 메모리에 dll 경로를 쓴다.
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);

	// #4. LoadLibraryA() API 주소를 구한다.
	hMod = GetModuleHandle(L"kernel32.dll");
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryW");
	if (pThreadProc == NULL) printf("getprocaddress failed\n");
	// #5. 프로세스에 스레드를 실행
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	if (hThread == NULL) printf("createremotethread failed\n");
	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);
	return TRUE;
}
