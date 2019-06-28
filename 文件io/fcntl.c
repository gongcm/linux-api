#include <stdio.h>
#include <fcntl.h>
#include <dirent.h>

#include <pwd.h> // /etc/passwd
#include <grp.h> // 用户组 /etc/group

#include <netdb.h> // 

// struct hostent   /etc/hosts          getnameinfo         getaddrinfo
// struct netent    /etc/networks       getnetbyname        getnetbyaddr
// struct protent   /etc/protocols      Getprotobyname      getprotobynumber
// struct servent   /etc/services       getservbyname       getservbyport


int str_cmp(char * str,char * s){
    
    
    while(1)
    {
        if(*s != *str){
            return -1;
        }
        
        if( (*s=='\0' )&& (*str == '\0')){
            break;
        }

        if((*s == '\0') || (*str == '\0')){
            return -1;
        }
        s ++;
        str++;

    }
    return 0;
}

static struct passwd * my_getpwnam(char * name){
    struct passwd * ptr ;

    setpwent();

    while((ptr = getpwent())!= NULL){
        if(str_cmp(ptr->pw_name,name)==0){
            break;
        }
    }

    endpwent();

    return ptr;
}

int passwd_test(int uid){

    /*  根据用户id获取Linux系统用户信息
    
     */

    #if 0
    struct passwd {
	    char	*pw_name;		/* user name */
	    char	*pw_passwd;		/* encrypted password */
	    uid_t	pw_uid;			/* user uid */
	    gid_t	pw_gid;			/* user gid */
	    char	*pw_comment;		/* comment */
	    char	*pw_gecos;		/* Honeywell login info */
	    char	*pw_dir;		/* home directory */
	    char	*pw_shell;		/* default shell */
    };

    #endif
    struct passwd * pass_wd = getpwuid(uid);
    if(pass_wd){
        pass_wd = getpwnam(pass_wd->pw_name);
    }
    return 0;
}
int isBigEndian()
{
    // 小端模式 高地址存低字节
    // 大端模式 高地址存高字节

    union
    {
        int a;
        char b;

        /* data */
    }t;

    // 联合体是从低地址开始存放
    // 0x22 为高地址
    // 0x11 为低地址

    // char * p = "hello" 一般 *p = h  *(p++) = e

    // 假设 为小端模式： 高--->低 = 0x22 0x11

    t.a = 1;
    
    return (t.b == 1);
}

int m_aoti(char * s)
{
    int  num = 0;
    int jin = 1;
    char * p = s;
    int i = 0;
    int temp;
    int big = isBigEndian();
    
    //while(*p++ != '\0') i ++;
   
    p = s;
    while(*p != '\0'){
        num += (*p - '0')*jin;
        
        jin = jin * 10;
        p ++;
    }

    if(num < 0)
    {
        // 超出了int 表示的范围
    }

    return num;
}

int main(int argc, char const *argv[])
{
    int val = 0;
    if(argc < 2){
        sprintf(stderr,"usage ：\n \t Inpute file FD,return fd flag. like O_RDONLY etc. \n \t test fcntl function \n");
        return -1;
    }

    if(val = fcntl(argv[1],F_GETFL,0) < 0){
        return -1;
    }
    
    if(val & O_RDWR){
        printf("O_RDWR\n");
    }
    if(val & O_RDONLY){
        printf("O_RDONLY\n");
    }

    if(val & O_WRONLY){
        printf("O_WRONLY\n");
    }

    if(val & O_NONBLOCK){
        printf("O_NOBLOCK\n");
    }
    /* code */
    return 0;
}
