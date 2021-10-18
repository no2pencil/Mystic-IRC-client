#include <stdio.h>
#include <string.h>
#include <curses.h>

#define MAX 256

int main(void) {
	char ch='\0';
	char Buff[MAX]={0};
	char command[MAX]={0};
	char output[MAX]={0};
	int  i=0, ln=0;
	
	initscr();
	cbreak();
	noecho();

	mvaddch(0, 0, '+');
	mvaddch(LINES - 1, 0, '-');
	mvaddstr(24, 1, ":>");
	mvaddstr(25, 0, "Status [");
	refresh();

	while(TRUE) {
		//fflush(stdout);
		//scanf ("%[^\r]",Buff);
		while(i<80) {
			scanf("%c",&Buff[i]);
			sprintf(output, ":> %i [ %s ]",i, Buff);
			mvaddstr(24, 1, output);
			refresh();
			if(Buff[i]=='\n' || Buff[i]=='\r') {
				i++;
				break;
			}
			i++;
		}
		Buff[i]='\0';
		if(Buff[0]!='\n' || Buff[0]!='\r') {
			if(Buff[0]=='/') {
				sprintf(command,"%s", Buff);
				sprintf(output, "Command ... %s",Buff);
				if(strcmp(Buff,"/quit")==0) {
					endwin();
					printf("Exiting clean...\n");
					return 0;
				}
			} else {
				sprintf(output, "Received %s",Buff);
			}
		}
		mvaddstr(25, 10, output);
		sprintf(output, ":> %s", Buff);
		mvaddstr(24, 1, output);
		refresh();
		for(i=0;i<MAX;i++) {
			Buff[i]='\0';
			output[i]='\0';
		}
		ln++;
		i=0;
	}

	endwin();

	return 0;
}

