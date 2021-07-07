#include"cpylurkStructure.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<iostream>
#include<cstring>
#include<cstdint>
#include<thread>
#include<ncurses.h>
#include<vector>
using namespace std;
int max_y, max_x, log_y, log_x;
char input[1024*1024];
thread network_thread;
void* network(void*);
uint8_t type;

struct windowinfo
	{
		int skt;
		WINDOW* header;
		WINDOW* log;
		WINDOW* inputwin;
		WINDOW* players;
		WINDOW* stats;
		WINDOW* filler;
		WINDOW* enemies;
	};
	//WINDOW *border}
characterPKG me;
gamePKG game;
vector < characterPKG > Rplayer;
vector < characterPKG > monsters;
vector < connectionPKG > connections;
vector < characterPKG > lootables;
roomPKG currentRm;

int main(int argc, char ** argv)
{
	initscr();
	getmaxyx(stdscr, max_y, max_x);
	WINDOW *header    = newwin(10,max_x/2,0,0);
	WINDOW *log       = newwin(5*(max_y/6)+1, max_x/2-2, 14,1);
	WINDOW *inputwin  = newwin(3, max_x/2, 10, 0);
	WINDOW *players   = newwin(20, max_x/2+1,13,max_x/2);
	WINDOW *stats     = newwin(10, max_x/2+1, 0, max_x/2);
	//WINDOW *statb	  = newwin(10, max_x/2+1, 0, max_x/2);
	WINDOW *filler    = newwin(max_y/2+10,max_x/4,33,(max_x/4)*2);
	WINDOW *enemies   = newwin(max_y/2+10,max_x/4,33,((max_x/4)*3)+1);
	WINDOW *border    = newwin(5*(max_y/6)+3, max_x/2, 13, 0);
	//WINDOW *something = newwin(10,max_x/2+1,0,max_x/2);
	struct windowinfo winfo;
	winfo.header = header;
	winfo.log = log;
	winfo.inputwin = inputwin;
	winfo.players = players;
	winfo.stats = stats;
	winfo.filler = filler;
	winfo.enemies = enemies;
	//winfo.border = border;

	wborder(header,'|','|','-','-','+','+','+','+');
	//wborder(statb,'|','|','-','-','+','+','+','+');
	wborder(inputwin,'|','|','-','-','+','+','+','+');
	wborder(players,'|','|','-','-','+','+','+','+');
	wborder(stats,'|','|','-','-','+','+','+','+');
	wborder(filler,'|','|','-','-','+','+','+','+');
	wborder(border,'|','|','-','-','+','+','+','+');
	wborder(enemies,'|','|','-','-','+','+','+','+');
	getmaxyx(log, log_y, log_x);
	scrollok(log,true);
	char inputs[] = "Enter commands  here";
	char str[10];
	mvwprintw(header,11,10, "This is a test");
	mvwprintw(header,1,1,  "1: message");
	mvwprintw(header,2,1,  "2: change room");
	mvwprintw(header,3,1,  "3: fight");
	mvwprintw(header,4,1,  "4: pvp");
	mvwprintw(header,5,1,  "5: loot");
	mvwprintw(header,6,1,  "6: start");
	mvwprintw(header,7,1,  "7: error");
	mvwprintw(header,1,20, "8: accept error");
	mvwprintw(header,2,20, "9: room");
	mvwprintw(header,3,20, "10: make character");
	mvwprintw(header,4,20, "11: game");
	mvwprintw(header,5,20, "12: quit");
	mvwprintw(header,6,20, "13: room connections");
	mvwprintw(header,8,1, "List of all available commands");
	mvwprintw(stats,1,1,"name: ");
	mvwprintw(stats,2,1,"atk: ");
	mvwprintw(stats,3,1,"def: ");
	mvwprintw(stats,4,1,"reg: ");
	mvwprintw(stats,5,1,"hp : ");
	mvwprintw(stats,6,1,"gold: ");
	//mvwprintw(enemies,1,1,"placeholder for enemies");
	//mvwprintw(filler,1,1,"placeholder for player list");
	//mvwprintw(players,1,1,"misnamed placeholder for rooms");
	//mvwprintw(something,1,1,"I'll put something here....maybe");
	wrefresh(header);
	//mvwprintw(log, 3, 1, "Poof\n");
	wrefresh(log);
	wrefresh(players);
	wrefresh(stats);
	wrefresh(filler);
	wrefresh(border);
	wrefresh(inputwin);
	wrefresh(enemies);
	//wrefresh(statb);

	if(argc < 3)
	{
		printf("Usage:  %s hostname port\n", argv[0]);
		return 1;
	}
	int skt = socket(AF_INET, SOCK_STREAM, 0);
	if(skt == -1)
	{
		perror("Socket:  ");
		return 1;
	}

	
	//these lines connect to server
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(atoi(argv[2]));

	struct hostent* entry = gethostbyname(argv[1]);
	struct in_addr **addr_list = (struct in_addr**)entry->h_addr_list;
	struct in_addr* c_addr = addr_list[0];
	char* ip_string = inet_ntoa(*c_addr);
	saddr.sin_addr = *c_addr;

	cout << connect(skt, (struct sockaddr*)&saddr, sizeof(saddr));

	thread  network_thread(network, &winfo);
	Rplayer = vector<characterPKG> ();
	monsters = vector<characterPKG> ();
	connections = vector<connectionPKG> ();
	lootables  = vector<characterPKG> ();
	while (true){
		winfo.skt = skt;

		wborder(inputwin,'|','|','-','-','+','+','+','+');
		wmove(inputwin,1,1);
		wgetstr(inputwin,input);
		wscrl(log, -1*(1+(strlen(input)/max_x)));
		if (!strcmp(input,"1"))
		{
			type = 1;
			mvwprintw(log,0,1,"number is 1");
			msgPKG pkg;
			sendMSG(pkg,3);
			pushMessage(pkg, log);
		}
		else if(!strcmp(input,"2"))
		{
			type = 2;
			//mvwprintw(log,0,1,"number is 2");
			changeroomPKG pkg;
			pkg.newRoom = 1;
			chngRoom(pkg,3);
			//pushRoom(pkg,players);
		}
		else if(!strcmp(input,"3"))
		{
			type = 3;
			 //mvwprintw(log,0,1,"number is 3");
			 fightPKG pkg;
			 fight(pkg,3);
			 pushFight(pkg,log);
		}
		else if(!strcmp(input,"4"))
                {
			type = 4;
			 //mvwprintw(log,0,1,"number is 4");
			 pvpPKG pkg;
			 PvP(pkg,3);
			 pushPVP(pkg, log);
                }
		else if(!strcmp(input,"5"))
                {
			type = 5;
			 //mvwprintw(log,0,1,"number is 5");
			 lootPKG pkg;
			 Loot(pkg,3);
			 pushLoot(pkg, log);
                }
		else if(!strcmp(input,"6"))
                {
			type = 6;
			 //mvwprintw(log,0,1,"number is 6");
			startPKG pkg;
			Start(pkg,3);
			pushStart(pkg,log);
                }
		else if(!strcmp(input,"7"))
                {
			type = 7;
			 //mvwprintw(log,0,1,"number is 7");
                }
		else if(!strcmp(input,"8"))
                {
			type = 8;
			 //mvwprintw(log,0,1,"number is 8");
			 acceptPKG pkg;
			 pushAccept(pkg,log);
                }
		else if(!strcmp(input,"9"))
                {
			 type = 9;
			 //mvwprintw(log,0,1,"number is 9");
			 roomPKG pkg;
			 pkg.roomNum = 9;
			 pkg.roomName = "test room";
			 pkg.roomDescription = "a test room";
			 pushRoom(pkg,players);
                }
		else if(!strcmp(input,"10"))
                {
			 type = 10;
			 characterPKG pkg;
			 mvwprintw(log,0,1,"%d",type);
			 pkg.name = "test";
			 pkg.flags = 1;
			 pkg.atk  = 50;
			 pkg.def  = 49;
			 pkg.regen = 1;
			 pkg.health = 100;
			 pkg.gold   = 0;
			 pkg.room  = 0;
			 pkg.characterDescription = "explorer";
			 Pcharacter(pkg,skt);
			 me = pkg;
			 pushCharacter(pkg,stats,filler,enemies);




                }
		else if(!strcmp(input,"11"))
                {
			 //mvwprintw(log,0,1,"number is 11");

                }
		else if(!strcmp(input,"12"))
                {
			 type = 12;
			 //mvwprintw(log,0,1,"number is 12");
			 leavePKG pkg;
			 exit(pkg,3);
			 pushLeave(pkg, log);
                }
		else if(!strcmp(input,"13"))
                {
			type = 13;
			 //mvwprintw(log,0,1,"number is 13");
			 connectionPKG pkg;
			 pushConnection(pkg,log);
                }

		wrefresh(log);
		wrefresh(border);
		wclear(inputwin); //clears window
	}
	// end of connection code

	// Interaction with the server starts here
	close(skt);
	endwin();
}

