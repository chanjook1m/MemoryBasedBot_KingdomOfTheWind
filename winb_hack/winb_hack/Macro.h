#pragma once
#include <string>
#include <thread>
#include "lua.hpp"

class Macro
{
	static struct Point {
		int x, y;
	};
public:
	Macro();
	~Macro();

	static void keypress(lua_State* lua);
	static void keydown(lua_State* lua);
	static void keyup(lua_State* lua);
	static int pc_x(lua_State* lua);
	static int pc_y(lua_State* lua);
	static int pc_hp(lua_State* lua);
	static int pc_mp(lua_State* lua);
	static int pc_exp(lua_State* lua);
	static int sell_exp(lua_State* lua);
	static int item_xy(lua_State* lua);
	static void attack(lua_State* lua); // 공격마법 4번
	static unsigned char * korStrToHex(const char *korStr);
	static void add_command(lua_State* lua);
	static int mapname(lua_State* lua);
	static void cardinal_direction(lua_State* lua); // 비영사천문 5번
	static void refresh(lua_State* lua);
	static void hp_charge(lua_State* lua); // 회복마법 3번
	static void mp_charge(lua_State* lua); // 공력증강 2번
	static void returnToInn(); // 귀환 6 번
	static void sleep(lua_State* lua);
	static void showtext(lua_State *lua);
	static void setInterval(lua_State* lua);
	static void clearInterval(lua_State* lua);
	static void eat();
	static void consoleshowtext(const char *korStr);
	static void move_char(int direction);
	static void bonus_exp_1(lua_State* lua);
	static void bonus_exp_2(lua_State* lua);
	static int bonus_exp_on(lua_State* lua);
	static int get_item_amnt(lua_State* lua);
	static void set_item_amnt(lua_State* lua);
	static void debug(lua_State* lua);
	static void say(lua_State* lua);
	
	

	static void pri(lua_State* lua);

	static HWND macroHWND;
	static unsigned int hp, mp;
	static unsigned int expe;
	static unsigned int bonusExpeCode[5];
	static unsigned int bonusExpeOn;
	static unsigned short id;
	static unsigned short mapId;
	static unsigned short playerX, playerY;
	static unsigned short prevplayerX, prevplayerY;
	static unsigned short mId;
	static unsigned short monsterX, monsterY;
	static unsigned char map[100];
	static unsigned char chatMsg[100];
	static Point itemXY[100];
	static lua_State* L;
	static int itemCnt;
	static unsigned int samePositionCnt;
	static unsigned int itemAmount;
};

