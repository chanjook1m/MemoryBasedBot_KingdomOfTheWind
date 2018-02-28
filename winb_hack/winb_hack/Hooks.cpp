#include "stdafx.h"
#include "Hooks.h"
#include "Client.h"
#include "Properties.h"
#include <WinSock2.h>

Client* Hooks::Owner;
int Hooks::Outgoing_Packet_Length;
int Hooks::Outgoing_Packet_Pointer;
int Hooks::Outgoing_Packet_Return_Address;

int Hooks::Packet_Encrypt_Return_Address;
int Hooks::Packet_Recv_Return_Address;

int Hooks::Ingoing_Packet_Pointer;
int Hooks::Ingoing_Packet_Length;

int Hooks::Con_Packet_Return_Address;
int Hooks::Con_Packet_Socket = 0;

SOCKET Hooks::sock = 0;
struct sockaddr *Hooks::addr = NULL;
int Hooks::length = 0;

unsigned char Hooks::encrypted[100] = { "0", };

Naked int __cdecl EncryptFunction(char* Source, char* Dest, int SourceSize, char* EncryptString, int EncryptOption)
{
	__asm
	{
	Winbaram_004A8290:
		PUSH EBP
			MOV EBP, ESP
			MOV EAX, DWORD PTR SS : [EBP + 0x10]
			PUSH EBX
			MOV EBX, DWORD PTR SS : [EBP + 0x0C]
			PUSH ESI
			MOV ESI, EAX
			XOR ECX, ECX
			SHR ESI, 2
			PUSH EDI
			MOV EDI, DWORD PTR SS : [EBP + 0x8]
			TEST ESI, ESI
			JLE SHORT Winbaram_004A82CC

			secondjump :
		MOV EAX, ECX
			ADD EDI, 4
			CDQ
			IDIV DWORD PTR SS : [EBP + 0x18]
			MOV EAX, DWORD PTR SS : [EBP + 0x14]
			ADD EBX, 4
			MOV EDX, DWORD PTR DS : [EDX * 4 + EAX]
			MOV EAX, DWORD PTR DS : [EDI - 4]
			XOR EDX, EAX
			INC ECX
			MOV DWORD PTR DS : [EBX - 4], EDX
			CMP ECX, ESI
			JL SHORT secondjump
			MOV EAX, DWORD PTR SS : [EBP + 0x10]
			AND EAX, 0x00000003
			MOV ESI, EAX
			JZ SHORT defaultcase
			MOV EAX, ECX
			CDQ
			IDIV DWORD PTR SS : [EBP + 0x18] //
			MOV EAX, DWORD PTR SS : [EBP + 0x14]
			MOV EDX, DWORD PTR DS : [EDX * 4 + EAX]
			MOV EAX, ESI
			DEC EAX
			JZ SHORT caseone
			DEC EAX
			JZ SHORT casetwo
			DEC EAX
			JNZ SHORT defaultcase
			MOV CX, WORD PTR DS : [EDI]
			XOR CX, DX
			MOV WORD PTR DS : [EBX], CX
			MOV AL, BYTE PTR DS : [EDI + 0x2]
			SHR EDX, 0x10
			XOR DL, AL
			POP EDI
			MOV BYTE PTR DS : [EBX + 0x2], DL
			POP ESI
			POP EBX
			POP EBP
			RETN 0x14

			Winbaram_004A82CC :
			AND EAX, 0x00000003
			MOV ESI, EAX
			JZ SHORT defaultcase
			MOV EAX, ECX
			CDQ

			IDIV DWORD PTR SS : [EBP + 0x18]
			MOV EAX, DWORD PTR SS : [EBP + 0x14]
			MOV EDX, DWORD PTR DS : [EDX * 4 + EAX]
			MOV EAX, ESI
			DEC EAX
			JZ SHORT caseone
			DEC EAX
			JZ SHORT casetwo
			DEC EAX
			JNZ SHORT defaultcase
			MOV CX, WORD PTR DS : [EDI]
			XOR CX, DX
			MOV WORD PTR DS : [EBX], CX
			MOV AL, BYTE PTR DS : [EDI + 0x2]
			SHR EDX, 0x10
			XOR DL, AL
			POP EDI
			MOV BYTE PTR DS : [EBX + 0x2], DL
			POP ESI
			POP EBX
			POP EBP
			RETN 0x14

			defaultcase :
			POP EDI
			POP ESI
			POP EBX
			POP EBP
			RETN 0x14

			caseone :
			MOV CL, BYTE PTR DS : [EDI]
			XOR CL, DL
			MOV BYTE PTR DS : [EBX], CL
			POP EDI
			POP ESI
			POP EBX
			POP EBP
			RETN 0x14

			casetwo :
			MOV AX, WORD PTR DS : [EDI]
			POP EDI
			XOR AX, DX
			POP ESI
			MOV WORD PTR DS : [EBX], AX
			POP EBX
			POP EBP
			RETN 0x14
	}
}

