#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstring>
#include "libpacket.h"
#include <hiredis-vip/hiredis.h>
#include <hiredis-vip/async.h>
#include <hiredis-vip/adapters/libevent.h>
#include <cstdlib> //for atoi
//#include <netinet/tcp.h>  //for nagle

#define MAXCON 8192 //4096     //10000
#define MAXEVENTS 1024 //65536       //10000
#define PORTDS 7001
#define IPDS "169.254.9.18"
int pkt_sent=0;
void SignalHandler(int signum)
{
        //Handle ctrl+C here
        //time(&end_app);
        //sec = difftime(end_app,start_app);
        //for(int i=0;i<MAX_THREADS;i++){
        //      rate+=packets[i];
        //}
//      rate=packets[0]+packets[1]+packets[2];
        //rate = ((rate*8)/(sec*1024*1024*1024));
        //printf("#Application level: rate:%lfGbps, time%lf \n",rate,sec);
        fflush(stdout);
        //signal_handler_dpdk(signum);
        //sleep(5);
        printf("Sent: %d\n",pkt_sent);
//      sleep(25);
}


struct getdata{
	int send_sockid;
	int sockid;
	int buf_key;
	//uint32_t buf_key;
};

int write_stream(int conn_fd, uint8_t *buf, int len) {
	int ptr;
	int retval;
	int written_bytes;
	int remaining_bytes;

	ptr = 0;
	remaining_bytes = len;
	if (conn_fd < 0 || len <= 0) {
		return -1;
	}	
	while (1) {
		written_bytes = write(conn_fd, buf + ptr, remaining_bytes);
		if (written_bytes <= 0) {
			retval = written_bytes;
			break;
		}
		ptr += written_bytes;
		remaining_bytes -= written_bytes;
		if (remaining_bytes == 0) {
			retval = len;
			break;
		}
	}
	return retval;
}

int make_socket_nb(int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      printf("Error: NBS fcntl\n");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      printf("Error: NBS fcntl flags\n");
      return -1;
    }

  return 0;
}
void getCallback1(redisClusterAsyncContext *c, void *r, void *privdata) {
	redisClusterAsyncCommand(c, (redisClusterCallbackFn *) NULL, NULL, "EXEC");
    redisReply *reply = (redisReply *) r;
    int j,ret;
//printf("callback\n");
    if (reply == NULL) return;
/*if (reply->type == REDIS_REPLY_ARRAY) {
        for (j = 0; j < reply->elements; j++) {
            printf("%u) %s\n", j, reply->element[j]->str);
        }
    }*/
// freeReplyObject(reply);
getdata* val1 = (getdata*)privdata;
/*LibPacket pkt2 = (LibPacket&)privdata;	
string rep = reply->str;
pkt2.append_item(rep);
pkt2.prepend_len();*/
//printf("buf_key %d\n",val1->buf_key);
string rep = reply->str; //uncomment dec22 //working feb21

//string rep = "pqrs"; //uncomment dec22
//char* rep = reply->str;
LibPacket pkt1;
pkt1.clear_pkt();
pkt1.append_item(val1->sockid);
pkt1.append_item(val1->buf_key);
pkt1.append_item(rep);
pkt1.prepend_len();
//printf("argv[%s]: %s\n", reply->str, reply->str);
//  redisClusterAsyncDisconnect(c);  
ret = write_stream(val1->send_sockid, pkt1.data, pkt1.len);  //sep25  //uncomment oct25
//ret = write(val1->send_sockid, pkt1.data, pkt1.len);  //sep25
if(ret < 0)
{
	cout<<"Error: Hss data not sent after accept"<<endl;
	exit(-1);
}
pkt_sent++;
//usleep(5);
//printf("buf_value is %d\n",val1->sockid);

//(LibPacket*)privdata.append_item(reply->str);
//				        (LibPacket*)privdata.prepend_len();	
//    printf("argv[%s]: %s\n", privdata, reply->str);
//printf("isue ins reply\n");
    /* Disconnect after receiving the reply to GET */
//	redisClusterAsyncFree(c);
  redisClusterAsyncDisconnect(c);  //sep20 //oct4
//freeReplyObject(reply);
   //return reply;
}
void setCallback1(redisClusterAsyncContext *c, void *r, void *privdata) {
redisClusterAsyncCommand(c, (redisClusterCallbackFn *) NULL, NULL, "EXEC");
//	printf("set done\n");
redisClusterAsyncDisconnect(c);
}

