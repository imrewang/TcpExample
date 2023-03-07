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
                        perror("errmsg");\
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
 
    int sockfd,acceptfd;
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
 
    //第三步，将套接字与服务器网络信息结构体绑定
    if(bind(sockfd,(struct sockaddr *)&serveraddr,addrlen)==-1)
    {
        ERRLOG("bind error");//sudo 要超级用户权限
    }
 
    //第四步,将套接字设置为被动监听状态
    if(listen(sockfd,5)==-1)
    {
        ERRLOG("listen error");
    }
 
    //第五步，阻塞等待客户端额连接
    if((acceptfd=accept(sockfd,(struct sockaddr *)&clientaddr,&addrlen))==-1)
    {
        ERRLOG("accept error");
    }
 
    printf("%s:%d is connected\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
 
    char buf[N]={0};
    ssize_t bytes;
    pid_t pid;
 
    //第六步，进行通信
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
            if(send(acceptfd,buf,N,0)==-1)
            {
                ERRLOG("send error");
            }
        }
    }
    else//子进程负责接收
    {
        while(1)
        {
            if((bytes=recv(acceptfd,buf,N,0))==-1)
            {
                ERRLOG("recv error");
            }
            else if(bytes==0)
            {
                printf("%s:%d 退出了\n",inet_ntoa(clientaddr.sin_addr),ntohs(clientaddr.sin_port));
                close(sockfd);
                close(acceptfd);
                exit(0);
            }
            else
            {
                printf("client:%s\n",buf);
            }
        }
    }
 
    return 0;
}