Naked void Hooks::LoadEncrypt(char* Source, int SourceSize, unsigned char* Dest)
{
	__asm
	{
		//Winbaram_004A81A0:
		PUSH EBP
		MOV EBP, ESP
		SUB ESP, 0x10
		MOV EDX, DWORD PTR SS : [EBP + 0x8]
		PUSH EBX
		PUSH ESI
		PUSH EDI
		MOV EDI, DWORD PTR SS : [EBP + 0x10]
		MOV AL, BYTE PTR DS : [EDX]
		MOV DWORD PTR SS : [EBP - 4], ECX
		MOV BYTE PTR DS : [EDI], AL
		MOV AL, BYTE PTR DS : [0x55E2C0]
		MOV BL, AL
		INC AL
		MOV BYTE PTR DS : [0x55E2C0], AL
		MOV EAX, DWORD PTR SS : [EBP + 0x0C]
		MOV BYTE PTR SS : [EBP + 0x8], BL
		MOV BYTE PTR DS : [EDI + 1], BL
		LEA ESI, [EAX - 0x1]
		TEST ESI, ESI
		JG SHORT firstjump
		MOV BYTE PTR DS : [EDI + EAX + 0x1], 0
		POP EDI
		POP ESI
		INC EAX
		POP EBX
		MOV ESP, EBP
		POP EBP
		RETN

		firstjump :
		MOV EBX, 0x00000009
			LEA EAX, [EDI + 0x2]
			PUSH EBX
			PUSH 0x55E2C4
			PUSH ESI
			INC EDX
			PUSH EAX
			PUSH EDX
			MOV DWORD PTR SS : [EBP - 0x8], EAX
			CALL EncryptFunction
			MOV ECX, 0x00000009
			LEA EAX, [ESI - 0x1]
			CDQ
			IDIV ECX
			XOR EBX, EBX
			MOV DWORD PTR SS : [EBP - 0x0C], ECX
			MOV ESI, EAX
			INC ESI
			TEST ESI, ESI
			JLE SHORT secondjump
			MOV EDI, DWORD PTR SS : [EBP - 0x8]
			MOV DWORD PTR SS : [EBP - 0x10], ECX

			secondjump :
		CMP BYTE PTR SS : [EBP + 0x8], BL
			JE SHORT thirdjump
			MOV EAX, DWORD PTR SS : [EBP - 0x0C]
			MOV ECX, EBX
			AND ECX, 0x000000FF
			PUSH 1
			LEA EDX, [ECX * 4 + 0x556C98]
			MOV ECX, DWORD PTR SS : [EBP - 4]
			PUSH EDX
			PUSH EAX
			PUSH EDI
			PUSH EDI
			CALL EncryptFunction


			thirdjump :
		MOV ECX, DWORD PTR SS : [EBP - 0x10]
			INC EBX
			ADD EDI, ECX
			CMP EBX, ESI
			JL SHORT secondjump
			MOV EDI, DWORD PTR SS : [EBP + 0x10]
			MOV ECX, DWORD PTR SS : [EBP + 0x8]
			MOV ESI, DWORD PTR SS : [EBP + 0x0C]
			AND ECX, 0x000000FF
			PUSH 1
			LEA EAX, [ESI - 0x1]
			LEA EDX, [ECX * 4 + 0x556C98]
			MOV ECX, DWORD PTR SS : [EBP - 4]
			PUSH EDX
			PUSH EAX
			LEA EAX, [EDI + 0x2]
			PUSH EAX
			PUSH EAX
			CALL EncryptFunction
			MOV BYTE PTR DS : [EDI + ESI + 0x1], 0
			LEA EAX, [ESI + 0x1]
			POP EDI
			POP ESI
			POP EBX
			MOV ESP, EBP
			POP EBP
			RETN
	}
}

Naked void Hooks::Con_Packet_Hook(void)
{
	__asm
	{
		POP Hooks::Con_Packet_Return_Address
		MOV Hooks::Con_Packet_Return_Address, 0x004A5C0B
		MOV Hooks::Con_Packet_Socket, EAX
		PUSHAD
	}
	Owner->Con_Packet_Hook_Callback();
	__asm
	{
		POPAD
		CALL Properties::Con_Packet_Original_Address
		PUSH Hooks::Con_Packet_Return_Address
		RET
	}
}
Naked void Hooks::Recv_Packet_Hook(void)
{
	__asm
	{
		POP Hooks::Packet_Recv_Return_Address
		CALL Properties::Recv_Packet_Original_Address
		PUSHAD
		PUSHFD
		MOV Hooks::Ingoing_Packet_Pointer, ESI
		MOV Hooks::Ingoing_Packet_Length, EAX
	}
	Owner->Recv_Packet_Hook_Callback();
	__asm
	{
		POPFD
		POPAD
		PUSH Hooks::Packet_Recv_Return_Address
		RET
	}
}
Naked void Hooks::Send_Packet_Hook(void)
{
	__asm
	{
		POP Hooks::Outgoing_Packet_Return_Address // the address after the call
		PUSHAD
		PUSHFD
		MOV EDX, DWORD PTR SS : [EBP + 0x10]
		MOV EDI, DWORD PTR SS : [EBP + 0x0C]
		MOV Hooks::Outgoing_Packet_Length, EDX
		MOV Hooks::Outgoing_Packet_Pointer, EDI
	}
	Owner->Send_Packet_Hook_Callback();

	__asm
	{
		POPFD
		POPAD
		CALL Properties::Send_Packet_Original_Address
		PUSH Hooks::Outgoing_Packet_Return_Address
		RET
	}
}

