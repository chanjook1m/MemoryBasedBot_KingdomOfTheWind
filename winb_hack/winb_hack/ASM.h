#pragma once

#include "stdafx.h"
#include <stdint.h>

typedef char * CHARARRAY;
typedef uint8_t* UCHARARRAY;

bool Buffer_Equals(CHARARRAY Buffer, CHARARRAY Buffer2, int Buffer2Length);
int Extract_Address(int CurrentAddress, int DestinationAddress);
int Calculate_jump(int CurrentAddress, int DestinationAddress);
int FindPattern(int StartAddress, int EndAddress, UCHARARRAY Bytes_To_Find, int Bytes_Length);
void WriteBytesASM(DWORD destAddress, LPVOID patch, DWORD numBytes);
void PatchAddress(int DestinationAddress, LPVOID func, BYTE nopCount);