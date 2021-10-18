#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <curses.h>
/* Load Networking Headers */
#include <unistd.h> // for sleep
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h> // for UNIX
#include <netdb.h>

#include "vars.h"
#include "functions.h"

int main(int argc, char *argv[]) {
	int i=0, ln=1, a=0, n=0, s, ss, cont=0,pars_fl=0, inc=0, png=0, opt=0;
	struct sockaddr_in addr;
	struct hostent *he;
	struct in_addr **addr_list;
	char ch='\0',* pvm; 
	char SendBuf[MAX]={0}, RecvBuf[MAX]={0},Buff[8][40]={0},TempBuff[40]={0};

	char DisplayBuff[MAX]={0};
	char command[MAX]={0};
	char output[MAX]={0};
	// Commands
	char *join="/join", *ircConnect="/connect", *help="/help", *motd="/motd", *quit="/quit";

	/* Before anything else, start a timer for random */
	srand(time(NULL));
	
	initscr();
	cbreak();
	noecho();
	raw();

	int sizeY, sizeX;
	getmaxyx(stdscr, sizeY, sizeX);

	WINDOW *chatWindow = newwin(23, 79, 0, 0);

	box(chatWindow, 0, 0);
	refresh();
	wrefresh(chatWindow);
	keypad(chatWindow, true);

	mvaddch(LINES - 1, 0, '-');
	mvaddstr(23, 1, ":>");
	mvaddstr(24, 0, "Status [");
	refresh();



	if ((s=socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		cont=1;
		exit(1);
	}
	
	/* DNS HOST ADDR */
	if((he=gethostbyname(SERVERHOST))==NULL) {
		perror("NS Translate");
		cont=2;
		exit(1);
	}
	
	addr_list=(struct in_addr **)he->h_addr_list;
	for(i=0; addr_list[i]!=NULL; i++) {
		//printf("%s ", inet_ntoa(*addr_list[i]));
	}
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(inet_ntoa(*addr_list[0]));
	addr.sin_port=htons(PORT);

	ss=connect(s, (struct sockaddr *)&addr, sizeof(addr));
	if(ss<0) {
		perror("connect");
		cont=3;
		exit(1);
	}

	sprintf(output, "Connected to %s", SERVERHOST);
	mvaddstr(ln, 1, output);
	ln++;
	refresh();

	sleep(2);

	while(TRUE) {

	n=0;
	while(n<MAX) { // Clear socket buffers
		RecvBuf[n]='\0';
		SendBuf[n]='\0';
		n++;
	}
	       
	i=recv(s, RecvBuf, MAX, 0);
	sprintf(output, "%d bytes received",i);
	mvaddstr(ln, 1, output);
	ln++;
	refresh();
	// Check for Ping-Pong from the server
	
	pvm=strtok(RecvBuf, " :!~\n");
	n=0;
	while(pvm!=NULL) {
		sprintf(output, "%s",pvm);
		mvaddstr(ln, 1, output);
		ln++;
		refresh();
		sprintf(Buff[n],"%s",pvm);
		pvm=strtok (NULL, " :!~\n");
		n++;
	}

	sleep(2);


		if(strcmp("PING", Buff[png])==0) {
			sprintf(output, "PING Detected... Sending PONG! %s",Buff[1]);
			mvaddstr(ln, 1, output);
			ln++;
			sprintf(SendBuf,"PONG %s%s\r\n", Buff[1],Buff[7]);
			if(send(s,SendBuf,strlen(SendBuf),0)<0) {
				perror("PING-PONG!");
			}
			cont=0;
		}

		if(cont==0) {
			sprintf(SendBuf, "NICK %s\r\nUSER %s %s %s : TestBot \rnJOIN %s\r\n", NICK, USER, IDENT< IDENT, CHAN);
			if(send(s,SendBuf,strlen(SendBuf),0)<0) {
				perror("Transmit");
				cont=10;
			}
			sprintf(SendBuf, "JOIN %s\r\n", CHAN);
			if(send(s,SendBuf,strlen(SendBuf),0)<0) {
				perror("Join");
			}
			while(n<MAX) {
				SendBuf[n]='\0';
				n++;
			}
			sleep(3);
		} else {
			sprintf(output, "Attempting to connect");
			mvaddstr(ln, 1, output);
			ln++;
			refresh();
		}


		i=0;

		while(i<80) {
			scanf("%c",&DisplayBuff[i]);
			sprintf(output, ":> %i [ %s ]",i, DisplayBuff);
			mvaddstr(23, 1, output);
			refresh();
			if(DisplayBuff[i]=='\n' || DisplayBuff[i]=='\r') {
				i++;
				break;
			}
			ln=i;
			i++;
		}
		DisplayBuff[i]='\0';
		output[i]='\0';
		if(DisplayBuff[0]!='\n' || DisplayBuff[0]!='\r') {
			if(DisplayBuff[0]=='/') {
				sprintf(command,"%s", DisplayBuff);
				sprintf(output, "Command ... %s",DisplayBuff);
				if(strstr(command,ircConnect)>0) {
						sprintf(output, "Connecting to Host [ %s ]", SERVERHOST);
						mvaddstr(24, 0, output);
						refresh();
						
						if(cont==0) {
							sprintf(output, "Connected to [ %s ]",SERVERHOST);
						} else {
							sprintf(output, "Unable to connect [ %i ] to [ %s ]",cont, SERVERHOST);
							/* Need to collect return code & display a more informative error */
						}
						mvaddstr(24, 0, output);
						refresh();
				}
				if(strstr(command,join)>0) {
					sprintf(SendBuf, "JOIN %s\r\n", CHAN);
					if(send(s,SendBuf,strlen(SendBuf),0)<0) {
						perror("Join");
					}
					while(n<MAX) {
						SendBuf[n]='\0';
						n++;
					}
					// Avoid Flooding the server with joins
					sleep(3);
					mvaddstr(24, 0 , output);
				}
				if(strstr(command,help)>0) {
					printhelp();
					wrefresh(chatWindow);
				}
				if(strstr(command,quit)>0) {
					sprintf(SendBuf,"Returning to the BBS! Tank, I need an exit!");
					if(send(s,SendBuf,strlen(SendBuf),0)<0) {
						perror("Quit"); // Does it matter?
					}
					endwin();
					printf("Returning to the BBS...\n");
					return 0;
				}
			} else {
				//sprintf(output, "Received %s",DisplayBuff);
				sprintf(SendBuf,"PRIVMSG %s %s\r\n", CHAN, DisplayBuff);
				if(send(s,SendBuf,strlen(SendBuf),0)<0) {
					perror("Error with message");
				}
			}
		}
		mvaddstr(24, 10, output);
		//sprintf(output, ":> %s", DisplayBuff);
		sprintf(output, ":>[                                                            ]");
		mvaddstr(23, 1, output);
		refresh();
		for(i=0;i<ln;i++) {
			DisplayBuff[i]='\0';
			output[i]='\0';
		}
		refresh();
		sprintf(output, "Status [                                                            ]");
		mvaddstr(24, 0, output);
		i=0;
		wrefresh(chatWindow);
	}

	endwin();

	return 0;
}

