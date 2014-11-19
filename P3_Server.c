#include<sys/types.h>
#include<sys/socket.h>
#include<sys/select.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define CMD_MAX 1000
int main(int argc,char *argv[])
{
	if(argc!=2)
	{
		fprintf(stderr, "Usage: %s <port>\n",argv[0]);
		exit(1);		
	}
	int listen_fd;
	int clients[FD_SETSIZE];
	int client_maxi = 0;
	int max_fd;
	memset(clients , 0xFF, sizeof(clients));
	
	struct sockaddr_in listen_addr;
	memset(&listen_addr, 0, sizeof(listen_addr));
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(atoi(argv[1]));
		
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd == -1)
	{
		fputs("Cannot create socket.\n",stderr);
		exit(2);
	}
	if(bind(listen_fd, (struct sockaddr *) &listen_addr, sizeof(listen_addr)) == -1)
	{
		fputs("Cannot bind.\n",stderr);
		exit(2);
	}
	if(listen(listen_fd ,1024)== -1)
	{
		fputs("Cannot listen.\n",stderr);
		exit(2);
	}
	
	fd_set all_set, r_set;
	FD_SET(listen_fd, &all_set);
	max_fd = listen_fd + 1;
	
	while(1)
	{
		r_set = all_set;
		int nready = select(max_fd, &r_set, NULL, NULL, NULL);
		
		if(FD_ISSET(listen_fd, &r_set))
		{
			struct sockaddr_in client_addr;
			int client_len = sizeof(client_addr);
			int client_fd = accept(listen_fd, (struct sockaddr *) &client_addr , &client_len);
			int i;
			for(i=0;i<FD_SETSIZE;i++)
			{
				if(clients[i]<0)
				{
					clients[i]=client_fd;
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				printf("Too many!\n");
				exit(2);
			}
			if(i>client_maxi)
				client_maxi=i;
			if(client_fd>=max_fd)
				max_fd = client_fd + 1;
			
			FD_SET(client_fd, &all_set);
			if(--nready)
				continue;
		}
	int i;
	for(i=0;i<=client_maxi&&nready;i++)
		{
		if(FD_ISSET(clients[i], &r_set))
			{
				--nready;
				
				char buff[CMD_MAX];
				char err[]={"error command"};
				int n = read(clients[i],buff,999);
				/////process
				buff[n] = '\0';					
				int sum;				
				char send[CMD_MAX +1];
				char *str = strtok(buff, " \n");
				
				if(strcmp("EXIT",str) == 0)
				{
					puts("Client Closed.");
					close(clients[i]);
					FD_CLR(clients[i],&all_set);
					clients[i] =  -1;
				}
				else
				{
					if(strcmp("ADD",str) == 0)
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
					{
						puts("Error command.");
						write(clients[i], err, strlen(err));
						break;
					}

					sprintf(send, "%d", sum);
					printf("Sending back result: %s...\n",send);
					if((write(clients[i], send, strlen(send))) == -1)
					{
						puts("Fail to send back result.");
						exit(2);
					}	
					else
						puts("Result sent back.");
				}
			}			
		}		
	}
}

