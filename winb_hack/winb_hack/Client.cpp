#include "stdafx.h"
#include "Client.h"
#include "bytebuffer.h"
#include "tchar.h"
#include <iterator>
#include "Macro.h"

#define DEFAULT_BUFLEN 2000
#define REACHABLE_X_DIST 3
#define REACHABLE_Y_DIST 3 
#define MAX_SAME_POS 3

Client::Client()
{
	
}


Client::~Client()
{
}

// connect function hook callback
void Client::Con_Packet_Hook_Callback()
{
	printf("---- %x\n", hooks->Con_Packet_Return_Address);
	printf("----Socket: %x\n", hooks->Con_Packet_Socket);
}

void Client::Send_Packet_Hook_Callback()
{
	// if you want to see send packets, delete the remark below
	/*
	ByteBuffer Packet((LPVOID)hooks->Outgoing_Packet_Pointer, hooks->Outgoing_Packet_Length);
	std::vector<uint8_t> data = Packet.ReadBytes(0, hooks->Outgoing_Packet_Length);
	std::stringstream result;
	std::copy(data.begin(), data.end(), std::ostream_iterator<int>(result, " "));
	std::string test = result.str();
	printf("Client is sending... : \n");
	for (int i = 0; i < data.size(); i++) {
		printf("%x ", data[i]);
	}
	printf("\n");

	// packet to std::string
	/*
	ByteBuffer Packet((LPVOID)hooks->Outgoing_Packet_Pointer, hooks->Outgoing_Packet_Length);
	printf("Client is sending a packet... Length: %x, Type: %x\n", hooks->Outgoing_Packet_Length, Packet.ReadUint16(0));
	//	Packet.WriteBytes(0, space, 2);
	std::vector<uint8_t> data = Packet.ReadBytes(0, hooks->Outgoing_Packet_Length);
	std::stringstream result;
	std::copy(data.begin(), data.end(), std::ostream_iterator<int>(result, " "));
	std::string test = result.str();
	*/
}

