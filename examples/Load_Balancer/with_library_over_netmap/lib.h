#ifndef LIB_H
#define LIB_H

#define _LARGEFILE64_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <iostream>
#define NETMAP_WITH_LIBS
#include <net/netmap_user.h>
#include <net/netmap.h>
#include <sys/poll.h>
#include <net/ethernet.h>
//#include "mtcp_api.h"
//#include "mtcp_epoll.h"
//#include "dpdk_api.h"
//#include "netmap_api.h"
//#include <iostream>
//#include "cpu.h"
//#include "debug.h"
#include <sys/time.h>
#include <sched.h>
#include <map>
#include <unordered_map>
#include <string>
#include <bitset>
#include <boost/pool/simple_segregated_storage.hpp>  //for memory pool
#include <boost/foreach.hpp>  //for memory pool
#include <vector>  //for memory pool
#include <cstddef>  //for memory pool
#include <algorithm>  //for client_vector
//#include "libpacket.h"
//#include "rss.h"
#include <mutex>
//for lb vnf
#define ARP_REQUEST 1
#define ARP_REPLY 2
#define ARP_CACHE_LEN 20

//
#define MAX_EVENTS 2048 //65536 //10000 //2048 //65536 //2048 //1024 //2048 //for single core 65536
#define MAX_THREADS 1
#define BUFSIZE 128 //1024
//for multicore B
#define MSB32                           0x80000000
#define MSB16                           0x8000
#define KEY_CACHE_LEN                   96
#define SEED                            'B' 
#define DS_IP				"169.254.9.18"

typedef void (*fn)( int, int, void*, char*);
using namespace std;
void initRequest(int msize[], int m_tot);
void* allocReqCtxt(int alloc_sockid, int id, int index); //uncomment nov28
void freeReqCtxt(int alloc_sockid, int id, int index);  //uncomemnt nov28
void addReqCtxt(int connID, int id, void* requestObj);
void* getRefPtr(int id, void* pkt_mem_ptr);
void removeRefPtr(int id, void* pkt_mem_ptr);
char* writePktmem(int id);
void setSlotLen(int len);
void* getDSptr(int ds_key);
void removeDSptr(int ds_key);
void SignalHandler(int signum);
void serverThreadFunc(void* arg1);
int createServer(string inter_face, string server_ip, int server_port, string protocol);
void registerCallback(int connID, int id, string event, void callbackFnPtr(int, int, void*, char*));
void startEventLoop();
void startPerCore(void* arg1);
int createClient(int id, string local_ip , string remoteServerIP, int remoteServerPort, string protocol);
int createClientDS(int id, string local_ip , int local_port, string remoteServerIP, int remoteServerPort, string protocol);
void sendData(int connID, int id, char* packetToSend, int size);
void sendDataClient(int connID, int id, string packetToSend, int size);
void sendDataServer(int connID, int id, string packetToSend, int packet_size);
void set_data_local(int id, string table_name, int key, uint64_t value);
uint64_t get_data_local(int id, string table_name, int key);
void setData(int connID, int id, int key, string localRemote, string value);
void getData(int connID, int id, int key, string localRemote, void callbackFnPtr(int, int, void*, char*));
void delData(int connID, int id, int key, string localRemote);

void handle_arp_packet(char* buffer);
void arp_request(const uint32_t *dest_ip);
void arp_reply(struct arp_pkt *arppkt);
void insert_arp_cache(uint32_t ip, struct ether_addr mac);
void prepare_arp_packet(struct arp_pkt *arp_pkt, const uint32_t *src_ip, const uint32_t *dest_ip, struct ether_addr *src_mac, struct ether_addr *dest_mac, uint16_t htype);
#endif //LIB_H
