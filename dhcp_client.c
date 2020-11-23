#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include "dhcpPacket.h"
#define SERVER "129.120.151.96" //ip address of the server

int main(int args, char **argv)
{
	if(args != 2)
	{
		printf("usage: ./a.out <portnumber> \n");
		return 0;
	}
	srand(time(NULL));
	struct sockaddr_in serv_addr;
	int sockfd, i=0, len, port, maxFd, wait;
	fd_set readFds; //used for select
	//client setup
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		perror("socket");
	}
	port = atoi(argv[1]);
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	len = sizeof(serv_addr);
	if (inet_aton(SERVER , &serv_addr.sin_addr) == 0) 
	{
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}
		
	char *str = malloc(sizeof(char) * 14);
	memcpy(str, "129.120.151.96", 14);
	struct dhcp_pkt *packet = (struct dhcp_pkt *) malloc(sizeof(struct dhcp_pkt));
	len = sizeof(serv_addr);

	packet->siaddr = getNetworkAddress(str);
	free(str);
      	packet->tran_ID = rand();	
	bzero(packet->buffer, 1024);
	memcpy(packet->buffer,  "DHCPDISCOVER", sizeof("DHCPDISCOVER"));
      	printInfo(packet);
	printf("\n");	
	if((sendto(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) == -1)
	{
		perror("sendto");
	}


	while(1)
	{
		if((recvfrom(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr *) &serv_addr, &len)) == -1)
		{
			perror("recvfrom");
		}
		else
		{
			if((strcmp(packet->buffer,"DHCPOFFER")) == 0)
			{
				bzero(packet->buffer, 1024);
				memcpy(packet->buffer, "DHCPREQUEST", sizeof("DHCPREQUEST"));
				packet->tran_ID +=1;
			       	printInfo(packet);
				printf("\n");	
				if((sendto(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr))) == -1)
				{
					perror("sendto");
				}
				printf("last packet sent \n");
			}
			else if((strcmp(packet->buffer,"DHCPACK")) == 0)
			{
				printInfo(packet);
				printf("\n");
				break;

			}
			else
			{
				printf("no ip addresses are left\n");
				break;
			}

		}


	}

	free(packet);
	close(sockfd);
	return 0;
}
