#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <sys/wait.h>

#define MAXLEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "JangoShell:- "

int chk=0;
int num_args=0;
int num_pipes=0;
int num_semi=0;
char* read_cmd(char*,FILE*);
char** tokenize(char* cmdline);
char** tokenize_pipe(char* cmdline);
char** tokenize_semicolon(char* cmdline);
int execute(char* arglist[],char*);
int check_builtin(char* arglist[]);
void check_redirect(char* cmdline,char* arglist[]);
int chk_pipe(char*);
void run_pipe(char*);

int my_cd(char **args);
int my_help(char **args);
int my_exit(char **args);
char *blt_str[]={"cd","exit","help"};


int main(){

	char *cmdline;
	char** ttl_list;
	char** arglist;
	char*prompt= PROMPT;

	while((cmdline=read_cmd(prompt,stdin))!=NULL){
		ttl_list=tokenize_semicolon(cmdline);
		
		for(int i=0;i<num_semi;i++){
			if(chk_pipe(ttl_list[i])==0){	
				if( (arglist=tokenize(ttl_list[i]))!=NULL){
					execute(arglist,ttl_list[i]);
					
					for(int j=0;j<MAXARGS+1;j++)
						free(arglist[j]);
					free(arglist);
				}
			}//end of chk_pipe
			else{
				run_pipe(ttl_list[i]);	
			}
		}//end for loop

		for(int i=0;i<num_semi;i++)
			free(ttl_list[i]);
		
		free(ttl_list);
 	}//end while loop
	
	printf("\n");
	
	return 0;	
}

int chk_pipe(char *cmdline){
	
	char* cp=cmdline;
	num_pipes=0;
	
	while(*cp!='\0'){
	
		if(*cp =='|')
			num_pipes++;
		cp++;
	}
	
	return num_pipes;
}


void run_pipe(char* cmdline){

	char** cmdlist;
	char** arglist;
	cmdlist=tokenize_pipe(cmdline);
	int status;
	
	int fd1[2];
   	int fd2[2];
   	pipe(fd1);
   	pipe(fd2);
	
	if(num_pipes==2){ //Parent do a first fork to execute man

		if (fork() == 0){
			printf("chk1");
			sleep(3);
      			dup2(fd1[1], 1);//redirect output of man
      			close(fd1[0]);
      			close(fd2[0]);
      			close(fd2[1]);
			arglist=tokenize(cmdlist[0]);

			execvp(arglist[0],arglist);
   		}
		else if (fork() == 0){ //Parent do a second fork to execute grep
      			dup2(fd1[0], 0);//redirect stdin of grep
      			dup2(fd2[1], 1);//redirect stdout of grep
      			close(fd1[1]);
      			close(fd2[0]);
			arglist=tokenize(cmdlist[1]);

		      	execvp(arglist[0],arglist);
   		}
		else if (fork() == 0){ //Parent do a third fork to execute wc	 
 			dup2(fd2[0], 0);//redirect stdin of wc
      			close(fd1[0]);
      			close(fd1[1]);
      			close(fd2[1]);
    			arglist=tokenize(cmdlist[2]);
      			
			execvp(arglist[0],arglist);
   		}
		//Now the parent waits for three children
   
		close(fd1[0]);
		close(fd1[1]);
   		close(fd2[0]);
   		close(fd2[1]);
   		
		for(int i=0;i<3;i++)
      			wait(NULL);
	}//end of if condition
	else{
		if (fork() == 0){
      			dup2(fd1[1], 1);//redirect output of man
      			close(fd1[0]);
			arglist=tokenize(cmdlist[0]);
      			
			execvp(arglist[0],arglist);
   		}
		else if (fork() == 0){  //Parent do a second fork to execute grep
      			dup2(fd1[0], 0);//redirect stdin of grep
      			close(fd1[1]);
      			arglist=tokenize(cmdlist[1]);
      
			execvp(arglist[0],arglist);
   		}

		close(fd1[0]);
   		close(fd1[1]);
		close(fd2[0]);
   		close(fd2[1]);
   		
		for(int i=0;i<2;i++)
      			wait(NULL);

	}// end of else condition
 
}


void check_redirect(char* cmdline,char* arglist[]){
	
	char *f1;
	char *f2;
	f1=strchr(cmdline,'<');
	chk=0;
	int inp_chk=0;
	
	if(f1){
		close(0);
		if(num_args==1)
			open(arglist[1],O_RDWR, 0777);
		else
			open(arglist[2],O_RDWR, 0777);
		chk++;
		inp_chk=1;
	}

	f2=strchr(cmdline,'>');

	if(f2){
		close(1);
		if(num_args==1){
			if(inp_chk)
				open(arglist[2],O_RDWR|O_CREAT, 0777);
			else
				open(arglist[1],O_RDWR|O_CREAT, 0777);
		} //end of num_args==1
		else{
			if(inp_chk)
				open(arglist[3],O_RDWR|O_CREAT, 0777);
			else
				open(arglist[2],O_RDWR|O_CREAT, 0777);
	
		} //else part end of numargs==1
		
		chk++;
	}
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


int execute(char* arglist[],char *cmd){
	
	int status;
	if((check_builtin(arglist)) ==2){  //if 2 then not built-in, do normal task
	
		if(fork()==0){
			check_redirect(cmd,arglist);

		if(chk==0){
			execvp(arglist[0],arglist);
			perror("Command not found...!");
		}
		else{
			
			if(num_args==1){
				execlp(arglist[0],"myexec",NULL);
				perror("Command not found...!");
			}
			else{
				execlp(arglist[0],"myexec",arglist[1],NULL);
				perror("Command not found...!");
			}
		}
	   
		exit(1);
	} //end of child
	else
		wait(NULL);
		return 0;
 	}
	return 0;
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

	num_args=0;
	char** arglist=(char**) malloc(sizeof(char*)*(MAXARGS+1));

	for(int i=0;i<MAXARGS+1;i++){
		arglist[i]=(char*) malloc(sizeof(char)*ARGLEN);
		bzero(arglist[i],ARGLEN);
	}
	
	char* cp=cmdline;
	char* start;
	int len;
	int red=0;
	int argnum=0;

	while(*cp!='\0'){
		while(*cp==' '||*cp=='\t')
			cp++;

		while((*cp=='0'&&*(cp+1)=='<')||(*cp=='1'&&*(cp+1)=='>')){
			cp=cp+2;
			red++;
		}

		start=cp;
		len=1;

		while(*++cp !='\0' && !(*cp==' '||*cp=='\t'))
			len++;

		strncpy(arglist[argnum],start,len);
		arglist[argnum][len]='\0';
		argnum++;
	}
	
	num_args=argnum-red;
	arglist[argnum]=NULL;

	return arglist;	
}


char** tokenize_pipe(char* cmdline){

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
		while(*cp==' '||*cp=='\t'||*cp=='|')
			cp++;

		start=cp;
		len=1;
	
		while(*++cp !='\0' && !(*cp=='|'))
			len++;

		strncpy(arglist[argnum],start,len);
		arglist[argnum][len]='\0';
		argnum++;
	}

	arglist[argnum]=NULL;
	return arglist;	
}


char** tokenize_semicolon(char* cmdline){

	num_semi=0;
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

		while(*cp==' '||*cp=='\t'||*cp==';')
			cp++;
	
		start=cp;
		len=1;

		while(*++cp !='\0' && !(*cp==';'))
			len++;

		strncpy(arglist[argnum],start,len);
		arglist[argnum][len]='\0';
		argnum++;
	}

	num_semi=argnum;
	arglist[argnum]=NULL;

	return arglist;	
}


