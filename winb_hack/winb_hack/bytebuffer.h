/*
* Abanoub aka Mr_PoP criedt this class he wrote it a few months ago to use it in our
* Unborn clientless bot project an i snatched it to use it in this project :P
* Special Thanks to him
*/
#ifndef _BYTEBUFFER_H_
#define _BYTEBUFFER_H_

#include <Windows.h>
#include <string>
#include <sstream>
#include <iostream> 
#include <ostream> 
#include <fstream>
#include <Psapi.h>
#include <ctime>
#include <io.h>
#include <fcntl.h>
#include <stdint.h>
#include <vector>
#include "ASM.h"

#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }

class ByteBuffer
{
public:
	ByteBuffer(UCHARARRAY buf, int length);
	ByteBuffer(LPVOID Address, int length);
	ByteBuffer(int length);

	virtual ~ByteBuffer(void);

	void Reverse_At(int StartIndex, int BytesCount);

	UCHARARRAY GetBuffer();
	int GetBufferLength();
	/*Expands the buffer by 8 bytes and writes the TQClient string stamp to the end of the buffer*/
	void Appned_Client_Stamp();
	/*Expands the buffer by 8 bytes and writes the TQServer string stamp to the end of the buffer*/
	void Appned_Server_Stamp();
	uint32_t ReadUint32(int offset);
	uint16_t ReadUint16(int offset);
	uint8_t ReadByte(int offset);
	void WriteUInt32(uint32_t Value, int offset);
	void WriteUInt16(uint16_t Value, int offset);
	void WriteByte(uint8_t Value, int offset);
	template <typename T> void Write(int offset, T value);
	void WriteString(int offset, std::string value, int length);
	void WriteBytes(int offset, UCHARARRAY value, int length);

	template <typename T> T Read(int offset);
	std::string ReadString(int offset, int length);
	std::vector<uint8_t> ReadBytes(int offset, int length);
private:
	std::vector<uint8_t> buffer;
	int length;
};

template<typename T> T ByteBuffer::Read(int offset) {
	return *(T*)&buffer.data()[offset];
}

template <typename T> void ByteBuffer::Write(int offset, T value) {
	*(T*)&buffer.data()[offset] = value;
}

#endif