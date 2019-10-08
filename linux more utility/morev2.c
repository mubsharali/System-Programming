#include<stdio.h>
#include<stdlib.h>

#define LINELEN 512
#define PAGELEN 20

void doMore(FILE*);
int getInput();


int main(int argc,char *argv[])
{
	if(argc==1){
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


	
void doMore(FILE *fp ){

	int ttl_lines=0;
	char buff[LINELEN];
	while(fgets(buff,LINELEN,fp)){
		fputs(buff,stdout);
		ttl_lines++;
		if(ttl_lines==PAGELEN){
			int inp=getInput();
			if(inp==0) // q pressed ,quit
			break;
			else if(inp==1) //space pressed ,print one more page
			ttl_lines=ttl_lines-PAGELEN;
			else if(inp==2) //enter pressed ,print one line more
			ttl_lines--;
			else if(inp==3) //invalid character
			break;		
		}
	}
}


int getInput(){
	char c;
	c=getchar();
	if(c=='q')
		return 0;
	if(c==' ')
		return 1;
	if(c=='\n')
		return 2;
	
	return 3;
}
