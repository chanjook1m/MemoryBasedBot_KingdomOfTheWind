#include "stdafx.h"
#include "Macro.h"
#include "Hooks.h"
#include "process.h"

// HWND
HWND Macro::macroHWND = NULL;

// game data
unsigned int Macro::hp = 0, Macro::mp = 0;
unsigned int Macro::expe = 0;
unsigned int Macro::bonusExpeCode[5] = { 0, };
unsigned int Macro::bonusExpeOn = false;
unsigned short Macro::id = 0;
unsigned short Macro::mapId = 0;
unsigned short Macro::playerX = 0, Macro::playerY = 0;
unsigned short Macro::prevplayerX = 0, Macro::prevplayerY = 0;
unsigned short Macro::mId = 0;
unsigned short Macro::monsterX = 0, Macro::monsterY = 0;
Macro::Point Macro::itemXY[100] = { 0, };
unsigned char Macro::map[100] = { 0, };
unsigned char Macro::chatMsg[100] = { 0, };
unsigned int Macro::samePositionCnt = 0;
unsigned int Macro::itemAmount = 0;
int Macro::itemCnt = 0;

// lua state
lua_State* Macro::L = luaL_newstate();

// INTERVAL_DATA structure for passing datas to thread
typedef struct Data {
	double msec;
	const char *funcName;
	BOOL threadCheck = false;
	lua_State* lua;
}INTERVAL_DATA;

INTERVAL_DATA data;

Macro::Macro()
{
}

Macro::~Macro()
{
}

void Macro::clearInterval(lua_State* lua) {
	std::string str = "";
	std::string startMsg = (std::string) lua_tostring(lua, 1);
	if (strlen((const char *)Macro::chatMsg)) {
		str = (const char *)Macro::chatMsg;
	}

	if (str.find(startMsg) > 0) {
		data.threadCheck = true;
	}
}

unsigned WINAPI gameThread(void* arg)
{
	LARGE_INTEGER Frequency;
	LARGE_INTEGER BeginTime;
	LARGE_INTEGER Endtime;
	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&BeginTime);

	double interval = data.msec / 1000.0;

	while (1) {
		if (data.threadCheck) {
			data.threadCheck = false;
			_endthreadex(0);
		}

		QueryPerformanceCounter(&Endtime);

		int elapsed = Endtime.QuadPart - BeginTime.QuadPart;

		double duringtime = (double)elapsed / (double)Frequency.QuadPart;
		duringtime = ceilf(duringtime * 10) / 10;

		if ((int)(duringtime * 100000) % (int)(interval * 100000) == 0) {
			Sleep(data.msec);
			lua_getglobal((lua_State*)arg, data.funcName);
			lua_call((lua_State*)arg, 0, 0);
		}
	}

	return 0;
}

unsigned char byteSwap(unsigned short n) {
	return ((n >> 2) | (n << 2));
}


unsigned char getLeftByte(unsigned short n) {
	return (unsigned char)(n >> 8);
}


unsigned char getRightByte(unsigned short n) {
	return (unsigned char)(n & 0xFF);
}

unsigned char * Macro::korStrToHex(const char *korStr) {
	size_t charCount = strlen(korStr);
	static unsigned char buffer[BUFSIZ] = { 0, };

	for (size_t i = 0; i < charCount; i++) {
		unsigned char cc = *((unsigned char *)korStr + i);
		buffer[i] = cc;
	}
	return buffer;
}

int Macro::mapname(lua_State* lua) {
	std::string str = "";
	str = (const char *)Macro::map;
	lua_pushfstring(lua, (const char *)Macro::map);
	return 1;
}

void Macro::consoleshowtext(const char *korStr) {
	static unsigned char text[BUFSIZ] = { 0, };
	unsigned char *hex = Macro::korStrToHex(korStr);
	int length = strlen(korStr);
	int i;

	text[0] = 0x0A;
	text[1] = 0x00;
	text[2] = 0x00;
	text[3] = length + 1;

	for (i = 4; i < length + 5; i++)
		text[i] = hex[i - 4];
	text[i] = 0x00;

	PostMessage(Macro::macroHWND, WM_USER + 3, (WPARAM)text, length + 6);
}
void Macro::showtext(lua_State *lua) {

	static unsigned char text[BUFSIZ] = { 0, };
	const char *korStr = lua_tostring(lua, 1);
	unsigned char *hex = Macro::korStrToHex(korStr);
	int length = strlen(korStr);
	int i;

	text[0] = 0x0A;
	text[1] = 0x00;
	text[2] = 0x00;
	text[3] = length + 1;

	for (i = 4; i < length + 5; i++)
		text[i] = hex[i - 4];
	text[i - 1] = 0x00;

	PostMessage(Macro::macroHWND, WM_USER + 3, (WPARAM)text, length + 6);
}

void Macro::pri(lua_State* lua)
{
	printf("lua print test\n");
}

