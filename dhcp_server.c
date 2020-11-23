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
	//you type the network address as an IP address
	if(args != 4)
	{
		printf("usage: ./server <portnumber> <network address> <subnet_part>\n");
		return 0;
	}
	struct sockaddr_in si_me, si_other;
	unsigned int ipAddress, mask, *ipAddresses;
	int port, recv_len, sendLen, maxfd, subnet_part, sockfd, size, index, *bitArray, cliLen;
	port = atoi(argv[1]);
	subnet_part = atoi(argv[3]);

	//creating the socket
	if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		perror("socket");
	}

	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	//bind socket to port
	if(bind(sockfd, (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		perror("bind");
	}	
	//setting up the ip addresses based on the subnet_port
	mask = getMask(subnet_part);

	int netAddress = getNetworkAddress(argv[2]);
	netAddress = netAddress & mask;
	//printf("%d \n", netAddress);

	//setIP_addresses(ipAddresses, subnet_part);
	unsigned int i, j, *arrSize;
	size = getSize(subnet_part);
	ipAddresses = (unsigned int *)malloc(sizeof(unsigned int) * (size -2));
	bitArray = (int *)malloc(sizeof(int) * (size -2));
	for(i =netAddress +1, j =0; j < size-2; i++, j++)
	{
		ipAddresses[j] = i;
	}
	bzero(bitArray, size);//seroing out the bitArray

	printf("%d \n", size);
	struct dhcp_pkt *packet = (struct dhcp_pkt *) malloc(sizeof(struct dhcp_pkt));
	packet->yiaddr = netAddress;
	cliLen = sizeof(si_other);
	//printInfo(packet);
	while(1)
	{

		if((recvfrom(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr *) &si_other, &cliLen)) == -1)
		{
			perror("recv_from");
		}
		else
		{
			printf("%s\n", packet->buffer);
			if(strcmp(packet->buffer, "DHCPDISCOVER") == 0)//discover IP address
			{
				//find an available ip address
				for(i =0; i < size-2; i++)
				{
					if(bitArray[i] == 0)
					{
						index = i;
						bitArray[i] = 1;
						break;
					}
				}

				if(i == size -2)
				{
					printf("No more IP addressses are left\n");
					memcpy(packet->buffer, "not available", sizeof("not available"));
					if((sendto(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr*) &si_other, cliLen))== -1)
					{
						printf("send error: \n");
						perror("send");
					}

				}
				else
				{

					packet->yiaddr = ipAddresses[index];
					packet->lifetime=3600;
					bzero(packet->buffer, 1024);
					memcpy(packet->buffer, "DHCPOFFER",9);
					printInfo(packet);
					printf("\n");
					if((sendto(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr*) &si_other, cliLen))== -1)
					{
						printf("send error: \n");
						perror("send");
					}
					bzero(packet->buffer, 1024);
				}
			}	
			else if(strcmp(packet->buffer, "DHCPREQUEST") == 0) //request IP address
			{
				bzero(packet->buffer, 1024);
				memcpy(packet->buffer, "DHCPACK",7);
				printInfo(packet);
				printf("\n");
				if((sendto(sockfd, packet, sizeof(struct dhcp_pkt), 0, (struct sockaddr*) &si_other, cliLen))== -1)
				{
					printf("waahhh\n");	
					perror("send");
				}	
			}

		}
	}

	return 0;
}


