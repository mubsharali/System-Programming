#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>

#define MAXLEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "JangoShell:- "

char* read_cmd(char*,FILE*);
char** tokenize(char* cmdline);
int execute(char* arglist[]);
int check_builtin(char* arglist[]);

int my_cd(char **args);
int my_help(char **args);
int my_exit(char **args);
char *blt_str[]={"cd","exit","help"};

int main(){

	char *cmdline;
	char** arglist;
	char*prompt= PROMPT;
	

	while((cmdline=read_cmd(prompt,stdin))!=NULL){
		if( (arglist=tokenize(cmdline))!=NULL){
			execute(arglist);
	
			for(int j=0;j<MAXARGS+1;j++)
				free(arglist[j]);

			free(arglist);
			free(cmdline);	
		} 
	
 	}

	printf("\n");
	return 0;	
}


int my_cd(char **args){

	if (args[1] == NULL){
	    fprintf(stderr, "Not proper Arguments...!\n");
	} 
	else{
		if(chdir(args[1]) != 0) {
			perror("Can't Change Directory...!\n");
		}
	}
	
	return 1;
}



int my_help(char **args){

	int i;
	printf("The following are built in Commands:\n");
	
	for (i = 0; i < 3; i++){
		printf("  %s\n", blt_str[i]);
	}
	
	return 1;
}



int my_exit(char **args){
	exit(0);
}


int execute(char* arglist[]){

	int status;
	
	if((check_builtin(arglist)) ==2){

		int cpid=fork();
		switch(cpid){
			case -1:
				perror("Fork failed...!\n");
				exit(1);
			case 0:
				execvp(arglist[0],arglist);
				perror("Command not found...!\n");	
				exit(1);
			default:
				waitpid(cpid,&status,0);
				return 0;
 		}
	}
}


int check_builtin(char* arglist[]){
	
	int num=-1;
	for(int i = 0; i <3; i++){
		if(strcmp(arglist[0], blt_str[i]) == 0){
			num=i;
		}
  	}

	if(num==-1)
		return 2;
	else if(num==0)
		return my_cd(arglist);
	else if(num==1)
		return my_exit(arglist);
	else
		return my_help(arglist);
}


char* read_cmd(char* prompt,FILE* fp){

	printf("%s",prompt);
	int c;
	int pos=0;
	char* cmdline=(char*) malloc(sizeof(char)*MAXLEN);
	while((c=getc(fp)) !=EOF){
		if(c=='\n')
			break;
		cmdline[pos++]=c;
	}
	
	if(c==EOF && pos==0)
		return NULL;

	cmdline[pos]='\0';
	return cmdline;	
}


char** tokenize(char* cmdline){

	char** arglist=(char**) malloc(sizeof(char*)*(MAXARGS+1));

	for(int i=0;i<MAXARGS+1;i++){
		arglist[i]=(char*) malloc(sizeof(char)*ARGLEN);
		bzero(arglist[i],ARGLEN);
	}
	
	char* cp=cmdline;
	char* start;
	int len;
	int argnum=0;

	while(*cp!='\0'){

		while(*cp==' '||*cp=='\t')
			cp++;
	
		start=cp;
		len=1;
	
		while(*++cp !='\0' && !(*cp==' '||*cp=='\t'))
			len++;
	
		strncpy(arglist[argnum],start,len);
		arglist[argnum][len]='\0';
		argnum++;
	}

	arglist[argnum]=NULL;
	return arglist;	
}
