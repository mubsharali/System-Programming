#include<stdio.h>
#include<dirent.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<pwd.h>
#include<grp.h>
#include<sys/ioctl.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


extern int errno;
void do_ls(char*);
void do_ls_single(char*);
char get_type(char*);
static int
       cmpstringp(const void *p1, const void *p2){
           return strcmp(* (char * const *) p1, * (char * const *) p2);
       }

int main(int argc,char* argv[]){

	if(argc==1){
		do_ls_single(".");
		exit(0);
	}
	else if(argc==2 &&argv[1][0]=='-'){	 
		do_ls(".");
	}
	else{
		int i=0;
		while(++i<argc){
			printf("Directory Listing of %s :\n",argv[i]);
			do_ls_single(argv[i]);
		}
	}
	return 0;
}

void do_ls(char * dir){

	struct dirent * entry;
	struct stat info;
	int own=0,group=0,name=0;
	DIR * dp=opendir(dir);
	DIR * temp=opendir(dir);
	if(temp!=NULL){
		while((entry=readdir(temp))!=NULL){
			lstat(entry->d_name,&info);			
			int uid=info.st_uid;
			struct passwd * pwd=getpwuid(uid);
			struct group * grp=getgrgid(info.st_gid);
			int len=strlen(pwd->pw_name);
			if(len>own)
				own=len;
			len=strlen(grp->gr_name);
			if(len>group)
				group=len;
			len=strlen(entry->d_name);
			if(len>name)
				name=len;	
		}
		closedir(temp);	
	}
	if(dp==NULL){
		printf(stderr,"Cannot open directory %s\n",dir);
		return;
	}
	errno=0;
	while((entry=readdir(dp))!=NULL){
		if(entry==NULL && errno!=0){
			perror("readdir failed");
			exit(errno);
		}
		else{
			if(entry->d_name[0]!='.'){
				char str[11];
				strcpy(str,"----------");
				lstat(entry->d_name,&info);
				char c=get_type(entry->d_name);
				str[0]=c;
				int mode=info.st_mode;
				if((mode & 0000400)==0000400) str[1]='r';
				if((mode & 0000200)==0000200) str[2]='w';
				if((mode & 0000100)==0000100) str[3]='x';
	
				if((mode & 0000040)==0000040) str[4]='r';
				if((mode & 0000020)==0000020) str[5]='w';
				if((mode & 0000010)==0000010) str[6]='x';
			
				if((mode & 0000004)==0000004) str[7]='r';
				if((mode & 0000004)==0000002) str[8]='w';
				if((mode & 0000004)==0000001) str[9]='x';
			
				if((mode & 0004000)==0004000) str[3]='s';
				if((mode & 0002000)==0002000) str[6]='s';
				if((mode & 0001000)==0001000) str[9]='t';
	
				printf("%10s",str);
				printf("%2ld",info.st_nlink);	
				int uid=info.st_uid;
				struct passwd * pwd=getpwuid(uid);
				char* ch;
				printf("%*s",own+1,pwd->pw_name);
				struct group * grp=getgrgid(info.st_gid);
				printf("%*s",group+1,grp->gr_name);
				printf("%6ld",info.st_size);
				ch=ctime(&info.st_mtime);
/*
				for(int i=4;i<strlen(ch);i++){
					ch[i-4]=ch[i];		
				}
				ch[strlen(ch)-5]='\0';
*/
				ch[strlen(ch)-1]='\0';
				printf("%25s",ch);
				switch(c){
				
					case 'p':
						printf("%s%*s\n",KMAG,name+1,entry->d_name);					
					break;
					case 'c':
						printf("%s%*s\n",KGRN,name+1,entry->d_name);						
					break;
					case 'd':
						printf("%s%*s\n",KBLU,name+1,entry->d_name);					
					break;
					case 'b':
						printf("%s%*s\n",KRED,name+1,entry->d_name);				
					break;
					case '-':
						if(strcmp(entry->d_name,"a.out"))
							printf("%s%*s\n",KWHT,name+1,entry->d_name);
						else
							printf("%s%*s\n",KGRN,name+1,entry->d_name);;
					break;
					case 'l':
						printf("%s%*s\n",KCYN,name+1,entry->d_name);					
					break;
					case 's':
						printf("%s%*s\n",KYEL,name+1,entry->d_name);			
					break;
					case '.':
						printf("%*s\n",name+1,entry->d_name);					
					break;
				}
				printf("%s",KNRM);	
			}
		}
	}
	closedir(dp);
}

void do_ls_single(char * dir){
	
	struct dirent * entry;
	struct stat info;
	char *ary[500]={" "};
	int name=0,count=0,len=0;
	DIR * temp=opendir(dir);
	DIR * dp=opendir(dir);
	if(temp==NULL){
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
			if(entry->d_name[0]!='.'){
				lstat(entry->d_name,&info);
				len=strlen(entry->d_name);			
				if(len>name)
					name=len;			
				ary[count]=entry->d_name;
					count++;
			}
		}
	}
	
	qsort(ary, count, sizeof(char *), cmpstringp);
	struct winsize w;
	ioctl(STDOUT_FILENO,TIOCGWINSZ,&w);
	int cols=w.ws_col/(name+1);
	
	int flag=0;
	for(int i=0;i<count;i++){
		printf("%-*s",name+1,ary[i]);
		flag++;	
		if(flag==cols){
			printf("\n");
			flag=0;
		}
	}
	printf("\n");
}

char get_type(char *name){

	struct stat buf;
	lstat(name,&buf);
	int m=buf.st_mode;
	if((m & 0170000)==0010000)
		return 'p';
	else if((m & 0170000)==0020000)
		return 'c';
	else if((m & 0170000)==0040000)
		return 'd';
	else if((m & 0170000)==0060000)
		return 'b';
	else if((m & 0170000)==0100000)
		return '-';
	else if((m & 0170000)==0120000)
		return 'l';
	else if((m & 0170000)==0140000)
		return 's';
	else
		return '.';	
}











