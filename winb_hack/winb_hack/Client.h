#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "stdafx.h"
#include "ASM.h"
#include "stdio.h"
#include "Hooks.h"

#define Naked __declspec (naked)
class Client
{
public:
	Client(void);
	~Client(void);
	void Send_Packet_Hook_Callback();
	void Recv_Packet_Hook_Callback();
	void Con_Packet_Hook_Callback();
	BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

	Hooks* hooks;
};
