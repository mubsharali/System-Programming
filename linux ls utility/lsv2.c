#include<stdio.h>
#include<dirent.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/stat.h>
#include<string.h>
#include<unistd.h>

extern int errno;
void do_ls(char*);
int main(int argc,char* argv[]){

	if(argc==1){
	 	do_ls(".");
		exit(0);
	}
	else{
		int i=0;
		while(++i<argc){
			printf("Directory Listing of %s :\n",argv[i]);
			do_ls(argv[i]);
		}
	}
	return 0;

}

void do_ls(char * dir){

	struct dirent * entry;
	DIR * dp=opendir(dir);
	if(dp==NULL){	
		printf(stderr,"Cannot Open Directory %s!!!\n",dir);
		return;
	}
	errno=0;
	while((entry=readdir(dp))!=NULL){
		if(entry==NULL && errno!=0){
			perror("readdir failed");
			exit(errno);
		}
		else{
		if(entry->d_name[0]!='.')
			printf("%s\n",entry->d_name);
	    	}
	}
	closedir(dp);
}











