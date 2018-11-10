/**************************************
udp_ser.c: the source file of the server in udp transmission
**************************************/
#include "headsock.h"

void str_ser1(int sockfd);                                                           // transmitting and receiving function

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in my_addr;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {			//create socket
		printf("error in socket");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYUDP_PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 8);
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) {           //bind socket
		printf("error in binding");
		exit(1);
	}

	printf("start receiving\n");
	while(1) {
		str_ser1(sockfd);                        // send and receive
	}
	close(sockfd);
	exit(0);
}

void str_ser1(int sockfd)
{
	char buf[BUFSIZE];
	FILE *fp;
	struct pack_so recvs;
	struct ack_so ack;
	int end, n = 0, ci, lsize=1, len;
	ci = end = ack.num = 0;
	struct sockaddr_in addr;

	len = sizeof (struct sockaddr_in);

	while(ci < lsize) {
		if ((n=recvfrom(sockfd, &recvs, MAXSIZE, 0, (struct sockaddr *)&addr, &len))==-1)                                   //receive the packet
		{
			printf("receiving error!\n");
			return;
		}
		else printf("%d data received\n", n);
		if (ci == 0) {
			lsize = recvs.len;								//copy the data length
			memcpy(buf, recvs.data, (n-HEADLEN));			//copy the data
			ci += n-HEADLEN;
		}
		else {
			memcpy((buf+ci), &recvs, n);
			ci += n;
		}
	}
	ack.len = 0;
	ack.num = 1;
//	memcpy(buf, recvs.data, recvs.len);

	if((fp = fopen ("myUDPreceive.txt","wt")) == NULL) {
		printf("File doesn't exit\n");
		exit(0);
	}
	printf("the data received: %d\n", ci);
	printf("the file size received: %d\n", lsize);
	fwrite (buf , 1 , lsize, fp);								//write the data into file
	fclose(fp);
	printf("a file has been successfully received!\n");

}
