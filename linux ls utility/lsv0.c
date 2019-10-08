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

	if(argc!=2){
		printf("Give One Argument...!\n");
		exit(0);
	}

	printf("Directory listing of %s :\n",argv[1]);
	do_ls(argv[1]);
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
		printf("%s\n",entry->d_name);
	}
	closedir(dp);
}











