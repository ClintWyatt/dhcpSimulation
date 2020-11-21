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

void printInfo(struct dhcp_pkt *packet)
{
	printf("siaddr: %d\n", packet->siaddr);
	printf("yiaddr: %d\n", packet->yiaddr);
	printf("Transaction ID: %d\n", packet->tran_ID);
	printf("Lifetime: %d\n", packet->lifetime);

}
