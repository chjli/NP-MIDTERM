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
	if(argc != 2)
	{
		puts("Usage: server {port}");
		exit(1);					
	}
	struct sockaddr_in self_addr;
	bzero(&self_addr, sizeof(self_addr));
	self_addr.sin_addr.s_addr = (in_addr_t) 0x00000000;
	self_addr.sin_port = htons(atoi(argv[1]));
	self_addr.sin_family = AF_INET;
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
	//bind
	puts("Binding...");
	if(bind(s_fd, (struct sockaddr *) &self_addr, sizeof(self_addr)) == -1)
	{
		puts("Fail to bind.");
		exit(2);		
	}
	else
		puts("Bind successfully.");
	//listen
	if(listen(s_fd, 5) == -1)
	{
		puts("Fail to listen.");			
		exit(2);
	}
	else
		puts("Start listening...");
	//accept clients
	struct sockaddr_in client_addr;
	int client_addr_size;
	char recv[MAX_MESSAGE_LEN + 1];
	char send[MAX_MESSAGE_LEN + 1];

	int client_s_fd = accept(s_fd, (struct sockaddr *) &client_addr, &client_addr_size);
	if(client_s_fd == -1)
	{
		puts("Fail to accept.");
		exit(2);
	}
	else
		puts("Accepted from client. Receiving...");
	//process
	while(1)
	{
		int n, sum;
		if((n = read(client_s_fd, recv, MAX_MESSAGE_LEN)) == -1)
		{
			puts("Fail to receive command.");
			exit(2);	
		}			
		else
		{
			recv[n] = '\0';
			printf("Command from client:\n\t%s\n",recv);
		}
		char *str = strtok(recv, " \n");
		if(strcmp("EXIT",str) == 0)
			break;
		else if(strcmp("ADD",str) == 0)
		{
			int len, num, i, j;
			str = strtok(NULL," \n");
			num = str[0]-48;
			sum = 0;	
			for(i=0;i<num;i++)
			{
				str = strtok(NULL," \n");
				len = strlen(str);
				int k = 1, temp = 0;
				for(j=len-1;j>=0;j--)
				{		
					temp += (str[j]-48)*k;
					k*=10;
				}
				sum += temp;
			}
		}		
		else if(strcmp("MUL",str) == 0)
		{
			int len, num, i, j;
			str = strtok(NULL," \n");
			num = str[0]-48;
			sum = 1;
			for(i=0;i<num;i++)	
			{
				str = strtok(NULL," \n");		
				len = strlen(str);
				int k = 1, temp = 0;
				for(j=len-1;j>=0;j--)
				{
					temp += (str[j]-48)*k;
					k*=10;		
				}
				sum *= temp;		
			}
		}
		else
			puts("Error command.");

		sprintf(send, "%d", sum);
		printf("Sending back result: %s...\n",send);
		if((write(client_s_fd, send, strlen(send))) == -1)
		{
			puts("Fail to send back result.");
			exit(2);
		}	
		else
			puts("Result sent back.");
	}
	puts("Closing socket...");
	close(s_fd);
	puts("Close the connection.");		
	
	return 0;		
}
