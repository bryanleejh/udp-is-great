/**********************************
tcp_ser.c: the source file of the server in tcp transmission
***********************************/


#include "headsock.h"

#define BACKLOG 10

void str_ser(int sockfd); // transmitting and receiving function

int main(void)
{
	int sockfd, con_fd, ret;
	struct sockaddr_in my_addr;
	struct sockaddr_in their_addr;
	int sin_size;

//	char *buf;
	pid_t pid;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0); //create socket
	if (sockfd <0)
	{
		printf("error in socket!");
		exit(1);
	}

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYUDP_PORT);
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(my_addr.sin_zero), 8);
	ret = bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)); //bind socket
	if (ret <0)
	{
		printf("error in binding");
		exit(1);
	}

	printf("start receiving\n");
	str_ser(sockfd);
	close(sockfd);
	exit(0);
}

void str_ser(int sockfd)
{
	char buf[BUFSIZE];
	FILE *fp;
	char recvs[BUFSIZE];
	// struct pack_so recvs;
	struct ack_so ack;
	int end = 0, n = 0, len;
	long lseek=0;
	struct sockaddr_in addr;
	len = sizeof (struct sockaddr_in);
	int datalen = DATALEN;

	ack.num = 1;
	ack.len = 0;

	while(!end){
		printf("expecting %d packet size of %d bytes\n", ack.num, datalen);
		if ((n = recvfrom(sockfd, &recvs, (datalen), 0, (struct sockaddr *)&addr, &len))==-1)                                   //receive the packet
		{
			printf("error when receiving\n");
			exit(1);
		}
		printf("received packet size of %d bytes\n", n);

		if (recvs[n-1] == '\0')	//if it is the end of the file
		{
			end = 1;
			n --;
			printf("end of file %d\n", n);
		}

		memcpy((buf+lseek), recvs, n);
		lseek += n;

		printf("bytes received so far %d\n", lseek);

		if (ack.num == 2) {
			if ((n = recvfrom(sockfd, &recvs, (datalen), 0, (struct sockaddr *)&addr, &len))==-1)                                   //receive the packet
			{
				printf("error when receiving\n");
				exit(1);
			}
			printf("received packet size of %d bytes\n", n);

			if (recvs[n-1] == '\0')	//if it is the end of the file
			{
				end = 1;
				n --;
				printf("end of file %d\n", n);
			}

			memcpy((buf+lseek), recvs, n);
			lseek += n;

			printf("bytes received so far %d\n", lseek);
		}

		printf("ack\n");
		if ((n = sendto(sockfd, &ack, 2, 0, (struct sockaddr *)&addr, len))==-1)
		{
				printf("error when sending ack\n");	//send the ack
				exit(1);
		} else {
			printf("ack sent on ser side\n");
		}

		if (ack.num == 1)
			ack.num = 2;
		else
			ack.num = 1;

	}

	if ((fp = fopen ("myUDPreceive.txt","wt")) == NULL)
	{
		printf("file doesn't exit\n");
		exit(0);
	}
	fwrite (buf , 1 , lseek , fp); //write data into file
	fclose(fp);
	printf("a file has been successfully received!\nthe total data received is %d bytes\n", (int)lseek);
}
