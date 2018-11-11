/*******************************
tcp_client.c: the source file of the client in tcp transmission
********************************/

#include "headsock.h"

float str_cli(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, long *len);                       //transmission function
void tv_sub(struct  timeval *out, struct timeval *in);	    //calcu the time interval between out and in

int main(int argc, char **argv)
{
	int sockfd, ret;
	float ti, rt;
	long len;
	struct sockaddr_in ser_addr;
	char ** pptr;
	struct hostent *sh;
	struct in_addr **addrs;
	FILE *fp;

	if (argc != 2) {
		printf("parameters not match");
	}

	sh = gethostbyname(argv[1]);	                                       //get host's information
	if (sh == NULL) {
		printf("error when gethostby name");
		exit(0);
	}

	printf("canonical name: %s\n", sh->h_name);					//print the remote host's information
	for (pptr=sh->h_aliases; *pptr != NULL; pptr++)
		printf("the aliases name is: %s\n", *pptr);
	switch(sh->h_addrtype)
	{
		case AF_INET:
			printf("AF_INET\n");
		break;
		default:
			printf("unknown addrtype\n");
		break;
	}

	addrs = (struct in_addr **)sh->h_addr_list;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);                           //create the socket
	if (sockfd <0)
	{
		printf("error in socket");
		exit(1);
	}
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(MYUDP_PORT);
	memcpy(&(ser_addr.sin_addr.s_addr), *addrs, sizeof(struct in_addr));
	bzero(&(ser_addr.sin_zero), 8);

	if((fp = fopen ("myfile.txt","r+t")) == NULL)
	{
		printf("File doesn't exit\n");
		exit(0);
	}

	ti = str_cli(fp, sockfd, (struct sockaddr *)&ser_addr, sizeof(struct sockaddr_in), &len);                       //perform the transmission and receiving
	rt = (len/(float)ti);                                         //caculate the average transmission rate
	printf("Time(ms) : %.3f, Data sent(byte): %d\nData rate: %f (Kbytes/s)\n", ti, (int)len, rt);

	close(sockfd);
	fclose(fp);
//}
	exit(0);
}

float str_cli(FILE *fp, int sockfd, struct sockaddr *addr, int addrlen, long *len)
{
	char *buf;
	long lsize, ci;
	// char sends[DATALEN];
	struct pack_so sends;
	struct ack_so ack;
	int n, isDouble = 0; //slen;
	float time_inv = 0.0;
	struct timeval sendt, recvt;
	int addr1len;
	struct sockaddr_in addr1;
	ci = 0;
	int x = DATALEN;

	fseek (fp , 0 , SEEK_END);
	*len = lsize = ftell (fp);
	printf("the lsize length is %ld bytes\n",lsize);
	rewind (fp);
	printf("The file length is %d bytes\n", (int)lsize);
	printf("the packet length is %d bytes\n", x+HEADLEN);

// allocate memory to contain the whole file.
	buf = (char *) malloc (lsize);
	if (buf == NULL) exit (2);

  // copy the file into the buffer.
	fread (buf,1,lsize,fp);

  /*** the whole file is loaded in the buffer. ***/
	buf[lsize] ='\0';									//append the end byte
	gettimeofday(&sendt, NULL);							//get the current time

	sends.num = 0;
	while(ci <= lsize) // while index less than file length keep sending
	{
		if (isDouble ==0){ // set 1 or 2 DU
			x = DATALEN;
			isDouble = 1;
			// printf("1 DU %d\n", x);
		}
		else {
			x = 2*DATALEN;
			isDouble = 0;
			// printf("2 DU %d\n", x);
		}

		if ((lsize+1-ci) <= x) {
			sends.len = lsize+1-ci+HEADLEN; //for the last packet
			printf("here \n");
		}
		else {
			sends.len = x+HEADLEN;
			printf("there \n");
		}
		memcpy(sends.data, (buf+ci), sends.len-HEADLEN);
		// printf("%s\n", sends.data);
		printf("copy done\n");
		if (ci != 0)	{ // if not first packet wait for ack
			if ((recvfrom(sockfd, &ack, 2, 0, (struct sockaddr *)&addr1, &addr1len)) == -1)//(n= recv(sockfd, &ack, 2, 0))==-1)                                   //receive the ack
			{
				printf("error when receiving ACK for packet\n");
				exit(1);
			}
			else {
				printf("Received ACK\n");
				n = sendto(sockfd, &sends, sends.len, 0, addr, addrlen);
			}
		}
		else {
			n = sendto(sockfd, &sends, sends.len, 0, addr, addrlen);//send(sockfd, &sends, slen, 0); in one packet
		}

		//check if packet is sent
		if(n == -1) {
			printf("send error!");		//send the data
			exit(1);
		}
		else printf("%d data sent\n", n);
		ci += sends.len-HEADLEN;
	}

	gettimeofday(&recvt, NULL);
	*len= ci;                                                         //get current time
	tv_sub(&recvt, &sendt);                                           // get the whole trans time
	time_inv += (recvt.tv_sec)*1000.0 + (recvt.tv_usec)/1000.0;
	return(time_inv);
}

void tv_sub(struct  timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) <0)
	{
		--out ->tv_sec;
		out ->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}