void connectCallback(const redisClusterAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
//  printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
}

int read_stream(int conn_fd, uint8_t *buf, int len) {
	int ptr;
	int retval;
	int read_bytes;
	int remaining_bytes;

	ptr = 0;
	remaining_bytes = len;
	if (conn_fd < 0 || len <= 0) {
		return -1;
	}
	while (1) {
		read_bytes = read(conn_fd, buf + ptr, remaining_bytes);
		if (read_bytes <= 0) {
			retval = read_bytes;
			break;
		}
		ptr += read_bytes;
		remaining_bytes -= read_bytes;
		if (remaining_bytes == 0) {
			retval = len;
			break;
		}
	}
	return retval;
}

int main (int argc, char **argv) {
    signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, SignalHandler);
    struct event_base *base = event_base_new();

    redisClusterAsyncContext *c = redisClusterAsyncConnect("127.0.0.1:8000", HIRCLUSTER_FLAG_NULL);
//    redisAsyncContext *c = redisAsyncConnect("169.254.9.18", 8002);
    if (c->err) {
        /* Let *c leak for now... */
        printf("Error: %s\n", c->errstr);
        return 1;
    }

    redisClusterLibeventAttach(c,base);
    redisClusterAsyncSetConnectCallback(c, (redisConnectCallback *) connectCallback);
//    redisAsyncSetDisconnectCallback(c,disconnectCallback);
    redisClusterAsyncCommand(c, (redisClusterCallbackFn *) setCallback1, NULL, "SET %s %s", "foo", "hello world");
//    redisAsyncCommand(c, NULL, NULL, "SET key %b", argv[argc-1], strlen(argv[argc-1]));
    //redisAsyncCommand(c, getCallback, (char*)"end-1", "GET foo");
printf("callback to be called\n");
//    redisClusterAsyncCommand(c, (redisClusterCallbackFn *) getCallback1, (char*)"foo", "GET foo");  //sep19
redisClusterAsyncCommand(c, (redisClusterCallbackFn *) setCallback1, NULL, "SET %s %s", "hoo", "hello world1");
// redisClusterAsyncCommand(c, (redisClusterCallbackFn *) getCallback1, (char*)"hoo", "GET hoo");   //sep19
//    redisAsyncCommand(c, getCallback, (char*)"foo", "LRANGE foo 0 -1");
    event_base_dispatch(base);
int lsfd, acfd, portno, data, n, numev, i, ccfd, cafd, cret,trf,loop_count=0,redis_ret;
	char buf[110],buf1[110];
	int buf_sockid,buf_key;
