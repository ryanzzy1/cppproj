#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>

#define MAXSIZE 1024
#define IP_ADDR "127.0.0.1"
#define IP_PORT 8888

struct send_data{
	int b;
	char x;
	bool c;
};

struct LtReq
    {
        LtReq() = default;
        ~LtReq() = default;
        uint32_t fnCfg;
        uint16_t ltDura;
        bool led1_rq;
        bool led2_rq;
        bool led3_rq;
        bool led4_rq;
        bool led5_rq;
        bool led6_rq;
        bool led7_rq;
        bool led8_rq;
        bool led9_rq;
        bool led10_rq;
        bool led11_rq;
        bool led12_rq;
        bool led13_rq;
        bool led14_rq;
        bool led15_rq;
        bool led16_rq;
        bool led17_rq;
        bool led18_rq;
        bool led19_rq;
        bool led20_rq;
        bool led21_rq;
        bool led22_rq;
        bool led23_rq;
        bool led24_rq;
    };

int main()
{
	int i_listenfd, i_connfd;
	struct sockaddr_in st_sersock;
	// char msg[MAXSIZE];
	LtReq ltreq;

	char recvbuffer[MAXSIZE];
	char sendbuffer[MAXSIZE];

	// size_t length = sizeof(char) + sizeof(int) + sizeof(bool);
	// std::cout << "len_send_data: " << sizeof(send_data) << "  length: " << length << std::endl;

	std::cout << "ltreq_len:" << sizeof(ltreq) << " LrReq_str_len:" << sizeof(LtReq) << std::endl;
	memset(sendbuffer, 0, sizeof(sendbuffer));
	int nrecvSize = 0;

	if((i_listenfd = socket(AF_INET, SOCK_STREAM, 0) ) < 0)	//建立socket套接字
	{
		// printf("socket Error: %s (errno: %d)\n", strerror(errno), errno);
		std::cout << "socket Error:" <<  strerror(errno) << "errno:"  << errno << std::endl;
		exit(0);
	}

	memset(&st_sersock, 0, sizeof(st_sersock));
	st_sersock.sin_family = AF_INET;  //IPv4协议
	st_sersock.sin_addr.s_addr = htonl(INADDR_ANY);	//INADDR_ANY转换过来就是0.0.0.0，泛指本机的意思，也就是表示本机的所有IP，因为有些机子不止一块网卡，多网卡的情况下，这个就表示所有网卡ip地址的意思。
	st_sersock.sin_port = htons(IP_PORT);

	if(bind(i_listenfd,(struct sockaddr*)&st_sersock, sizeof(st_sersock)) < 0) //将套接字绑定IP和端口用于监听
	{
		// printf("bind Error: %s (errno: %d)\n", strerror(errno), errno);
				std::cout << "bind Error:" <<  strerror(errno) << "errno:"  << errno << std::endl;
		exit(0);
	}

	if(listen(i_listenfd, 20) < 0)	//设定可同时排队的客户端最大连接个数
	{
		// printf("listen Error: %s (errno: %d)\n", strerror(errno), errno);
		std::cout << "listen Error:" <<  strerror(errno) << "errno:"  << errno << std::endl;
		exit(0);
	}

	// printf("======waiting for client's request======\n");
	std::cout << "======waiting for client's request======\n";
	//准备接受客户端连接
	{
		if((i_connfd = accept(i_listenfd, (struct sockaddr*)NULL, NULL)) < 0)	//阻塞等待客户端连接
		{
			// printf("accept Error: %s (errno: %d)\n", strerror(errno), errno);
		std::cout << "accept Error:" <<  strerror(errno) << "errno:"  << errno << std::endl;
		//	continue;
		}	
		else
		{
			// printf("Client[%d], welcome!\n", i_connfd);
			std::cout << "Client[" <<  i_connfd << "], welcome!" << std::endl;
		}


		while(1)	//循环 接受客户端发来的消息并作处理(小写转大写)后回写给客户端
		{
			memset(recvbuffer, 0 ,sizeof(recvbuffer));
			if((nrecvSize = read(i_connfd, recvbuffer, MAXSIZE)) < 0)
			{
				// printf("accept Error: %s (errno: %d)\n", strerror(errno), errno);
				
				continue;
			}
			else if( nrecvSize == 0)	//read返回0代表对方已close断开连接。
			{
				// printf("client has disconnected!\n");
				std::cout << "client has disconnected!" << std::endl;				
				close(i_connfd);  //
				break;
			}
			else
			{
				// printf("recvMsg:%s\n", recvbuffer);
				// send_data sd;
				// memset(&sd, 0, sizeof(send_data));	// 清空结构体缓存
				// memcpy(&sd, recvbuffer, sizeof(send_data));
				// std::cout << "sd.b:" << sd.b << " sd.x:"<<sd.x << " sd.c:" << sd.c << std::endl;


				memset(&ltreq, 0, sizeof(ltreq));
				memcpy(&ltreq, recvbuffer, sizeof(ltreq));
				std::cout << "ltreq struct member:" << "\n" 
						  << "fnCfg:" << ltreq.fnCfg << "\n"
						  << "ltDura:" << ltreq.ltDura << "\n"
						  << "led1_rq:" << ltreq.led1_rq << "\n"
						  << "led2_rq:" << ltreq.led2_rq << "\n"
						  << "led3_rq:" << ltreq.led3_rq << "\n"
						  << "led4_rq:" << ltreq.led4_rq << "\n"
						  << "led5_rq:" << ltreq.led5_rq << "\n"
						  << "led6_rq:" << ltreq.led6_rq << "\n"
						  << "led7_rq:" << ltreq.led7_rq << "\n"
						  << "led8_rq:" << ltreq.led8_rq << "\n"
						  << "led9_rq:" << ltreq.led9_rq << "\n"
						  << "led10_rq:" << ltreq.led10_rq << "\n"
						  << "led11_rq:" << ltreq.led11_rq << "\n"
						  << "led12_rq:" << ltreq.led12_rq << "\n"
						  << "led13_rq:" << ltreq.led13_rq << "\n"
						  << "led14_rq:" << ltreq.led14_rq << "\n"
						  << "led15_rq:" << ltreq.led15_rq << "\n"
						  << "led16_rq:" << ltreq.led16_rq << "\n"
						  << "led17_rq:" << ltreq.led17_rq << "\n"
						  << "led18_rq:" << ltreq.led18_rq << "\n"
						  << "led19_rq:" << ltreq.led19_rq << "\n"
						  << "led20_rq:" << ltreq.led20_rq << "\n"
						  << "led21_rq:" << ltreq.led21_rq << "\n"
						  << "led22_rq:" << ltreq.led22_rq << "\n"
						  << "led23_rq:" << ltreq.led23_rq << "\n"
						  << "led24_rq:" << ltreq.led24_rq << "\n"
						  << std::endl;


				std::cout << "recvMsg: " << recvbuffer << std::endl;
				memset(sendbuffer, 0, sizeof(sendbuffer));
				memcpy(sendbuffer, recvbuffer, strlen(recvbuffer)+1); 
				// for(int i=0; recvbuffer[i] != '\0'; i++)
				// {
				// 	sendbuffer[i] = toupper(recvbuffer[i]);
					// std::cout << "test" << std::endl;
				// }
				if(write(i_connfd, sendbuffer, strlen(sendbuffer)+1) < 0)
				{
					// printf("accept Error: %s (errno: %d)\n", strerror(errno), errno);
					std::cout << "accept Error:" <<  strerror(errno) << "errno:"  << errno << std::endl;
				}
				std::cout << "writer after" << std::endl;
			}
		}
	}
	close(i_connfd);
	close(i_listenfd);

	return 0;
}
