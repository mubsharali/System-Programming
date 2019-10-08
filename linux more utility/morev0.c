#include<stdio.h>
#include<stdlib.h>

#define LINELEN 512

void doMore(FILE*);

int main(int argc,char *argv[])
{
	if(argc==1){
		printf("This is the Help Page....!\n");
		exit(0);
	}
	
	FILE *fp;
     	fp=fopen(argv[1],"r");
	if(fp==NULL){
		printf("Error Opening File...!\n");
		exit(1);
	} 
	
	doMore(fp);
	fclose(fp);
	return 0;
}


	
 void doMore(FILE *fp ){

	char buff[LINELEN];
	while(fgets(buff,LINELEN,fp)){
	    fputs(buff,stdout);   
	
   }
}
