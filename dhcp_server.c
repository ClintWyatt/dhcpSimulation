#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
//#include <pthread.h>
//#include <semaphoer.h>
#include "dhcpPacket.h"
#include <math.h>

int main(int args, char **argv)
{

	if(args != 4)
	{
		printf("usage: ./server <portnumber> <network address> <subnet_part>\n");
		return 0;
	}
	struct sockaddr_in servaddr, client;
	unsigned int ipAddress, mask, *ipAddresses;
	int port, recv_len, sendLen, maxfd, subnet_part, sockfd, size, index, *bitArray;
	socklen_t cliLen;
	char buffer[1024];
	port = atoi(argv[1]);
	subnet_part = atoi(argv[3]);

	//setting up the ip addresses based on the subnet_port
	mask = getMask(subnet_part);
	//setIP_addresses(ipAddresses, subnet_part);
	unsigned int i, j, *arrSize;
	size = getSize(subnet_part);
	ipAddresses = (unsigned int *)malloc(sizeof(unsigned int) * size);
	bitArray = (int *)malloc(sizeof(int) * size);
	for(i =0xFFFFFF0, j =0; i < 0xFFFFFF0 + size; i++, j++)
	{
		ipAddresses[j] = i;
	}
	bzero(bitArray, size);//seroing out the bitArray
	//printf("%d\n", size);
	//printf("%d\n", arrSize[1]); 


	//creating the socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket");
		exit(EXIT_FAILURE);
	}
	//setting up the server details
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//binding the socket
	if(bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == -1)
	{
		perror("bind");
		exit(EXIT_FAILURE);
	}
	//allocating memory for the packet
	struct dhcp_pkt *packet = (struct dhcp_pkt *) malloc(sizeof(struct dhcp_pkt));
	cliLen = sizeof(client);
	while(1)
	{

		if((recvfrom(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr *) &servaddr, &cliLen)) == -1)
		{
			perror("recv_from");
		}
		else
		{
			if(strcmp(packet->buffer, "DHCPDISCOVER") == 0)//discover IP address
			{
				//find an available ip address
				for(i =0; i < size; i++)
				{
					if(bitArray[i] == 0)
					{
						index = i;
						break;
					}
				}
				packet->yiaddr = ipAddresses[index];
				packet->lifetime=3600;
				bzero(packet->buffer, 1024);
				memcpy(packet->buffer, "DHCPOFFER",9); 
				if((sendto(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr*) &client, cliLen))== -1)
				{
					perror("send");
				}

			}	
			else if(strcmp(packet->buffer, "DHCPREQUEST") == 0) //request IP address
			{
				if((recvfrom(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr *) &servaddr, &cliLen)) == -1)
				{
					perror("recv_from");
				}
				bzero(packet->buffer, 1024);
				memcpy(packet->buffer, "DHCPACK",7);
                                if((sendto(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr*) &client, cliLen))== -1)
                                {
                                        perror("send");
                                }	
			}

		}
	}

	return 0;
}


