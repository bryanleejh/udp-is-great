/*******************************
udp_client.c: the source file of the client in udp
********************************/

#include "headsock.h"

void str_cli(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, long *len);

int main(int argc, char *argv[]) {
	int sockfd, ret;
	float ti, rt;
	long len;
	struct sockaddr_in ser_addr;
	char ** pptr;
	struct hostent *sh;
	struct in_addr **addrs;
	FILE *fp;

	if (argc!= 2) {
		printf("parameters not match.");
		exit(0);
	}

	if ((sh=gethostbyname(argv[1]))==NULL) {             //get host's information
		printf("error when gethostbyname");
		exit(0);
	}

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);             //create socket
	if (sockfd<0) {
		printf("error in socket");
		exit(1);
	}

	addrs = (struct in_addr **)sh->h_addr_list;
	printf("canonical name: %s\n", sh->h_name);
	for (pptr=sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr);			//printf socket information
	switch(sh->h_addrtype) {
		case AF_INET:
			printf("AF_INET\n");
		break;
		default:
			printf("unknown addrtype\n");
		break;
	}

	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(MYUDP_PORT);
	memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
	bzero(&(ser_addr.sin_zero), 8);

	if((fp = fopen ("myfile.txt","r+t")) == NULL)		//open local file to read the data
	{
		printf("File doesn't exit\n");
		exit(0);
	}

	str_cli(fp, sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in), &len);                       //perform the transmission and receiving

	close(sockfd);
	fclose(fp);

	exit(0);
}

void str_cli(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, long *len)
{
	long lsize;
	struct pack_so sends;
	struct ack_so acks;
	int n;
	float time_inv = 0.0;
	struct timeval sendt, recvt;

	fseek (fp , 0 , SEEK_END); // sets the file position of the stream to the given offset
	*len= lsize = ftell (fp); // returns the current file position of the given stream
	rewind (fp); // sets pointer to start of file
	printf("The file length is %d bytes\n", (int)lsize);


  // copy the file into the buffer.
	fread (sends.data,1,lsize,fp); //read the file data into the data area in packet
	printf("Copy file into buffer successfully\n");

  /*** the whole file is loaded in the buffer. ***/

	gettimeofday(&sendt, NULL); //get the current time

	sends.len = lsize; //the data length
	sends.num = 0;

	printf("This is %d\n", sends.num);
	printf("This is %d\n", sends.len);
	n=sendto(sockfd, &sends, (sends.len), 0, addr, addrlen); //send the data in one packet
	if (n == -1)	{
		printf("error sending data\n");
		printf("Oh dear, something went wrong with read()! %s\n", strerror(errno));
		printf("Oh dear, something went wrong with read()! %d\n", errno);
		exit(1);
	}a
	else printf("%d data sent", n);
}