void Client::Recv_Packet_Hook_Callback()
{
	// get HWND for winbaram.exe
	ByteBuffer HWND_Packet((LPVOID)0x0019FC8C, 4);
	std::vector<uint8_t> HWND_data = HWND_Packet.ReadBytes(0, 4);
	unsigned char copied[4] = { "0", };
	int x;
	for (int i = 3; i >= 0; i--) {
		copied[3 - i] = (char)HWND_data[3 - i];
	}
	std::memcpy(&x, copied, 4);
	Macro::macroHWND = (HWND)x;

	//------------------------------

	// get packets
	ByteBuffer Packet((LPVOID)hooks->Ingoing_Packet_Pointer, hooks->Ingoing_Packet_Length);
	std::vector<uint8_t> data = Packet.ReadBytes(0, hooks->Ingoing_Packet_Length);
	std::stringstream result;
	std::copy(data.begin(), data.end(), std::ostream_iterator<int>(result, " "));
	std::string test = result.str();
	//std::size_t found = test.find("186 241 185 208 188 188 192 219 192 199 193 253 49"); //test.find("61 0 0 0");//
	int strLen = 0;
	char nameMsg[DEFAULT_BUFLEN];
	int y;

	// user id number
	if (data[0] == 0x5 && hooks->Ingoing_Packet_Length == 0xd || data[0] == 0x11 && hooks->Ingoing_Packet_Length == 0x7) {
		Macro::id = data[4];
	}

	// map id
	if (data[0] == 0x11 && hooks->Ingoing_Packet_Length) {
		Macro::mapId = data[3];
	}

	// get initial x,y position
	if (data[0] == 0x33 && hooks->Ingoing_Packet_Length == 0x20) {
		Macro::playerX = data[2];
		Macro::playerY = data[4];

		if (Macro::playerX == Macro::prevplayerX && Macro::playerY == Macro::prevplayerY) {
			Macro::samePositionCnt++;
		}

		if (Macro::samePositionCnt > MAX_SAME_POS) {
			Macro::samePositionCnt = 0;
			for (int i = 0; i <= Macro::itemCnt; i++) {
				Macro::itemXY[i].x = 0;
				Macro::itemXY[i].y = 0;
			}
			Macro::returnToInn();
		}

		Macro::prevplayerX = Macro::playerX;
		Macro::prevplayerY = Macro::playerY;
	}

	// get x,y position after movement
	if (data[0] == 0x26 && hooks->Ingoing_Packet_Length == 0xc) {
		Macro::playerX = data[7];
		Macro::playerY = data[9];

		for (int i = 0; i < Macro::itemCnt; i++) {
			if ((Macro::itemXY[i].x == Macro::playerX && Macro::itemXY[i].y == Macro::playerY)) { //|| (abs(Macro::itemXY[i].x - Macro::playerX) > 10 || abs(Macro::itemXY[i].y - Macro::playerY) > 10)) {
				Macro::itemXY[i].x = 0;
				Macro::itemXY[i].y = 0;
			}
		}
	}

	// map name
	if (data[0] == 0x15) {
		int i;
		for (i = 0; i < data[9]; i++) {
			Macro::map[i] = data[10 + i];
		}
		Macro::map[i] = '\0';
	}

	// monster data
	if (data[0] == 0xc && hooks->Ingoing_Packet_Length == 0xb) {
		if (abs(data[6] - Macro::playerX) < REACHABLE_X_DIST && abs(data[8] - Macro::playerY) < REACHABLE_Y_DIST) {
			Macro::mId = data[4];
			Macro::monsterX = data[6];
			Macro::monsterY = data[8];
		}
	}

	// item data
	if (data[0] == 0x7 && hooks->Ingoing_Packet_Length == 0x10) {
		if ((abs(data[4] - Macro::playerX) < REACHABLE_X_DIST && abs(data[6] - Macro::playerY) < REACHABLE_Y_DIST)) { //|| (abs(data[4] - Macro::playerX) > 10 || abs(data[6] - Macro::playerY) > 10)) {
			Macro::itemXY[Macro::itemCnt].x = data[4];
			Macro::itemXY[Macro::itemCnt].y = data[6];
			Macro::itemCnt++;
			if (Macro::itemCnt > 99)
				Macro::itemCnt = 0;
		}
	}

	// check item amount
	if (data[0] == 0xf) {
		Macro::itemAmount = data[hooks->Ingoing_Packet_Length - 3];
	}

	// bonus experience point code
	if (data[0] == 0x30 && hooks->Ingoing_Packet_Length == 0xd0) {
		Macro::bonusExpeOn = true;
		for (int i = 0; i < 5; i++) {
			Macro::bonusExpeCode[i] = data[148 + i];
		}
	}

	// user data
	if (data[0] == 0x8 && hooks->Ingoing_Packet_Length == 0x1a) {
		Macro::hp = data[3] * 0x10000 + data[4] * 0x100 + data[5];
		Macro::mp = data[7] * 0x10000 + data[8] * 0x100 + data[9];
		Macro::expe = data[10] * 0x1000000 + data[11] * 0x10000 + data[12] * 0x100 + data[13];

		printf("hp %d \n", Macro::hp);
		printf("mp %d \n", Macro::mp);
		//printf("id %d \n", Macro::id);
		//printf("exp %d \n", Macro::expe);
		printf("player x %d \n", Macro::playerX);
		printf("player y %d \n", Macro::playerY);
	}

	// chat msg
	if (data[0] == 0xd) {
		for (int i = 0; i < data[6]; i++) {
			Macro::chatMsg[i] = data[7 + i];
		}

		// compare chat msg with a specific string
		if (strstr((char *)Macro::chatMsg, "준비")) {
			Macro::consoleshowtext("[정보] 스크립트 로딩중...");
			luaL_loadfile(Macro::L, "test.lua");
		}
		else if (strstr((char *)Macro::chatMsg, "시작")) {
			Macro::consoleshowtext("[정보] 시작...");
			lua_pcall(Macro::L, 0, LUA_MULTRET, 0);
		}
		else if (strstr((char *)Macro::chatMsg, "중지")) {
			Macro::consoleshowtext("[정보] 중지...");
			lua_pcall(Macro::L, 0, LUA_MULTRET, 0);
		}
	}

	// if you want to see recv packets, delete the remark below
	/*
	printf("Client received a packet... Length: %x\n", hooks->Ingoing_Packet_Length);

	printf("Client received a packet... Data:");
	for (int i = 0; i < data.size(); i++) {
		printf("%x ", data[i]);
	}
	printf("\n");
	/*
	*/
}