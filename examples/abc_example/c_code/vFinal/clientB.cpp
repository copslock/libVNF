#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

#include "common.h"

#include <set>
#include <iterator>
#include <map>

using namespace std;


//int num_conn;
//int max_conn;

struct cadata{
	int fd,num;
};

int make_socket_nb(int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      cout<<"Error: NBS fcntl"<<'\n';
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      cout<<"Error: NBS fcntl flags"<<'\n';
      return -1;
    }

  return 0;
}

void run()
{

	int lsfd, acfd, portno, data, n, numev, i, ccfd, cafd, cret;
	char buf[110];
	
	int count,tcount;
	
	struct sockaddr_in server, c_addr;
	struct hostent *c_ip;
	
	set<int> srca, srcc, srcr; //set-C-accept/connect/read;
	map<int, cadata> mm;
	struct cadata cd;

	lsfd = socket(AF_INET, SOCK_STREAM, 0);

	if(lsfd < 0) {
    	cout<<"ERROR : opening socket"<<'\n';
      	exit(-1);
   	}

	bzero((char *) &server, sizeof(server) );
   	portno = PORTB;
   	cout<<"\nPort Num : "<<portno<<'\n';

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(portno);

    if (bind(lsfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
      	cout<<"ERROR: BIND ERROR"<<'\n';
      	exit(-1);      
    }

    make_socket_nb(lsfd);

   	listen(lsfd, MAXCON);

   	int epfd = epoll_create(MAXEVENTS + 5);
   	if( epfd == -1){
   		cout<<"Error: epoll create"<<'\n';
   		exit(-1);
   	}

   	int retval;
   	struct epoll_event ev, rev[MAXEVENTS];

   	ev.data.fd = lsfd;
   	ev.events = EPOLLIN | EPOLLET;

   	retval = epoll_ctl( epfd, EPOLL_CTL_ADD, lsfd, &ev);
   	if( retval == -1) {
   		cout<<"Error: epoll ctl lsfd add"<<'\n';
   		exit(-1);
   	}

   	cout<<"Entering Loop"<<'\n';
   	count = 0;
   	tcount=0;


   	while( 1 ){

   		numev = epoll_wait( epfd, rev, MAXEVENTS, 500);
   		if(numev < 0){
			cout<<"Error: EPOLL wait!"<<'\n';
			exit(-1);
		}

		if(numev == 0){
			//cout<<"Tick "<<'\n';
		}

		for( i = 0; i < numev; i++){

			//Check Errors
			if(	(rev[i].events & EPOLLERR) ||
				(rev[i].events & EPOLLHUP)	
				) {
				cout<<"ERROR: epoll monitoring failed, closing fd"<<'\n';
				if(rev[i].data.fd == lsfd){
					cout<<"Oh Oh, lsfd it is"<<'\n';
					exit(-1);
				}
				close(rev[i].data.fd);
				continue;
			}

			else if( rev[i].data.fd == lsfd ) { 	//listening socket!!
							
				//Check if events full				? 		LATER LATER LATER



				while(1){
					
					acfd = accept(lsfd, NULL, NULL);
					
					if(acfd < 0){
						
						if((errno == EAGAIN) ||	//Need lsfd non blocking to run this!!!!!!
						   (errno == EWOULDBLOCK))
						{
							//processed all connections !!!
							break;
						}
						else
						cout<<"Error on accept"<<'\n';
						break;
						//exit(-1);
					}

					make_socket_nb(acfd);
					ev.data.fd = acfd;
					ev.events = EPOLLIN | EPOLLET;
					retval = epoll_ctl( epfd, EPOLL_CTL_ADD, acfd, &ev);
   					if( retval == -1) {
   						cout<<"Error: epoll ctl lsfd add"<<'\n';
   						exit(-1);
   					}
   					tcount++;

   					srca.insert(acfd);

				}//End While 1 for accept
				//cout<<"conn handled total : "<<tcount<<'\n';
				
			}
			else 	
			if( srca.find(rev[i].data.fd) != srca.end() ){									//accepted fd;

				cafd = rev[i].data.fd;
				bzero(buf, 100);
		        n = read(cafd, buf, 100);
		        if ( n < 0) {
		           cout<<"Error : Read Error "<<'\n';
		           exit(-1);
		        }
		        //	Connect C

		        portno = PORTC;
		        
		        c_ip = gethostbyname(IPADDR);
		        if(c_ip == NULL)
		        {
		        	cout<<"ERROR: C Ip error"<<'\n';
		        	exit(-1);
		        }

		        ccfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
		        if( ccfd < 0){
		        	cout<<"Error: C socket open"<<'\n';
		        	exit(-1);
		        }

				bzero((char *) &c_addr, sizeof(c_addr));
				c_addr.sin_family = AF_INET;
				bcopy((char *)c_ip->h_addr, (char *)&c_addr.sin_addr.s_addr, c_ip->h_length);
				c_addr.sin_port = htons(portno);
				

				cret = connect(ccfd, (struct sockaddr*)&c_addr, sizeof(c_addr));
				if((cret == -1) && (errno == EINPROGRESS))				//Not Connect Immediately;
				{
					cout<<"Connect C using epoll"<<'\n';
					ev.data.fd = ccfd;
					ev.events = EPOLLOUT | EPOLLET;
					retval = epoll_ctl(epfd, EPOLL_CTL_ADD, ccfd, &ev);
					if(retval == -1)
					{
						cout<<"Error : Adding connection to C to epoll"<<'\n';
						exit(-1);
					}
					srca.erase(cafd);
					srcc.insert(ccfd);
					cd.fd = cafd;
					cd.num = atoi(buf);
					mm.insert(make_pair(ccfd, cd));
				}
				else
				if(cret == -1)
				{
					cout<<"Error: Client C connect error"<<'\n';
					exit(-1);
				}
				else						//Connection EsTD;
				{
					n = write(ccfd, buf, strlen(buf));
					if(n < 0){
						cout<<"Error : C Write Error"<<'\n';
						exit(-1);
					}
					cout<<"Conn Now, Watching socket for reads"<<'\n';
					ev.data.fd = ccfd;
					ev.events = EPOLLIN | EPOLLET;
					
					retval = epoll_ctl(epfd, EPOLL_CTL_ADD, ccfd, &ev);
					if(retval == -1)
					{
						cout<<"Error : Adding read watch to C to epoll"<<'\n';
						exit(-1);
					}
					srca.erase(cafd);
					cd.fd = cafd;
					cd.num = atoi(buf);
					mm.insert(make_pair(ccfd, cd));
					srcr.insert(ccfd);
				}
				
					
			}// End for acfd processing
			else
			if(	srcc.find(rev[i].data.fd) != srcc.end())	//Done Connection 	!!
			{		
				ccfd = rev[i].data.fd;
				if(rev[i].events & EPOLLOUT)
				{
					int err = 0;
					socklen_t len = sizeof (int);
					cret = getsockopt (ccfd, SOL_SOCKET, SO_ERROR, &err, &len);	//Check if connect succedd or failed??
					if( (cret != -1) && (err == 0))	//Conn estd
					{
						cd = mm.at(ccfd);
						sprintf(buf,"%d",cd.num);

						n = write(ccfd, buf, strlen(buf));
						if(n < 0){
							cout<<"Error : In srcc Send to C Write Error"<<'\n';
							exit(-1);
						}
						cout<<"In Srcc Sent to C, now wait"<<'\n';

						ev.data.fd = ccfd;
						ev.events = EPOLLIN | EPOLLET;
						
						retval = epoll_ctl(epfd, EPOLL_CTL_MOD, ccfd, &ev);
						if(retval == -1)
						{
							cout<<"Error : epoll modify conn to read error"<<'\n';
							exit(-1);
						}
						srcc.erase(ccfd);
						srcr.insert(ccfd);		
					}
					else
					{
						cout<<"Error: Connect during epoll failed"<<'\n';	//HANDLE HERE
					}

				}
				else //else Not EPOLLOUT ???
				{
					cout<<"Not Epoll out in Connect listen ??"<<'\n';
				}
			}
			else
			if(srcr.find(rev[i].data.fd) != srcr.end())		//Done Read from C 	!!
			{
				ccfd = rev[i].data.fd;

				n = read(ccfd, buf, 99);
				if( n < 0) {
					cout<<"Error: C Read Error"<<'\n';
					exit(-1);
				}
				
				close(ccfd);		        
				//Done C Processing
		        cd = mm.at(ccfd);
		        cafd = cd.fd;
		        data = atoi(buf);
		        
		        data += 1;
		        sprintf(buf, "%d", data);
		        n = write(cafd, buf, strlen(buf));

		        if ( n < 0) {
		           cout<<"Error : Write Back to A Error "<<'\n';
		           exit(-1);
		        }

		        close(cafd);
		        mm.erase(ccfd);
		        srcr.erase(ccfd);
			}
			else
			{
				cout<<"Dont Know Which Event : "<<rev[i].data.fd<<'\n';
			}
		}//End Num REvents Traversal

   	}//End EPOLL WAIT

}	//End Function




int main()
{
	run();
}