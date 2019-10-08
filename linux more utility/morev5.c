#include<stdio.h>
#include<stdlib.h>

#define LINELEN 512
#define PAGELEN 20

void doMore(FILE*);
int getInput(FILE*);
void getTotalLines(FILE*);

int linesDisplayed=1;
int totalFileLines=1;

int main(int argc,char *argv[])
{

	if(argc==1){
		getTotalLines(stdin);	
		doMore(stdin);
	}

	FILE *fp;
	int i=0;
	char c;
	while(i<argc-1){

		fp=fopen(argv[i+1],"r");
		if(fp==NULL){
			printf("Error Opening File\n");
			exit(1);
		} 
	
		getTotalLines(fp);	
		fclose(fp);
		fp=fopen(argv[i+1],"r");
		
		if(fp==NULL){
			printf("Error Opening File\n");
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


	int totelLines=0;
	char buff[LINELEN];
	FILE *fp_tty=fopen("/dev/tty","r");

	while(fgets(buff,LINELEN,fp)){
	
		fputs(buff,stdout);
		totelLines++;
		linesDisplayed++;
		if(totelLines==PAGELEN){

			int inp=getInput(fp_tty);
			printf("\033[1A \033[2K \033[1G");	
			if(inp==0) // q pressed ,quit
			break;
			else if(inp==1) //space pressed ,print one more page
			totelLines=totelLines-PAGELEN;
			else if(inp==2) //enter pressed ,print one line more
			totelLines--;
			else if(inp==3) //invalid character
			break;		
		}
	}
}


int getInput(FILE* cmdstm){

	char c;
	printf("\033[7m --more-- (%d %)  \033[m",(linesDisplayed*100/totalFileLines)-1);
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
	
	char buff[LINELEN];
	int count=0;

	while(fgets(buff,LINELEN,fp)){
		totalFileLines++;
	}
	
  }

