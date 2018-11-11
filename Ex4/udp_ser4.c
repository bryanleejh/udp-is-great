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
	char recvs[DATALEN];
	struct ack_so ack;
	int end, n = 0, len;
	long lseek=0;
	end = 0;

	struct sockaddr_in addr;

	len = sizeof (struct sockaddr_in);

	printf("receiving data!\n");

	while(!end)
	{
		if ((n= recvfrom(sockfd, &recvs, DATALEN, 0, (struct sockaddr *)&addr, &len))==-1)                                   //receive the packet
		{
			printf("%d", sockfd);
			printf("Oh dear, something went wrong with recvfrom()! %s\n", strerror(errno));

			printf("error when receiving\n");
			exit(1);
		}
		if (recvs[n-1] == '\0')									//if it is the end of the file
		{
			end = 1;
			n --;
		}
		memcpy((buf+lseek), recvs, n);
		lseek += n;
	}
	ack.num = 1;
	ack.len = 0;
	if ((n = sendto(sockfd, &ack, 2, 0, (struct sockaddr *)&addr, &len))==-1)
	{
			printf("send error!");								//send the ack
			printf("%d", n);
			printf("Oh dear, something went wrong with sendto()! %s\n", strerror(errno));
			exit(1);
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
