/*
* Abanoub aka Mr_PoP criedt this class he wrote it a few months ago to use it in our
* Unborn clientless bot project an i snatched it to use it in this project :P
* Special Thanks to him
*/
#include "stdafx.h"
#include "bytebuffer.h"

ByteBuffer::ByteBuffer(UCHARARRAY buf, int length)
{
	buffer.resize(length);
	memcpy(buffer.data(), buf, length);
	this->length = length;
}

ByteBuffer::ByteBuffer(LPVOID Address, int length)
{
	buffer.resize(length);
	memcpy(buffer.data(), Address, length);
	this->length = length;
}

ByteBuffer::ByteBuffer(int length) {
	buffer.resize(length);
	memset(buffer.data(), 0, length);
	this->length = length;
}

ByteBuffer::~ByteBuffer(void)
{
}

void ByteBuffer::Reverse_At(int StartIndex, int BytesCount)
{
	CHARARRAY Array = new char[BytesCount];
	memcpy(Array, buffer.data() + StartIndex, BytesCount);
	for (int i = 0; i < BytesCount; i++)
	{
		buffer[StartIndex + i] = Array[(BytesCount - 1) - i];
	}
	SAFE_DELETE_ARRAY(Array);
}

UCHARARRAY ByteBuffer::GetBuffer() {
	return this->buffer.data();
}

int ByteBuffer::GetBufferLength() {
	return buffer.size();
}

uint32_t ByteBuffer::ReadUint32(int offset)
{
	return Read<uint32_t>(offset);
}
uint16_t ByteBuffer::ReadUint16(int offset)
{
	return Read<uint16_t>(offset);
}
uint8_t ByteBuffer::ReadByte(int offset)
{
	return Read<uint8_t>(offset);
}
std::vector<uint8_t> ByteBuffer::ReadBytes(int offset, int size) {
	std::vector<uint8_t> result(size, 0);
	memcpy(&result[0], &buffer.data()[offset], size);
	return result;
}
std::string ByteBuffer::ReadString(int offset, int length) {
	std::string str(length, 0);
	strncpy_s(&str[0], length, (const char*)&buffer.data()[offset], length);
	return str;
}

void ByteBuffer::WriteUInt32(uint32_t Value, int offset)
{
	Write<uint32_t>(offset, Value);
}
void ByteBuffer::WriteUInt16(uint16_t Value, int offset)
{
	Write<uint16_t>(offset, Value);
}
void ByteBuffer::WriteByte(uint8_t Value, int offset)
{
	Write<uint8_t>(offset, Value);
}
void ByteBuffer::WriteBytes(int offset, uint8_t * value, int length) {
	memcpy(&buffer.data()[offset], value, length);
}
void ByteBuffer::WriteString(int offset, std::string value, int length) {
	if ((int)value.length() < length) {
		memcpy(&buffer.data()[offset], value.c_str(), value.length());
		memset(&buffer.data()[offset + value.length()], 0, length - value.length());
	}
	else memcpy(&buffer.data()[offset], value.c_str(), length);
}

void ByteBuffer::Appned_Client_Stamp()
{
	buffer.resize(length + 8);
	WriteString(length, "TQClient", 8);
	length += 8;

}

void ByteBuffer::Appned_Server_Stamp()
{
	buffer.resize(length + 8);
	WriteString(length, "TQServer", 8);
	length += 8;

}