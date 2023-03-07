#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
 
#define N 32
#define ERRLOG(errmsg) do{\
                        perror(errmsg);\
                        printf("%s-%s-%d\n",__FILE__,__func__,__LINE__);\
                        exit(1);\
                        }while(0);
 
int main(int argc,const char *argv[])
{
    if(argc<3)
    {
        fprintf(stderr,"Usage: %s <ip> <port>\n",argv[0]);
        exit(1);
    }
 
    int sockfd;
    struct sockaddr_in serveraddr,clientaddr;
    socklen_t addrlen=sizeof(struct sockaddr_in);
    
    //第一步，创建套接字
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        ERRLOG("socket error");
    }
 
    //第二部，填充网络信息结构体
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_addr.s_addr=inet_addr(argv[1]);
    serveraddr.sin_port=htons(atoi(argv[2]));
 
    //第三部分，向服务器发送连接请求
    if(connect(sockfd,(struct sockaddr *)&serveraddr,addrlen)==-1)
    {
        ERRLOG("connect error");
    }
 
    char buf[N]={0};
    ssize_t bytes;
    pid_t pid;
 
    //第四步，进行通信
    if((pid=fork())==-1)
    {
        ERRLOG("fork error");
    }
    else if(pid>0)//父进程负责发送
    {
        while(1)
        {
            memset(buf,0,N);
            fgets(buf,N,stdin);
            buf[strlen(buf)-1]='\0';
            if(send(sockfd,buf,N,0)==-1)
            {
                ERRLOG("send error");
            }
        }
    }
    else//子进程负责接收
    {
        while(1)
        {
            if((bytes=recv(sockfd,buf,N,0))==-1)
            {
                ERRLOG("recv error");
            }
            else if(bytes==0)
            {
                printf("服务器 %s:%s 已关闭\n",argv[1],argv[2]);
                close(sockfd);
                exit(0);
            }
            else
            {
                printf("server:%s\n",buf);
            }
        }
    }
    
 
    return 0;
}