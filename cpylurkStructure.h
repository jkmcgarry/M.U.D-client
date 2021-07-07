#ifndef  lurkStructure
#define  lurkStructure
#include <iostream>
#include <string>
#include<ncurses.h>
using namespace std;

struct msgPKG{
	string sender;
	string receiver;
	string message;
};
struct changeroomPKG{
	uint16_t newRoom;

};
struct fightPKG{

};
struct pvpPKG{
	string targetPlayer;

};
struct lootPKG{
	string targetPlayer;

};
struct startPKG{

};
struct errorPKG{
	uint8_t errorCode;
	string errorMsg;
};
struct acceptPKG{
	uint8_t acception;
};
struct roomPKG{
	uint16_t roomNum;
	string roomName;
	string roomDescription;

};
struct characterPKG{
	string name;
	uint8_t flags;
	uint16_t atk;
	uint16_t def;
	uint16_t regen;
	uint16_t health;
	uint16_t gold;
	uint16_t room;
	string characterDescription;
};
struct gamePKG{
	uint16_t points;
	uint16_t stats;
	string description;
};
struct leavePKG{
};
struct connectionPKG{
	uint16_t roomNum;
	string roomName;
	string roomDescription;
};
void pushMessage(msgPKG &pkg,WINDOW*);		//1
void pushChangeRoom(changeroomPKG &pkg, WINDOW*); //2
void pushFight(fightPKG &pkg,WINDOW*);		//3
void pushPVP(pvpPKG &pkg,WINDOW*);		//4
void pushLoot(lootPKG &pkg,WINDOW*);		//5
void pushStart(startPKG &pkg,WINDOW*);		//6
void pushError(errorPKG &pkg,WINDOW*);		//7
void pushAccept(acceptPKG &pkg,WINDOW*);		//8
void pushRoom(roomPKG &pkg,WINDOW*);		//9
void pushCharacter(characterPKG &pkg,WINDOW*,WINDOW*,WINDOW*);	//10
void pushGame(gamePKG &pkg,WINDOW*);		//11
void pushLeave(leavePKG &pkg,WINDOW*);		//12
void pushConnection(connectionPKG &pkg,WINDOW*); //13

void sendMSG(msgPKG pkg, int skt);
void chngRoom(changeroomPKG pkg, int skt);
void fight(fightPKG pkg, int skt);
void PvP(pvpPKG pkg, int skt);
void Loot(lootPKG pkg, int skt);
void Start(startPKG pkg, int skt);
void Accept(acceptPKG pkg, int skt);
void Pcharacter(characterPKG pkg, int skt);
void exit(leavePKG pkg, int skt);
#endif