void* network(void *arg){			//function for network
	struct windowinfo* outputs = (struct windowinfo*)arg;
 	int skt = outputs->skt;
	while (true){          			//loop for sending and receiving packet
		 //uint8_t type;		//type declared as unsigned 1 byte int
		 read(skt, &type,1);
//		 mvwprintw(outputs->log,0,1, "%d",type);
//		 mvwprintw(outputs->log,0,4,"network text test");
		 wrefresh(outputs->log);	//read from socket type
		 if (type == 1){	//message
		   mvwprintw(outputs->log,2,5,"message");
		   mvwprintw(outputs->log,10,1, "%d",type);
		   wscrl(outputs->log, -1);
		   wrefresh(outputs->log);
		   msgPKG pkg;
		   uint16_t msgLen;
		   read(skt, &msgLen, 2);
	 	   char mbuffer[msgLen+1];
		   char buffer[33];
		   read(skt, buffer, 32);
		   buffer[32] = 0;
		   pkg.receiver = (buffer);
		   read(skt, buffer, 32);
		   pkg.sender = (buffer);
		   read(skt, mbuffer, msgLen);
		   mbuffer[msgLen] = 0;
		   pkg.message = (mbuffer);
		   pushMessage(pkg,outputs->log);

	 	 }
		 else if (type == 2){	//change room ignore
		  mvwprintw(outputs->log,2,5,"changing to room");
		  mvwprintw(outputs->log,10,1, "%d",type);
		  wscrl(outputs->log,-1);
		  wrefresh(outputs->log);
		  changeroomPKG pkg;
		  read(skt,&(pkg.newRoom),2);
		  pushChangeRoom(pkg,outputs->log);
		 }
		 else if (type == 3){	//fight ignore
		  mvwprintw(outputs->log,2,5,"fighting monster");
		  mvwprintw(outputs->log,10,1,"%d",type);
		  wscrl(outputs->log, -1);
		  wrefresh(outputs->log);
		  fightPKG pkg;
		  pushFight(pkg,outputs->log);
		 }
		 else if (type == 4){	//pvp fight ignore
		  mvwprintw(outputs->log,2,5,"fighting player");
		  mvwprintw(outputs->log,10,1,"%d",type);
		  wscrl(outputs->log, -1);
		  wrefresh(outputs->log);
		  pvpPKG pkg;
		  char buffer[33];
		  read(skt, buffer, 32);
		  buffer[32] = 0;
		  pkg.targetPlayer = (buffer);
		  pushPVP(pkg,outputs->log);
		 }
		 else if (type == 5){	//loot  ignore
		  mvwprintw(outputs->log,2,5,"looting");
		  mvwprintw(outputs->log,10,1,"%d",type);
		  wscrl(outputs->log, -1);
		  wrefresh(outputs->log);
		  lootPKG pkg;
		  char buffer[33];
		  read(skt, buffer, 32);
		  buffer[32] = 0;
		  pkg.targetPlayer = (buffer);
		  pushLoot(pkg,outputs->log);
		 }
		 else if (type == 6){	//start game ignore
		  mvwprintw(outputs->log,2,5, "starting");
		  mvwprintw(outputs->log,10,1,"%d",type);
		  wscrl(outputs->log, -1);
		  wrefresh(outputs->log);
		  startPKG pkg;
		  pushStart(pkg,outputs->log);
		 }
		 else if (type == 7){	//error
		   mvwprintw(outputs->log,2,5, "error");
		   mvwprintw(outputs->log,10,1, "%d",type);
		   wscrl(outputs->log,-1);
		   wrefresh(outputs->log);
		   errorPKG pkg;
		   read(skt,&(pkg.errorCode),1); 
		   uint16_t eLen;
		   read(skt, &eLen,2);
		   char ebuffer[eLen+1];
		   read(skt, ebuffer, eLen);
		   ebuffer[eLen] = 0;
		   pkg.errorMsg = (ebuffer);
		   pushError(pkg,outputs->log);

		 }
		 else if (type == 8){	//accept
		   mvwprintw(outputs->log,2,5, "accept");
		   mvwprintw(outputs->log,10,1, "%d",type);
		   wscrl(outputs->log, -1);
		   wrefresh(outputs->log);
		   acceptPKG pkg;
		   read(skt,&(pkg.acception),1);
		   pushAccept(pkg,outputs->log);

		 }
		 else if (type == 9){	//room
		   mvwprintw(outputs->players,2,5, "room");
		   mvwprintw(outputs->players,10,1,"%d",type);
		   wscrl(outputs->players, -1);
		   wrefresh(outputs->players);
		   roomPKG pkg;
		   read(skt,&(pkg.roomNum),2);
		   char rbuffer[33];
		   read(skt, rbuffer, 32);
		   rbuffer[32] = 0;
		   pkg.roomName = (rbuffer);
		   uint16_t rmLen;
		   read(skt,&rmLen,2);
		   char rmbuffer[rmLen+1];
		   read(skt, rmbuffer, rmLen);
		   rmbuffer[rmLen] = 0;
		   pkg.roomDescription = (rmbuffer);
		   pushRoom(pkg,outputs->players);

		 }
		 else if (type == 10){	//character
		   mvwprintw(outputs->log,2,5, "character");
		   mvwprintw(outputs->log,10,1,"%d",type);
		   wscrl(outputs->log, -1);
		   wrefresh(outputs->log);
		   characterPKG pkg;
		   char namebuffer[33];
		   read(skt, namebuffer,32);
		   namebuffer[32] = 0;
		   pkg.name = (namebuffer);
		   read(skt,&(pkg.flags),1);
		   read(skt,&(pkg.atk),2);
		   read(skt,&(pkg.def),2);
		   read(skt,&(pkg.regen),2);
		   read(skt,&(pkg.health),2);
		   read(skt,&(pkg.gold),2);
		   read(skt,&(pkg.room),2);
		   uint16_t pdLen;
		   read(skt, &pdLen,2);
		   char cbuffer[pdLen+1];
		   read(skt, cbuffer, pdLen);
		   cbuffer[pdLen] = 0;
		   pkg.characterDescription = (cbuffer);
		   pushCharacter(pkg,outputs->stats,outputs->filler,outputs->enemies);

		 }
		 else if (type == 11){	//game
		   mvwprintw(outputs->log,0,1, "game");
		   mvwprintw(outputs->log,10,1,"%d",type);
		   wscrl(outputs->log, -1);
		   wrefresh(outputs->log);
		   gamePKG pkg;
		   read(skt,&(pkg.points),2);
		   read(skt,&(pkg.stats),2);
		   uint16_t dlen;
		   read(skt, &dlen,2);
		   char buffer[dlen+1];
		   read(skt, buffer, dlen);
		   buffer[dlen] = 0;
		   pkg.description = (buffer);
		   pushGame(pkg,outputs->log);

		 }
		 else if (type == 12){	//leave ignore
		   mvwprintw(outputs->log,1,1,"%d",type);
		   wscrl(outputs->log, -1);
		   wrefresh(outputs->log);
		   leavePKG pkg;
		   pushLeave(pkg,outputs->log);

		 }
		 else if (type == 13){	//room connection
		   mvwprintw(outputs->players,2,5,"connections");
		   mvwprintw(outputs->players,10,1,"%d",type);
		   wscrl(outputs->players,-1);
		   wrefresh(outputs->players);
		   connectionPKG pkg;
		   read(skt,&(pkg.roomNum),2);
		   char crbuffer[33];
		   read(skt, crbuffer,32);
		   crbuffer[32] = 0;
		   pkg.roomName = (crbuffer);
		   uint16_t rdLen;
		   read(skt, &rdLen, 2);
		   char rmbuffer[rdLen+1];
		   read(skt, rmbuffer, rdLen);
		   rmbuffer[rdLen] = 0;
		   pkg.roomDescription = (rmbuffer);
		   pushConnection(pkg,outputs->players);

		 }
	}
}
//writes to the server for each packet
void sendMSG(msgPKG pkg, int skt ){
	uint8_t type = 1;
	write(skt, &type,1);
	uint16_t msgLen = (uint16_t) pkg.message.length();
	write(skt, &msgLen,2);
	char msg[33];
	strncpy(msg, pkg.receiver.c_str(), sizeof(msg));
	msg[sizeof(msg) - 1] = 0;
	write(skt, msg,32);
	strncpy(msg, pkg.sender.c_str(), sizeof(msg));
	msg[sizeof(msg) - 1] = 0;
	write(skt, msg,32);
	char mbuffer[msgLen];
	strncpy(mbuffer, pkg.message.c_str(), sizeof(mbuffer));
	write(skt, mbuffer,msgLen);
	
}

