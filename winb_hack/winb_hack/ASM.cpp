#include "stdafx.h"
#include "ASM.h"
#include "bytebuffer.h"

#ifndef SAFE_DELETE
#ifdef _DEBUG
#define SAFE_DELETE(p){ if(p) delete p; p=NULL; }
#else
#define SAFE_DELETE(p) { if(p) { try{ delete p; } catch(...){} p=NULL; }}
#endif
#endif

bool Buffer_Equals(CHARARRAY Buffer, CHARARRAY Buffer2, int Buffer2Length)
{
	for (int i = 0; i < Buffer2Length; i++)
		if (Buffer[i] != Buffer2[i])
			return false;
	return true;
}

int Calculate_jump(int CurrentAddress, int DestinationAddress)
{
	return CurrentAddress - (int)(0xFFFFFFFF - DestinationAddress - 4);
}

int Extract_Address(int CurrentAddress, int DestinationAddress)
{
	ByteBuffer Array((LPVOID)DestinationAddress, 4);
	//Array.Reverse_At(0, 4);
	return Calculate_jump(CurrentAddress, Array.ReadUint32(0));
}

void WriteBytesASM(DWORD destAddress, LPVOID patch, DWORD numBytes)
{
	DWORD oldProtect = 0;
	DWORD srcAddress = PtrToUlong(patch);
	VirtualProtect((void*)(destAddress), numBytes, PAGE_EXECUTE_READWRITE, &oldProtect);

	__asm
	{
		nop						// Filler
		nop						// Filler
		nop						// Filler

		mov esi, srcAddress		// Save the address
		mov edi, destAddress	// Save the destination address
		mov ecx, numBytes		// Save the size of the patch
		Start :
		cmp ecx, 0				// Are we done yet?
			jz Exit					// If so, go to end of function

			mov al, [esi]			// Move the byte at the patch into AL
			mov[edi], al			// Move AL into the destination byte
			dec ecx					// 1 less byte to patch
			inc esi					// Next source byte
			inc edi					// Next destination byte
			jmp Start				// Repeat the process
			Exit :
		nop						// Filler
			nop						// Filler
			nop						// Filler
	}
	VirtualProtect((LPVOID)(destAddress), numBytes, oldProtect, &oldProtect);
}

void PatchAddress(int DestinationAddress, LPVOID func, BYTE nopCount)
{
	CHARARRAY nopPatch = new char[nopCount];
	char strPatch[] = "\xe8????";

	*((int*)(strPatch + 1)) = PtrToUlong(func) - (DestinationAddress + 5);

	WriteBytesASM(DestinationAddress, strPatch, 5);

	if (nopCount == 0) return;

	memset(nopPatch, 0x90, nopCount);
	WriteBytesASM(DestinationAddress + 5, nopPatch, nopCount);
	SAFE_DELETE(nopPatch);
}
int FindPattern(int StartAddress, int EndAddress, UCHARARRAY Bytes_To_Find, int Bytes_Length)
{
	int PTR = StartAddress; //begin of memory
	CHARARRAY Buffer = new char[Bytes_Length];
	while (PTR <= EndAddress)   //end of memory 
	{
		memcpy(Buffer, (LPVOID)PTR++, Bytes_Length);
		if (!Buffer) break;
		if (Buffer_Equals(Buffer, (CHARARRAY)Bytes_To_Find, Bytes_Length))
		{
			SAFE_DELETE(Buffer);
			return PTR - 1;
		}
		memset(Buffer, 0, Bytes_Length);
	}
	SAFE_DELETE(Buffer);
	return -1;
}
