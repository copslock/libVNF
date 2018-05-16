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
#include <pthread.h>

using namespace std;

//Receiver Address
#define MAX_THREADS 40
pthread_t server[MAX_THREADS];

void *action(void* i1)
{
//	int i = (int) *i1;
	int socketfd, portno, n;
	struct sockaddr_in rcvr_addr;
	struct hostent *rcvr;
	char buf[100];
	srand(time(NULL));
	portno = 5000;
	//cout<<"\nPort Num : "<<portno<<'\n';
	rcvr = gethostbyname("169.254.9.33");
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
	
	if(connect(socketfd, (struct sockaddr*)&rcvr_addr, sizeof(rcvr_addr)) < 0 ) {
		cout<<"Error : connecting"<<'\n';
		exit(-1);
	}
	cout<<"\n\t\t\tconnecting"<<'\n';
	strcpy(buf,"heelo");
//	sprintf(buf, "%d", r);
//	sleep(1);
	n = write(socketfd, buf, 5);
	if(n < 0){
		cout<<"Error : Write Error"<<'\n';
		exit(-1);
	}

	cout<<"Sent: "<<buf;
	n = read(socketfd, buf, 6);
	if( n < 0) {
		cout<<"Error: Read Error"<<'\n';
		exit(-1);
	}
//	n = atoi(buf);
//	r *=2;
	//if(n != r)
	cout<<"\tRcvd : \t"<<buf<<'\n';

	close(socketfd);
//	exit(0);
}

int main()
{
	int i,n,r;
//	while(1)
//	{
		//cout<<"Threads to spwan: \n";
		//cin>>n;

		for(i = 0; i <MAX_THREADS; i++)
		{
			pthread_create(&server[i], NULL, action, (void*)i);
			/*if( r == 0)	//Child Process
			{
				action(i);
			}*/
		}
//	}
	for(i=0;i<MAX_THREADS; i++){
		pthread_join(server[i],NULL);
	}
	
}