void Macro::keypress(lua_State* lua) {
	int key = lua_tonumber(lua, 1);
	PostMessage(Macro::macroHWND, 0x100, key, 0);//22020097); // key down
	Sleep(100);
	PostMessage(Macro::macroHWND, 0x101, key, 0);//22020097); // key up
	Sleep(100);
}

void Macro::keydown(lua_State* lua) {
	int key = lua_tonumber(lua, 1);
	PostMessage(Macro::macroHWND, 0x100, key, 0);//22020097); // key down
}

void Macro::keyup(lua_State* lua) {
	int key = lua_tonumber(lua, 1);
	PostMessage(Macro::macroHWND, 0x101, key, 0);//22020097); // key up
}

void Macro::move_char(int direction) {
	int size = 16;
	char packet[16] = { 0x6, 0x2, 0x80, 0x0, 0x9, 0x0, 0x14, 0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	packet[1] = direction;
	packet[4] = Macro::playerX;
	packet[6] = Macro::playerY;
	unsigned char sendpacket[100] = { "0", };
	Hooks::LoadEncrypt(packet, size, Hooks::encrypted);
	sendpacket[0] = 0xAA;
	sendpacket[1] = 0x00;
	sendpacket[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		sendpacket[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)sendpacket, size + 4, 0);
}
void Macro::hp_charge(lua_State* lua) {
	int size = 11;
	char real[11] = { 0xf, 0x3, 0x0, 0x0, 0x3, 0x80, 0x0, 0xa, 0x0, 0x10, 0x0 };
	real[4] = Macro::mapId;
	real[5] = Macro::id;
	printf("x,y: %d, %d\n", Macro::playerX, Macro::playerY);
	real[7] = Macro::playerX;
	real[9] = Macro::playerY;

	unsigned char clear[100] = { "0", };
	Hooks::LoadEncrypt(real, size, Hooks::encrypted);
	clear[0] = 0xAA;
	clear[1] = 0x00;
	clear[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		clear[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)clear, size + 4, 0);
}

void Macro::mp_charge(lua_State* lua) {
	int size = 4;
	char real[4] = { 0xf, 0x2, 0x0, 0x0 };
	unsigned char clear[100] = { "0", };
	Hooks::LoadEncrypt(real, size, Hooks::encrypted);

	clear[0] = 0xAA;
	clear[1] = 0x00;
	clear[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		clear[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)clear, size + 4, 0);
}

void Macro::returnToInn() {
	int size = 4;
	char real[4] = { 0xf, 0x6, 0x0, 0x0 };
	unsigned char clear[100] = { "0", };
	Hooks::LoadEncrypt(real, size, Hooks::encrypted);

	clear[0] = 0xAA;
	clear[1] = 0x00;
	clear[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		clear[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)clear, size + 4, 0);
}

void Macro::sleep(lua_State* lua) {
	int miliSec = lua_tonumber(lua, 1);
	Sleep(miliSec);
}

void Macro::attack(lua_State* lua) {
	if (Macro::mId == 0)
		return;
	int size = 11;
	char packet[11] = { 0xf, 0x4, 0x0, 0x0, 0x3, 0x80, 0x0, 0xa, 0x0, 0x10, 0x0 };
	packet[4] = Macro::mapId;
	packet[5] = Macro::mId;
	packet[7] = Macro::monsterX;
	packet[9] = Macro::monsterY;

	unsigned char sendpacket[100] = { "0", };
	Hooks::LoadEncrypt(packet, size, Hooks::encrypted);

	sendpacket[0] = 0xAA;
	sendpacket[1] = 0x00;
	sendpacket[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		sendpacket[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)sendpacket, size + 4, 0);

	Macro::mId = 0;
}

int Macro::pc_x(lua_State* lua) {
	lua_pushnumber(lua, Macro::playerX);
	return 1; // number of return value
}

int Macro::pc_y(lua_State* lua) {
	lua_pushnumber(lua, Macro::playerY);
	return 1;
}

int Macro::pc_hp(lua_State* lua) {
	lua_pushnumber(lua, Macro::hp);
	return 1;
}

int Macro::pc_mp(lua_State* lua) {
	lua_pushnumber(lua, Macro::mp);
	return 1;
}

int Macro::sell_exp(lua_State* lua) {
	int size = 3;
	char packet[11] = { 0x1c, 0x9, 0x0 };

	unsigned char sendpacket[100] = { "0", };
	Hooks::LoadEncrypt(packet, size, Hooks::encrypted);

	sendpacket[0] = 0xAA;
	sendpacket[1] = 0x00;
	sendpacket[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		sendpacket[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)sendpacket, size + 4, 0);
}

int Macro::pc_exp(lua_State* lua) {
	lua_pushnumber(lua, Macro::expe);
	return 1;
}

int Macro::item_xy(lua_State* lua) {
	lua_newtable(lua);

	for (int i = 0; i <= Macro::itemCnt; i++) {
		lua_newtable(lua);

		lua_pushnumber(lua, Macro::itemXY[i].x);
		lua_rawseti(lua, -2, 1);
		lua_pushnumber(lua, Macro::itemXY[i].y);
		lua_rawseti(lua, -2, 2);
		lua_pushnumber(lua, Macro::itemCnt);
		lua_rawseti(lua, -2, 3);
		lua_rawseti(lua, -2, i + 1);
	}

	return 1;
}


void Macro::add_command(lua_State* lua) {
	std::string str = "";
	std::string startMsg = (std::string) lua_tostring(lua, 1);
	const char * funcName = lua_tostring(lua, 2);


	lua_getglobal(lua, funcName);
	if (strlen((const char *)Macro::chatMsg)) {
		str = (const char *)Macro::chatMsg;
	}
	if (str.find(startMsg) != -1) {
		// call the function with 0 arguments, return 0 result
		lua_pcall(Macro::L, 0, LUA_MULTRET, 0);
		lua_call(lua, 0, 0);
	}
}

void Macro::cardinal_direction(lua_State* lua) {
	int size = 4;
	int direction = lua_tonumber(lua, 1);
	char real[4] = { 0xf, 0x5, 0x33, 0x0 }; // 31 = µ¿, 32 = ¼­, 33 = ³², 34 = ºÏ
	real[2] = direction;
	unsigned char clear[100] = { "0", };
	Hooks::LoadEncrypt(real, size, Hooks::encrypted);

	clear[0] = 0xAA;
	clear[1] = 0x00;
	clear[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		clear[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)clear, size + 4, 0);
}

void Macro::refresh(lua_State* lua) {
	int size = 2;
	char real[4] = { 0x38, 0x0 };
	unsigned char clear[100] = { "0", };
	Hooks::LoadEncrypt(real, size, Hooks::encrypted);

	clear[0] = 0xAA;
	clear[1] = 0x00;
	clear[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		clear[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)clear, size + 4, 0);
}

void Macro::eat() {
	int size = 3;
	char real[4] = { 0x7, 0x1, 0x0 };
	unsigned char clear[100] = { "0", };
	Hooks::LoadEncrypt(real, size, Hooks::encrypted);

	clear[0] = 0xAA;
	clear[1] = 0x00;
	clear[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		clear[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)clear, size + 4, 0);
}

void Macro::setInterval(lua_State* lua) {
	data.msec = lua_tonumber(lua, 1);
	data.funcName = lua_tostring(lua, 2);
	_beginthreadex(NULL, 0, gameThread, (void*)lua, 0, NULL);
}

void Macro::debug(lua_State* lua) {
	int num = lua_tonumber(lua, 1);

	printf("debug: %d \n", num);
}

void Macro::bonus_exp_1(lua_State* lua) {
	int size = 9;
	char packet[9] = { 0xe, 0x0, 0x5, 0x2f, 0xb9, 0xde, 0xb1, 0xe2, 0x0 };

	unsigned char sendpacket[100] = { "0", };
	Hooks::LoadEncrypt(packet, size, Hooks::encrypted);

	sendpacket[0] = 0xAA;
	sendpacket[1] = 0x00;
	sendpacket[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		sendpacket[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)sendpacket, size + 4, 0);
}

void Macro::bonus_exp_2(lua_State* lua) {
	int size = 18;
	char packet[18] = { 0x3a, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x5,
		0x33, 0x33, 0x32, 0x35, 0x35, 0x0 };

	for (int i = 0; i < 5; i++) {
		packet[12 + i] = Macro::bonusExpeCode[i];
	}

	unsigned char sendpacket[100] = { "0", };
	Hooks::LoadEncrypt(packet, size, Hooks::encrypted);

	sendpacket[0] = 0xAA;
	sendpacket[1] = 0x00;
	sendpacket[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		sendpacket[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)sendpacket, size + 4, 0);
}

int Macro::bonus_exp_on(lua_State* lua) {
	lua_pushnumber(lua, Macro::bonusExpeOn);
	return 1;
}

int Macro::get_item_amnt(lua_State* lua) {
	lua_pushnumber(lua, Macro::itemAmount);
	return 1;
}

void Macro::set_item_amnt(lua_State* lua) {
	Macro::itemAmount = (lua, 1);
}

void Macro::say(lua_State* lua) {
	const char* korStr = lua_tostring(lua, 1);
	unsigned char *hex = Macro::korStrToHex(korStr);
	int length = strlen(korStr);
	int i;
	int size = length + 4;
	char packet[100] = { 0xe, 0x0, 0x5, 0x2f, 0x0, };

	packet[2] = length;
	for (i = 3; i < length + 4; i++)
		packet[i] = hex[i - 3];
	unsigned char sendpacket[100] = { "0", };
	Hooks::LoadEncrypt(packet, size, Hooks::encrypted);

	sendpacket[0] = 0xAA;
	sendpacket[1] = 0x00;
	sendpacket[2] = size + 0x1;
	for (int i = 3; i < size + 4; i++)
		sendpacket[i] = Hooks::encrypted[i - 3];
	send(Hooks::Con_Packet_Socket, (const char *)sendpacket, size + 4, 0);
}