void chngRoom(changeroomPKG pkg, int skt){
	uint8_t type = 2;
	write(skt, &type,1);
	write(skt, &(pkg.newRoom),2);
}

void fight(fightPKG pkg, int skt){
	uint8_t type = 3;
	write(skt, &type,1);
}

void PvP(pvpPKG pkg, int skt){
	uint8_t type = 4;
	write(skt, &type,1);
	uint16_t charLen = (uint16_t) pkg.targetPlayer.length();
	char player[33];
	strncpy(player, pkg.targetPlayer.c_str(), sizeof(player));
	player[sizeof(player) - 1] = 0;
	write(skt, player,32);
}

void Loot(lootPKG pkg, int skt){
	uint8_t type = 5;
	write(skt, &type,1);
	uint16_t pLen = (uint16_t) pkg.targetPlayer.length();
	char target[33];
	strncpy(target, pkg.targetPlayer.c_str(), sizeof(target));
	target[sizeof(target) -1 ] = 0;
	write(skt, target,32);
}

void Start(startPKG pkg, int skt){
	uint8_t type = 6;
	write(skt, &type,1);
}

void Accept(acceptPKG pkg, int skt){
	uint8_t type = 8;
	write(skt, &type,1);
	write(skt, &(pkg.acception),1);
}
void Pcharacter(characterPKG pkg, int skt){
	uint8_t type = 10;
	write(skt, &type,1);
	uint16_t cLen = (uint16_t) pkg.characterDescription.length();
	char cname[33];
	strncpy(cname, pkg.name.c_str(), sizeof(cname));
	cname[sizeof(cname) - 1] = 0;
	write(skt, cname,32);
	write(skt, &(pkg.flags),1);
	write(skt, &(pkg.atk),2);
	write(skt, &(pkg.def),2);
	write(skt, &(pkg.regen),2);
	write(skt, &(pkg.health),2);
	write(skt, &(pkg.gold),2);
	write(skt, &(pkg.room),2);
	write(skt, &cLen,2);
	char cbuffer[cLen];
	memcpy(cbuffer, pkg.characterDescription.c_str(), cLen);
	write(skt, cbuffer, cLen);
}
void exit(leavePKG pkg, int skt){
	uint8_t type = 12;
	write(skt, &type,1);
	//close(skt);
}