//	uint32_t buf_key_long;
	string buf_cmd,buf_table,buf_value,buf_value1,b_key;
	char* buf2;
	redisReply *reply;
	long long transactions = 0;

	int count,tcount;
	
	struct sockaddr_in server, c_addr;
	struct hostent *c_ip;
	

	lsfd = socket(AF_INET, SOCK_STREAM, 0);

	if(lsfd < 0) {
    	printf("ERROR : opening socket\n");
      	exit(-1);
   	}

	bzero((char *) &server, sizeof(server) );
   	portno = PORTDS;
   	//cout<<"\nPort Num : "<<portno<<'\n';
    int port_nos = atoi(argv[1]);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("169.254.9.18");
    server.sin_port = htons(port_nos);

    if (bind(lsfd, (struct sockaddr *) &server, sizeof(server)) < 0) {
      	printf("ERROR: BIND ERROR\n");
      	exit(-1);      
    }

    make_socket_nb(lsfd);

   	listen(lsfd, MAXCON);

   	int epfd = epoll_create(MAXEVENTS + 5);
   	if( epfd == -1){
   		printf("Error: epoll create\n");
   		exit(-1);
   	}

   	int retval;
   	struct epoll_event ev, *rev;//[MAXEVENTS];

   	ev.data.fd = lsfd;
   	ev.events = EPOLLIN | EPOLLET;

   	retval = epoll_ctl( epfd, EPOLL_CTL_ADD, lsfd, &ev);
   	if( retval == -1) {
   		printf("Error: epoll ctl lsfd add\n");
   		exit(-1);
   	}
	rev = (struct epoll_event*)calloc(MAXEVENTS, sizeof(struct epoll_event));   //sep20
   	printf("Entering Loop\n");
   	count = 0;
   	tcount=0;

   	trf = 0;
   	transactions = 0;
   	while( 1 ){
//		printf("inside while loop\n");
		//memset(buf,0,98);
   		numev = epoll_wait( epfd, rev, MAXEVENTS, -1);
   		//numev = epoll_wait( epfd, rev, MAXEVENTS, 100);
		//if(numev==0)
		//	printf("tick!!!\n");
   		if(numev < 0){
			printf("Error: EPOLL wait!\n");
			exit(-1);
		}
	for( i = 0; i < numev; i++){
			trf = 1;
			if(	(rev[i].events & EPOLLERR) ||
				(rev[i].events & EPOLLHUP)	
				) {
				printf("ERROR: epoll monitoring failed, closing fd\n");
				if(rev[i].data.fd == lsfd){
			//		cout<<"Oh Oh, lsfd it is"<<'\n';
					printf("lsfd error\n");
					exit(-1);
				}
				close(rev[i].data.fd);
				continue;
			}

			else if( rev[i].data.fd == lsfd ) { 	//listening socket!!
							
				//Check if events full				? 		LATER LATER LATER
			//	while(1){   //sep18
				acfd = accept(lsfd, NULL, NULL);
					
					if(acfd < 0){
						
						if((errno == EAGAIN) ||	(errno == EWOULDBLOCK))
						{
							//processed all connections !!!
							//break;   //sep18
						}
						else
						printf("Error on accept\n");
						break;
						//exit(-1);
					}

					make_socket_nb(acfd);
					//---diabale nagle
					//int flag = 1;
				        //int result = setsockopt(acfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
				        //--disable nagle
					ev.data.fd = acfd;
					ev.events = EPOLLIN | EPOLLET;
					//ev.events = EPOLLIN;
					retval = epoll_ctl( epfd, EPOLL_CTL_ADD, acfd, &ev);
   					if( retval == -1) {
   						printf("Error: epoll ctl lsfd add\n");
   						exit(-1);
   					}
   					//tcount++;
			//	}  //sep18

					printf("client accepted\n");
   					
			}
			else if (rev[i].events & EPOLLIN) {
					
					//n = read(rev[i].data.fd, buf, sizeof buf);
					while(1){   //sep18
					//bzero(buf, 100);
					//loop_count++;
					//memset(buf,0,98);  
					//n = read(rev[i].data.fd, buf, sizeof buf);
					LibPacket pkt;
                                        int pkt_len,retval;
//					n = read(rev[i].data.fd, buf, sizeof(int)); //sep20

					pkt.clear_pkt();
					retval = read_stream(rev[i].data.fd, pkt.data, sizeof(int));
					if(retval < 0)
						{
							 if (errno == EAGAIN)
	                                                {
        	                                          //sep20
                	                                  //perror ("read");
                        	                        //  printf("loop count is %d", loop_count);
                                	                 // loop_count = 0;
                                        	          break;
                                                	}

						//	TRACE(cout<<"Error: Read pkt len case 3, exit for now"<<endl;)
						//	exit(-1);
						}
					else
						{
							memmove(&pkt_len, pkt.data, sizeof(int) * sizeof(uint8_t));
							pkt.clear_pkt();
							retval = read_stream(rev[i].data.fd, pkt.data, pkt_len);
							pkt.data_ptr = 0;
							pkt.len = retval;
							if(retval < 0)
							{
								TRACE(cout<<"Error: Packet from HSS Corrupt, break"<<endl;)
								//lflag = 1;
								break;
							}
						}	

					pkt.extract_item(buf_sockid);
					pkt.extract_item(buf_cmd);
					pkt.extract_item(buf_table);
					pkt.extract_item(buf_key);
//					pkt.extract_item(buf_value);
					/*LibPacket pkt1, *pkt2;
					pkt1.clear_pkt();
				        pkt1.append_item(buf_sockid);
				        pkt1.append_item(buf_key);*/
			/*		getdata* val1 = new getdata;
					val1->sockid = buf_sockid;
					val1->buf_key = buf_key;
					val1->send_sockid = rev[i].data.fd;*/
				        //pkt1.append_item(value);
					//redisClusterAsyncCommand(c, (redisClusterCallbackFn *) setCallback1, NULL, "SET %s %s", "moo", (buf1.substr(0,2)).c_str());
					if(buf_cmd=="set"){
						pkt.extract_item(buf_value);
						//printf("buf in set 2  %d\n", buf_key);
						//printf("buf in set is %d %d %d %s %s %d %s \n", rev[i].data.fd, buf_sockid, n, buf_cmd.c_str(), buf_table.c_str(), buf_key, buf_value.c_str());
						//redisClusterAsyncCommand(c, (redisClusterCallbackFn *) setCallback1, NULL, "SET %d %s", buf_key, buf_value.c_str());
						//redisClusterAsyncCommand(c, (redisClusterCallbackFn *) setCallback1, NULL, "SET %d %s", buf_key, buf_value.c_str());   //working feb21
						redisClusterAsyncCommand(c, (redisClusterCallbackFn *) NULL, NULL, "MULTI");
						//event_base_dispatch(base);
						redisClusterAsyncCommand(c, (redisClusterCallbackFn *) setCallback1, NULL, "SET %d %s", buf_key, buf_value.c_str());
						//event_base_dispatch(base);
						//redisClusterAsyncCommand(c, (redisClusterCallbackFn *) NULL, NULL, "EXEC");   
						//b_key = to_string(buf_key);
						//redisClusterAsyncCommand(c, (redisClusterCallbackFn *) setCallback1, NULL, "SET %s %s", b_key.c_str(), buf_value.c_str()); //test dec22
						//redisClusterAsyncCommand(c, NULL, NULL, "SET %d %s", buf_key, buf_value.c_str());
						//redisClusterAsyncDisconnect(c);
						event_base_dispatch(base);
					}
					else if(buf_cmd=="get"){
					//	printf("buf in get is %d %d %d %s %s %d \n", rev[i].data.fd, buf_sockid, n, buf_cmd.c_str(), buf_table.c_str(), buf_key);
						//printf("buf in get 1 %d\n", buf_sockid);
						//redisClusterAsyncCommand(c, (redisClusterCallbackFn *) getCallback1, (char*)"moo", "GET moo");
						getdata* val1 = new getdata;
	                                        val1->sockid = buf_sockid;
        	                                val1->buf_key = buf_key;
                	                        val1->send_sockid = rev[i].data.fd;  //working feb22
						//printf("buf in get 2  %d\n", buf_key);
						/*b_key = to_string(buf_key);
                                                char *y = new char[b_key.length() + 1];
                                                std::strcpy(y, b_key.c_str());*/
						//redis_ret = redisClusterAsyncCommand(c, (redisClusterCallbackFn *) getCallback1, (void*)val1, "GET %d", buf_key);  //uncomment this oct24
						redisClusterAsyncCommand(c, (redisClusterCallbackFn *) NULL, NULL, "MULTI");
						redis_ret = redisClusterAsyncCommand(c, (redisClusterCallbackFn *) getCallback1, (void*)val1, "GET %d", buf_key);  //uncomment this oct24 //working feb21
						//redisClusterAsyncCommand(c, (redisClusterCallbackFn *) NULL, NULL, "EXEC");
					//	redis_ret = redisClusterAsyncCommand(c, (redisClusterCallbackFn *) getCallback1, (void*)val1, "MULTI GET %d EXEC", buf_key);  //uncomment this oct24
						/*redis_ret = 2;
						while(redis_ret!=0){
						redis_ret = redisClusterAsyncCommand(c, (redisClusterCallbackFn *) getCallback1, (void*)val1, "GET %d", buf_key);
						if(redis_ret<0){
								redisClusterReset((redisClusterContext*)&c);
                                                                event_base_dispatch(base);
							//	redisClusterAsyncSetConnectCallback(c, (redisConnectCallback *) connectCallback);	
							//	event_base_dispatch(base);
							}
						} */// comment oct24
						//printf("redis msg is %d \n",redis_ret);
							
						 event_base_dispatch(base);
						//printf("buf in get 2  %d\n", buf_sockid);
						//printf("buf in get2 is %d %d %d %s %s %d %s \n", rev[i].data.fd, buf_sockid, n, buf_cmd.c_str(), buf_table.c_str(), buf_key, buf_value1.c_str());
					}
						 //event_base_dispatch(base);
					}    //sep18
					//cout<<"buf_temp is "<<buf_temp<<endl;
					//cout.flush();
					/*retval = epoll_ctl( epfd, EPOLL_CTL_MOD, rev[i].data.fd, &ev);
                                        if( retval == -1) {
                                                printf("Error: epoll ctl lsfd add\n");
                                                exit(-1);
                                        }*/
					//memset(buf,0,98);

			}
	}
	}
    return 0;
}
