#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include <time.h>
#include <stdlib.h>
#include "common.h"


using namespace std;

//Receiver Address



void action(int i)
{
	
	int socketfd, portno, n;
	struct sockaddr_in rcvr_addr;
	struct hostent *rcvr;
	char buf[100];
	srand(time(NULL));
	int r = rand() % 100; 
	r += i;
	portno = PORTB;
	//cout<<"\nPort Num : "<<portno<<'\n';
	rcvr = gethostbyname(IPADDR);
	if(rcvr == NULL) {
		cout<<"Error:No such host"<<'\n';
		exit(-1);
	}

	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0) {
		cout<<"Error: Opening Socket"<<'\n';
		exit(-1);
	}

	bzero((char *) &rcvr_addr, sizeof(rcvr_addr));
	rcvr_addr.sin_family = AF_INET;
	bcopy((char *)rcvr->h_addr, (char *)&rcvr_addr.sin_addr.s_addr, rcvr->h_length);
	rcvr_addr.sin_port = htons(portno);
	//cout<<"\n\t\t\tconnecting"<<'\n';
	if(connect(socketfd, (struct sockaddr*)&rcvr_addr, sizeof(rcvr_addr)) < 0 ) {
		cout<<"Error : connecting"<<'\n';
		exit(-1);
	}
	sprintf(buf, "%d", r);
	//sleep(i);
	cout<<"Sent: "<<buf;
	n = write(socketfd, buf, strlen(buf));
	if(n < 0){
		cout<<"Error : Write Error"<<'\n';
		exit(-1);
	}

	n = read(socketfd, buf, 99);
	if( n < 0) {
		cout<<"Error: Read Error"<<'\n';
		exit(-1);
	}
	n = atoi(buf);
	r *=2;
	//if(n != r)
	cout<<"\tRcvd : \t"<<buf<<'\n';

	close(socketfd);
	exit(0);
}

int main()
{
	int i,n,r;
	while(1)
	{
		cout<<"Process to spwan: \n";
		cin>>n;

		for(i = 1; i <= n; ++i)
		{
			r = fork();
			if( r == 0)	//Child Process
			{
				action(i);
			}
		}
	}

	
}