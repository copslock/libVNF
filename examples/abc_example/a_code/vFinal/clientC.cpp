#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>

#include <string.h>

#include "common.h"

using namespace std;

int main()
{

   int clfd, sockfd, data, n, portno;
   char buf[110];

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

      bzero(buf, 100);
      n = read(clfd, buf, 100);
      if ( n < 0) {
         cout<<"Error : Read Error ";
         exit(-1);
      }

      data = atoi(buf);
      cout<<"Read : "<<data<<'\n';
      data *= 2;
      sleep(1);
      sprintf(buf, "%d", data);
      n = write(clfd, buf, strlen(buf));

      if ( n < 0) {
         cout<<"Error : Write Error ";
         exit(-1);
      }

      close(clfd);

   }
}