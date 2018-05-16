/*
    C socket server example, handles multiple clients using threads
*/
 
#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include <iostream>
#include <csignal>
 using namespace std;
//the thread function
void *connection_handler(void *);
 int new_sock_count=0;
void signalHandler(int signum ) {
//   cout << "Interrupt signal (" << signum << ") received.\n";
cout<<new_sock_count<<endl;
   // cleanup and close up stuff here  
   // terminate program  

   exit(signum);  
}
int main(int argc , char *argv[])
{
	signal(SIGINT, signalHandler); 
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
    string my_ip = "169.254.9.78";    
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
//    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr =  inet_addr(my_ip.c_str());
    server.sin_port = htons(6000);
     
    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }
  //  puts("bind done");
     
    //Listen
    //listen(socket_desc , 3);
    //listen(socket_desc , 4096);
    listen(socket_desc , 10000);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
    //    puts("Connection accepted");
  //	cout<<client_sock<<endl;       
        pthread_t sniffer_thread;
        new_sock = (int *)malloc(1);
        *new_sock = client_sock;
        //new_sock_count++; 
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
	 	cout<<"could not create thread"<<endl;
            perror("could not create thread");
            return 1;
        }
         
        //Now join the thread , so that we dont terminate before the thread
        pthread_join( sniffer_thread , NULL);
      //  puts("Handler assigned");
   // }
     
    if (client_sock < 0)
    {
	cout<<"acept failed"<<endl;
        perror("accept failed");
        return 1;
    }
    } 
    if (client_sock < 0)
    {
        cout<<"acept failed"<<endl;
        perror("accept failed");
        return 1;
    }
    return 0;
}
 
/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size,write_size;
    char *message , client_message[5];
     
    //Send some messages to the clienti
/*    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
  */   
    //Receive a message from client
//	bzero(client_message, 5);  //oct23
    //cout<<"client read start"<<endl;
    read_size = recv(sock , client_message , 5, 0);
	if(read_size==0){
		cout<<"read error"<<endl;
		close(sock);
		free(socket_desc);
		return 0;
	}
    //cout<<"data read "<<client_message<<endl;
//read_size = recv(sock , client_message , 6, 0);
	int a=0;
	for(int i=0;i<200;i++){
		a = a+i;
	}    
        //Send the message back to client
//	cout<<"data read "<<client_message<<endl;
        write_size = write(sock , client_message , 5);
	if(write_size==0){
                cout<<"write error"<<endl;
                close(sock);
                free(socket_desc);
                return 0;
        }
//    cout<<"data written to B"<<endl;
     
    /*if(read_size == 0)
    {
        cout<<"Client disconnected"<<endl;
        //fflush(stdout);
    }
    else if(read_size == -1)
    {
        cout<<"recv failed"<<endl;
    }*/
         
    //Free the socket pointer
    close(sock);
    free(socket_desc);
     
    return 0;
}
