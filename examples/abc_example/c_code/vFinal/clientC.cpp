#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <string.h>

#include "common.h"

using namespace std;

int main()
{

   int clfd, sockfd, n, portno;
   char buf[110], data[100];

   struct sockaddr_in server;
   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if(sockfd < 0) {
      cout<<"ERROR : opening socket";
      exit(-1);
   }

   bzero((char *) &server, sizeof(server) );
   portno = PORTC ;
   cout<<"\nPort Num : "<<portno<<'\n';

   server.sin_family = AF_INET;
   server.sin_addr.s_addr = INADDR_ANY;
   server.sin_port = htons(portno);

   if (bind(sockfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
      cout<<"ERROR: BIND ERROR";
      exit(-1);      
   }

   listen(sockfd, 50);

   while(1){
      clfd = accept(sockfd, NULL, NULL);
      if(clfd < 0) {
         cout<<"Error on accept";
         exit(-1);
      }
//sleep(1);			//remove priya
     cout<<"connect from B done "<<clfd<<endl;

      bzero(buf, 100);
	cout<<"now read data "<<clfd<<endl;
      n = read(clfd, buf, 6);
      if ( n < 0) {
         cout<<"Error : Read Error ";
         exit(-1);
      }

//      data = atoi(buf);
      cout<<"Read : "<<buf<<'\n';
  //    data *= 2;
//	data="hiB";//).c_str();
	string reply="hiB";
//	strcpy(data, reply.c_str());
//      sleep(1);
//      sprintf(buf, "%c", data);
      n = write(clfd, reply.c_str(), 3);

      if ( n < 0) {
         cout<<"Error : Write Error ";
         exit(-1);
      }
	cout<<"data sent to B "<<clfd<<endl;
//    sleep(1);
      close(clfd);

   }
}
