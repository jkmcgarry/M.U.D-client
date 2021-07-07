#include "cpylurkStructure.h"
#include <iostream>
#include<ncurses.h>
#include<string.h>
#include<vector>
int max_height, max_width, log_height, log_width;
using namespace std;
extern characterPKG me;
extern gamePKG game;
extern vector < characterPKG > Rplayer;
extern vector < characterPKG > monsters;
extern vector < connectionPKG > connections;
extern vector < characterPKG > lootables;
extern roomPKG currentRm;

void pushMessage(msgPKG &package, WINDOW* log )
{
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(log,0,1, "%s",package.sender.c_str());
	wscrl(log,-1*(1+(package.sender.size()/max_width)));
	mvwprintw(log,0,1, "%s",package.receiver.c_str());
	wscrl(log,-1*(1+(package.receiver.size()/max_width)));
	mvwprintw(log,0,1, "%s",package.message.c_str());
	wscrl(log,-1*(1+(package.message.size()/max_width)));
	wrefresh(log);
	//sendMSG(package, 3);
}
void pushChangeRoom(changeroomPKG &package, WINDOW* log){
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(log,0,1, "changing to room");
	wscrl(log,-1);
	mvwprintw(log,0,1, "%d", package.newRoom);
	wscrl(log,-1);
	wrefresh(log);
	//chngRoom(package, 3);
}
void pushFight(fightPKG &package, WINDOW* log){
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(log,0,1, "fighting...");
	wscrl(log,-1);
	wrefresh(log);
	//fight(package, 3);
}
void pushPVP(pvpPKG &package, WINDOW* log){
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(log,0,1, "fighting player...");
	wscrl(log,-1);
	mvwprintw(log,0,1, "%s", package.targetPlayer.c_str());
	wscrl(log,-1);
	wrefresh(log);
	//PvP(package, 3);
}
void pushLoot(lootPKG &package, WINDOW* log){
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(log,0,1, "looting...");
	wscrl(log,-1);
	mvwprintw(log,0,1, "%s",package.targetPlayer.c_str());
	wscrl(log, -1);
	wrefresh(log);
	//Loot(package, 3);
}
void pushStart(startPKG &package, WINDOW* log){
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(log,0,1, "starting....");
	wscrl(log,-1);
	wrefresh(log);
	//Start(package,3);
}
void pushError(errorPKG &package, WINDOW* log){
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(log,0,1, "%s",package.errorMsg.c_str());
	wscrl(log,-1*(1+(package.errorMsg.size()/max_width)));
	wrefresh(log);
}
void pushAccept(acceptPKG &package, WINDOW* log){
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(log,0,1, "%d",package.acception);
	wscrl(log,-1);
	wrefresh(log);
	Accept(package,3);
}
void pushRoom(roomPKG &package, WINDOW* players){
	Rplayer = vector<characterPKG> ();
        monsters = vector<characterPKG> ();
        connections = vector<connectionPKG> ();
        lootables  = vector<characterPKG> ();
	currentRm = package;
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(players,1,1, "%d",package.roomNum);
	mvwprintw(players,2,1, "%s",package.roomName.c_str());
	mvwprintw(players,3,1, "%s",package.roomDescription.c_str());
	wscrl(players,1*(1+(package.roomDescription.size()/max_width)));
	wrefresh(players);
}
void pushCharacter(characterPKG &package, WINDOW* stats, WINDOW* filler, WINDOW* enemies){
	if (me.name == package.name && !(package.flags&(1<<5)))
	{
		getmaxyx(stdscr, max_height, max_width);
		wclear(stats);
		mvwprintw(stats,1,10, "%s",package.name.c_str());
		mvwprintw(stats,2,10, "%d",package.atk);
		mvwprintw(stats,3,10, "%d",package.def);
		mvwprintw(stats,4,10, "%d",package.regen);
		mvwprintw(stats,5,10, "%d",package.health);
		mvwprintw(stats,6,10, "%d",package.gold);
		mvwprintw(stats,1,30, "%s",package.characterDescription.c_str());
		wscrl(stats,1*(1+(package.characterDescription.size()/max_width)));
		wrefresh(stats);
		//wclear(stats);
	}
	else if (!(package.flags&(1<<7)))
	{
		getmaxyx(stdscr, max_height, max_width);
		lootables.push_back(package);
	}
	else if (!(package.flags&(1<<5)))
	{
		bool exists = false;
		for (int i = 0; i < Rplayer.size(); i++)
		{
			if (Rplayer[i].name == package.name)
			{
				exists = true;
				Rplayer[i] = package;
			}
		}
		if (exists == false)
		{
			//Rplayer.push_back(package);

			getmaxyx(stdscr, max_height, max_width);
			wclear(filler);
			mvwprintw(filler,1,0, "%s",package.name.c_str());
			mvwprintw(filler,2,0, "%d",package.atk);
			mvwprintw(filler,3,0, "%d",package.def);
			mvwprintw(filler,4,0, "%d",package.regen);
			mvwprintw(filler,5,0, "%d",package.health);
			mvwprintw(filler,6,0, "%d",package.gold);
			mvwprintw(filler,7,0, "%s",package.characterDescription.c_str());
			wscrl(filler, 1*(1+(package.characterDescription.size()/max_width)));
			wrefresh(filler);
			Rplayer.push_back(package);
		}
	}
	else if (package.flags&(1<<5))
	{
		bool exists = false;
		for (int i = 0; i < monsters.size();i++)
		{
			if (monsters[i].name == package.name)
			{
				exists = true;
				monsters[i] = package;
			}
		}
		if (exists == false)
		{
			//monsters.push_back(package);
			getmaxyx(stdscr, max_height, max_width);
			wclear(enemies);
			mvwprintw(enemies,1,0, "%s",package.name.c_str());
			mvwprintw(enemies,2,0, "%d",package.atk);
			mvwprintw(enemies,3,0, "%d",package.def);
			mvwprintw(enemies,4,0, "%d",package.regen);
			mvwprintw(enemies,5,0, "%d",package.health);
			mvwprintw(enemies,6,0, "%d",package.gold);
			mvwprintw(enemies,7,0, "%s",package.characterDescription.c_str());
			wscrl(enemies, 1*(1+(package.characterDescription.size()/max_width)));
			wrefresh(enemies);
			monsters.push_back(package);
		}
	}
	//Pcharacter(package,3);
}
void pushGame(gamePKG &package, WINDOW* log){
	game = package;
	getmaxyx(stdscr, max_height,  max_width);
	mvwprintw(log,0,1, "%s",package.description.c_str());
	wscrl(log, -1*(1+(package.description.size()/max_width)));
	wrefresh(log);
}
void pushLeave(leavePKG &package, WINDOW* log){
	mvwprintw(log,0,1, "did it work?");
	getmaxyx(stdscr, max_height, max_width);
	wrefresh(log);
	//exit(package,3);
}
void pushConnection(connectionPKG &package, WINDOW* players){
	connections.push_back(package);
	
	getmaxyx(stdscr, max_height, max_width);
	mvwprintw(players,0,1, "%d", package.roomNum);
	wscrl(players,-1);
	mvwprintw(players,0,1, "%s",package.roomName.c_str());
	wscrl(players,-1);
	mvwprintw(players,0,1, "%s",package.roomDescription.c_str());
	wscrl(players,- 1*(1+(package.roomDescription.size()/max_width)));
	wrefresh(players);

}
