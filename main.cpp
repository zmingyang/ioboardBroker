/*************************************************************************
    > File Name: main.c
    > Author: zhao ming yang
    > Mail: 13720042437@163.com
    > Created Time: Fri 01 Mar 2022 04:15:27 PM CST
 ************************************************************************/

#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<signal.h>
#include <iostream> 
#include <thread>

#include <string>

#include <sys/prctl.h>      /* Definition of SYS_* constants */

#include "msgQueue.hpp"
#include "mqtt.h"
#include "cJSON.h"

#define ERR_EXIT(m) \
     do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    }  while ( 0)

void do_service( int);
static int conn = 0;  // 已连接套接字(变为主动套接字，即可以主动connect)


int main( void)
{
   signal(SIGCHLD, SIG_IGN);

   // char name[32];
   // prctl ( PR_SET_NAME, "hello\0",NULL, NULL, NULL);
   // prctl(PR_GET_NAME,(unsigned long)name);
   // printf("%s \n", name);
   // while(1)
   // sleep(1);

   //prctl ( PR_SET_NAME, inet_ntoa(peeraddr.sin_addr), NULL, NULL, NULL);
     
  int listenfd;  //被动套接字(文件描述符），即只可以accept, 监听套接字
     if ((listenfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) <  0)
         //  listenfd = socket(AF_INET, SOCK_STREAM, 0)
        ERR_EXIT( "socket error");

      struct sockaddr_in servaddr;
      memset(&servaddr,  0,  sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_port = htons( 8234 );
      servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
     /* servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); */
     /* inet_aton("127.0.0.1", &servaddr.sin_addr); */

     int on =  1;
     if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on,  sizeof(on)) <  0)
        ERR_EXIT( "setsockopt error");

     if (bind(listenfd, ( struct sockaddr *)&servaddr,  sizeof(servaddr)) <  0)
        ERR_EXIT( "bind error");

     if (listen(listenfd, SOMAXCONN) <  0)  //listen应在socket和bind之后，而在accept之前
        ERR_EXIT( "listen error");

     struct sockaddr_in peeraddr;  //传出参数
    socklen_t peerlen =  sizeof(peeraddr);  //传入传出参数，必须有初始值

    pid_t pid;

     while ( 1)
    {
         if ((conn = accept(listenfd, ( struct sockaddr *)&peeraddr, &peerlen)) <  0)  //3次握手完成的序列
            ERR_EXIT( "accept error");



        pid = fork();
         if (pid == - 1)
            ERR_EXIT( "fork error");
         if (pid ==  0)
        {
             // 子进程
            close(listenfd);

            printf( "recv connect ip=%s port=%d\n", inet_ntoa(peeraddr.sin_addr),
               ntohs(peeraddr.sin_port));


            connectMQTT( std::string(inet_ntoa(peeraddr.sin_addr)),  std::to_string(ntohs(peeraddr.sin_port)));

            do_service(conn);
            exit(EXIT_SUCCESS);
        }
         else
            close(conn);  //父进程
    }



     return  0;
}


void receiveThr()
{
   unsigned char recvbuf[ 16];
   char convertBuf[16];
   std::string msg;
   int count = 0;
   while ( 1)
   {
      memset(recvbuf,  0,  sizeof(recvbuf));
      int ret = recv(conn, recvbuf,  1, 0);
      printf("receive msg %d\n", ret);
      if (ret ==  0)    //客户端关闭了
      {
         printf( "client close\n");
         break;
      }
      else  if (ret == - 1)
         ERR_EXIT( "read error");


      if(count < 9)
      {
        
         sprintf(convertBuf,"%02x",recvbuf[0]);
         msg += std::string( convertBuf) + ",";
         count++;
      }
      else if(count == 9)
      {
         sprintf(convertBuf,"%02x",recvbuf[0]);
         msg += std::string( convertBuf);



         Mosq_SendPrivateTopic((char*)msg.c_str(), msg.length());
         std::cout << msg << std::endl;
         msg.clear();
         count = 0;
      }
      
   }
}
// 使用字符分割
void Stringsplit(const std::string& str, const char split, std::vector<std::string>& res)
{
	if (str == "")		return;
	//在字符串末尾也加入分隔符，方便截取最后一段
	std::string strs = str + split;
	size_t pos = strs.find(split);

	// 若找不到内容则字符串搜索函数返回 npos
	while (pos != strs.npos)
	{
		std::string temp = strs.substr(0, pos);
		res.push_back(temp);
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(split);
	}
}
void sentThr()
{
   char recvbuf[ 1024];
   while ( 1)
    {
      std::string msg;
      ToIOBoardQueue.getMsg(msg);


      cJSON* root;
      cJSON* element;
      root = cJSON_Parse(msg.c_str());
      std::string devProperty = cJSON_GetStringValue(cJSON_GetObjectItem(root, "devProperty"));
      cJSON_Delete(root);

      printf("msg %s\n", devProperty.c_str());

      std::vector<std::string> strGroup;
      Stringsplit(devProperty, ',', strGroup);


      unsigned char buf[10] = {0};
      printf("%d \n", strGroup.size());

      for (int i = 0; i < strGroup.size(); i++)
      {
         int nValude = 0;
         sscanf(strGroup[i].c_str(), "%x", &nValude);

         buf[i] = nValude;
      }

      for(int i = 0; i < 10; i++)
      {
         printf( "#16  %x \n", buf[i] );
      }
     int wn = send(conn, buf, 10, 0);
     
    }
}
void do_service( int conn)
{


   std::thread recv (receiveThr); 
   std::thread sent (sentThr); 



  // synchronize threads:
  recv.join();                // pauses until first finishes
  sent.join();               // pauses until second finishes

  std::cout << "foo and bar completed.\n";

  
}
