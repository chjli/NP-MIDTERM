#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define MAX_MESSAGE_LEN 1024

int main(int argc,char *argv[])
{
	struct sockaddr_in addr;
	char *port;
	bzero(&addr,sizeof(addr));
	if(argc != 2 ||(addr.sin_addr.s_addr = inet_addr(strtok(argv[1],":"))) == INADDR_NONE || (port = strtok(NULL,":")) == NULL)
	{
		puts("Usage: client xxx.xxx.xxx.xxx:xxxx");
		puts("              ^ Server IP     ^ Port");
		exit(1);					
	}
	else
	{
		addr.sin_port = htons(atoi(port));
		addr.sin_family = AF_INET;	
	}	
	//socket
	puts("Creating socket...");
	int s_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(s_fd == -1)
	{
		puts("Fail to create socket.");
		exit(2);	
	}	
	else
		puts("Socket created.");
	//connect
	printf("Connecting to %s:%s...\n", argv[1], port);
	if(connect(s_fd, (struct sockaddr *) &addr , sizeof(addr)) == -1)
	{
		puts("Fail to connect.");
		exit(2);		
	}
	else
		puts("Server connected.");
	char send[MAX_MESSAGE_LEN + 1];
	char recv[MAX_MESSAGE_LEN + 1];
	int n;
	while(1)
	{
		printf("Enter command to server: ");
		fgets(send,MAX_MESSAGE_LEN,stdin);
		printf("Sending to server...\n");
		if(write(s_fd, send, strlen(send)) == -1)
		{
			puts("Fail to send command to server.");
			exit(2);	
		}			
		else
			puts("Command sent.");
		puts("Receiving result from server...");
		
		if((n = read(s_fd, recv, MAX_MESSAGE_LEN)) == 0)
				break;
		else
		{
			recv[n] = '\0';
			printf("Result from server:\n\t%s\n",recv);
		}
	}
	puts("Closing socket...");
	close(s_fd);
	puts("The server has closed the connection.");		
	
	return 0;		
}
