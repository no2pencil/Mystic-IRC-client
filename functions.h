/*
 * Functions Header
 */

#include <time.h>  // Required for srand

/* Global var */
char place[6][MAX]={0};

int getrand(int min,int max){
     return(rand()%(max-min)+min);
}

void printhelp(void) {
  /* Output Help File */
  FILE * helpfile = fopen("irc-howto.asc","r");
  char buffer[80],ch='\0';
  int  ln=0,i=0;

  while(ch!=EOF) {
    ch = getc(helpfile);
    //printf("%c", ch);
    if(ch=='\n') {
	    ln++;
	    i=0;
    }

    mvaddch(ln, i, ch);
    i++;
  }

  fclose(helpfile);
}

