#include <stdio.h>

struct dhcp_pkt{
	unsigned int siaddr; //server IP address
	unsigned int yiaddr; //your IP address
	unsigned int tran_ID; //Transaction ID
	unsigned short int lifetime; //LEase time of the IP address
	char buffer[1024];//represents the message from the client/server
};

int getSize(int x)
{
	int size;
	if(x == 24){size = 254;}
	else if(x == 25){size = 126;}
	else if(x == 26){size = 62;}
	else if(x == 27){size = 30;}
	else if(x == 28){size = 14;}
	else if(x == 29){size = 6;}
	else if(x == 30){size = 2;}

	return size;
}

unsigned int getMask(int x)
{
	unsigned int mask;
	if(x < 24 || x > 30)
	{
		printf("invali subnet_part. subnet_part must be greater than 23, less than 31 \n");
		exit(EXIT_FAILURE);
	}
	switch(x){

		case 24:
			mask = 0xFFFFFF0;
			break;
		case 25:
			mask = 0xFFFFFF80;
			break;
		case 26:
			mask = 0xFFFFFFC0;
			break;
		case 27:
			mask = 0xFFFFFFE0;
			break;
		case 28:
			mask = 0xFFFFFFF0;
			break;
		case 29:
			mask = 0xFFFFFFF8;
			break;
		case 30:
			mask = 0xFFFFFFFC;
			break;
	}

	return mask;
}


//the following code is from the following source: https://stackoverflow.com/questions/1680365/integer-to-ip-address-c
char * printIPAddress(unsigned int ipAddress, char *str)
{
	unsigned char ip[4];
	ip[0] = ipAddress & 0xFF;
	ip[1] = (ipAddress >> 8) & 0xFF;
	ip[2] = (ipAddress >> 16) & 0xFF;
	ip[3] = (ipAddress >> 24) & 0xFF;
	sprintf(str, "%d.%d.%d.%d", ip[3], ip[2], ip[1], ip[0]);
	return str;
}

void printInfo(struct dhcp_pkt *packet)
{
	char *str = malloc(sizeof(char) * 16);
	printf("siaddr: %s\n", printIPAddress(packet->siaddr, str));
	bzero(str, 12);
	printf("yiaddr: %s\n", printIPAddress(packet->yiaddr, str));
	bzero(str, 12);
	printf("Transaction ID: %d\n", packet->tran_ID);
	bzero(str, 12);
	printf("Lifetime: %d\n", packet->lifetime);
	free(str);
}

unsigned int getNetworkAddress(char *str)
{
	unsigned int total[4], sum =0;
	char *token;
	int i = 0;
	token = strtok(str, ".");
	while(token != NULL)
	{
		//printf("%s\n", token);
		total[i] = atoi(token);
		i++;
		token = strtok(NULL, ".");
	}

	sum = (total[0] * 16777216) + (total[1] * 65536) + (total[2] * 256) + total[3];
	return sum;
}
