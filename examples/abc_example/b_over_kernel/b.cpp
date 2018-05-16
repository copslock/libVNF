#include "lib.h"

string mme_ip = "169.254.9.33";
int mme_port = 5000;
struct mme_state{
	char* req;
	void* dsreq;
    int val1;
	char b[5];

};
char* temp_data = "hello";
void handle_ds_reply1(int conn_id, int id, void* request, char* packet){
		int a=5,i,key_id;
		string connid = to_string(conn_id);
		mme_state *x = static_cast<mme_state*>(request);
		if(id==1){
                		key_id = 100000+conn_id;
                        
        	}
        else if(id==2){
               			key_id = 200000+conn_id;
        		}
		else if(id==3){
                        key_id = 300000+conn_id;
                }
		else if(id==4){
                        key_id = 400000+conn_id;
                }
		else if(id==5){
                        key_id = 500000+conn_id;
                }
		else if(id==6){
                        key_id = 600000+conn_id;
                }
		else if(id==7){
                        key_id = 700000+conn_id;
                }
                else{
                        key_id = 800000+conn_id;
                }

		x->dsreq = getDSptr(key_id);  
		removeDSptr(key_id);		
		removeRefPtr(id, (void*)x->req);
		char* pkt = writePktmem(id);
		memcpy((void*)pkt,(void*)(x->b),3);
		sendData(conn_id, id, pkt, 3);
		freeReqCtxt(conn_id, id, 1);
		delData(conn_id, id, key_id, "local");
}

void handle_c_reply1(int conn_id, int id, void* request, char* packet){
        int a=5,i,key_id;
        int server_id = get_data_local(id, "",conn_id);
		freeReqCtxt(conn_id, id, 1);
		if(id==1){
            key_id = 100000+server_id;
		}
        else if(id==2){
            key_id = 200000+server_id;
		}
		else if(id==3){
            key_id = 300000+server_id;
        }
		else if(id==4){
            key_id = 400000+server_id;
        }
		else if(id==5){
            key_id = 500000+server_id;
        }
		else if(id==6){
            key_id = 600000+server_id;
        }
		else if(id==7){
            key_id = 700000+server_id;
        }
        else{
            key_id = 800000+server_id;
        }
		getData(server_id, id, key_id, "local", handle_ds_reply1);
		
}

void handle_ue(int conn_id, int id, void* request, char* packet){
		int a=conn_id,i, key_id, c_id;
		request = allocReqCtxt(conn_id, id, 1);
     	c_id = createClient(id, "169.254.9.33", "169.254.9.78", 6000, "tcp");  
		for(i=1;i<20000000;i++){  
                        a = a+i;
        }
		mme_state *x = static_cast<mme_state*>(request); 
		x->val1 = a;
		x->b[0]='a';
		x->b[1]='b';
		x->b[2]='\0';
		x->req = (char*)getRefPtr(id, (void*)packet);
		addReqCtxt(c_id, id, request); 
		registerCallback(c_id, id, "read", handle_c_reply1); 
		set_data_local(id, "",c_id,conn_id); 
		if(id==1){
			key_id = 100000+conn_id;
			
		}
		else if(id==2){
			key_id = 200000+conn_id;
			
		}
		else if(id==3){
			key_id = 300000+conn_id;
		}
		else if(id==4){
			key_id = 400000+conn_id;
		}
		else if(id==5){
            key_id = 500000+conn_id;
        }
		else if(id==6){
        	key_id = 600000+conn_id;
        }
		else if(id==7){
            key_id = 700000+conn_id;
        }
		else{
            key_id = 800000+conn_id;
        }

		setData(conn_id, id, key_id, "local", "abcd"); 
		char* pkt = writePktmem(id);
		memcpy((void*)pkt, (void*)(x->b), 3);
		sendData(c_id, id, pkt, 3);		
}
int main(int argc, char *argv[]) {
	int serverID = createServer("",mme_ip,mme_port, "tcp");
	registerCallback(serverID, -1, "read", handle_ue);
	int reqpool[1] = {sizeof(struct mme_state)};
	initRequest(reqpool, 1);
	startEventLoop();
	return 0;
}
