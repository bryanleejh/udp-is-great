/**********************************
tcp_ser.c: the source file of the server in tcp transmission
***********************************/


#include "headsock.h"

#define BACKLOG 10

void str_ser(int sockfd);                                                        // transmitting and receiving function

int main(void)
{
	int sockfd, con_fd, ret;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int sin_size;

//	char *buf;
	pid_t pid;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);          //create socket
	if (sockfd <0)
	{
		printf("error in socket!");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYUDP_PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr("172.0.0.1");
	bzero(&(my_addr.sin_zero), 8);
	ret = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));                //bind socket
	if (ret <0)
	{
		printf("error in binding");
		exit(1);
	}

	while (1)
	{
		printf("waiting for data\n");
		sin_size = sizeof (struct sockaddr_in);

		str_ser(sockfd);
	}
	close(sockfd);
	exit(0);
}

void str_ser(int sockfd)
{
	char buf[BUFSIZE];
	FILE *fp;
	// char recvs[DATALEN];
	struct pack_so recvs;
	struct ack_so ack;
	int end = 0, n = 0, len;
	long lseek=0;
	// int end, n = 0, ci, lsize=1;
	// ci = end = ack.num = 0;

	struct sockaddr_in addr;

	len = sizeof (struct sockaddr_in);

	printf("receiving data!\n");

	while(!end)
	{
		if ((n= recvfrom(sockfd, &recvs, DATALEN, 0, (struct sockaddr *)&addr, &len))==-1)                                   //receive the packet
		{
			printf("error when receiving\n");
			exit(1);
		} else {
			printf("receiving\n");
			ack.num = 1;
			ack.len = 0;
			printf("ack\n");
			if ((n = sendto(sockfd, &ack, sizeof(ack), 0, (struct sockaddr *)&addr, len))==-1)
			{
					printf("send error!\n");								//send the ack
					printf("%d\n", n);
					printf("Oh dear, something went wrong with sendto()! %s\n", strerror(errno));
					exit(1);
			} else {
				printf("ack sent on ser side\n");
			}
		}

		if (recvs.data[n-1] == '\0')									//if it is the end of the file
		{
			end = 1;
			n --;
		}

		memcpy((buf+lseek), &recvs, n);
		lseek += n;
		printf("lseek %d\n", lseek);
		printf("n %d\n", n);
		printf("end %d\n", end);
	}

	if ((fp = fopen ("myUDPreceive.txt","wt")) == NULL)
	{
		printf("File doesn't exit\n");
		exit(0);
	}
	fwrite (buf , 1 , lseek , fp);					//write data into file
	fclose(fp);
	printf("a file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);
}
