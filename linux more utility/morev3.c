#include<stdio.h>
#include<stdlib.h>

#define LINELEN 512
#define PAGELEN 20

void doMore(FILE*);
int getInput();

int main(int argc,char *argv[])
{if(argc==1){
		printf("This is the Help Page....!\n");
		exit(0);
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
	doMore(fp);
	fclose(fp);
	i++;
	}

	return 0;
}


	
 void doMore(FILE *fp){


	int totalLines=0;
	char buff[LINELEN];

	while(fgets(buff,LINELEN,fp)){

		fputs(buff,stdout);
		totalLines++;
		if(totalLines==PAGELEN){

			int inp=getInput();
			printf("\033[1A \033[2K \033[1G");
	
			if(inp==0) // q pressed ,quit
				break;
			else if(inp==1) //space pressed ,print one more page
				totalLines=totalLines-PAGELEN;
			else if(inp==2) //enter pressed ,print one line more
				totalLines--;
			else if(inp==3) //invalid character
			break;		
		}
	}
}


int getInput(){
	
	char c;
	printf("\033[7m --more-- (%%) \033[m");
	
	c=getchar();
	
	if(c=='q')
		return 0;
	if(c==' ')
		return 1;
	if(c=='\n')
		return 2;

	return 3;
}
