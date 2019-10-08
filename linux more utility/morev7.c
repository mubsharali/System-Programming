#include<stdio.h>
#include<stdlib.h>
#include <sys/ioctl.h> 
#include <unistd.h>

#define LINELEN 512



int linesDisplayed=1;
int totalFileLines=1;
int PAGELEN=0;

void doMore(FILE*);
int getInput(FILE*);
void getTotalLines(FILE*);

int main(int argc,char *argv[])
{
	
	struct winsize noOfLines;
	ioctl(STDIN_FILENO, TIOCGWINSZ, &noOfLines);
	PAGELEN=noOfLines.ws_row-1;

	if(argc==1){
	   doMore(stdin);
	}
	
	FILE *fp;
	int i=0;
	char c;
	while(i<argc-1){

		fp=fopen(argv[i+1],"r");
		if(fp==NULL){
			printf("Error Opening File...!\n");
			exit(1);
		}
 
		getTotalLines(fp);
		
		fclose(fp);
		fp=fopen(argv[i+1],"r");
		
		if(fp==NULL){
			printf("Error Opening File...!\n");
			exit(1);
		} 
		
		doMore(fp);
		fclose(fp);
		i++;
		linesDisplayed=0;
		totalFileLines=0;
	}

	return 0;
}


	
 void doMore(FILE *fp ){


	int totalLines=0;
	char buff[LINELEN];
	FILE *fp_tty=fopen("/dev/tty","r");

	while(fgets(buff,LINELEN,fp)){
		
		fputs(buff,stdout);
		totalLines++;
		linesDisplayed++;

		if(totalLines==PAGELEN){
			
			int inp=getInput(fp_tty);
				
			if(inp==0){
				printf(" \033[1K \033[1G"); // q pressed ,quit
				break;
			}
			else if(inp==1){ //space pressed ,print one more page
				totalLines=totalLines-PAGELEN;
				printf(" \033[1K \033[1G");
			}
			else if(inp==2){
				printf("\033[1A \033[2K \033[1G");	 //enter pressed ,print one line more
				totalLines--;
			}
			else if(inp==3){
				printf(" \033[1K \033[1G"); //invalid character
				break;
			}		
		}
	}
}


int getInput(FILE* cmdstm){
	
	char c;	
	printf("\033[7m --more-- (%d %)  \033[m",(linesDisplayed*100/totalFileLines)-1);

	system( "stty -icanon" );
	c=getc(cmdstm);
		

	if(c=='q')
		return 0;
	if(c==' ')
		return 1;
	if(c=='\n')
		return 2;

	return 3;
}

void getTotalLines(FILE *fp){
	
	char buff1[LINELEN];
	int count=0;

	while(fgets(buff1,LINELEN,fp)){
		totalFileLines++;
	}
	
  }

