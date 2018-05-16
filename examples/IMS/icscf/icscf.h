//int handleRegistrationRequest(int epollfd,int cur_fd, map<int, mdata> &fdmap, mdata &fddata, char * ServerAddress, int port, epoll_event &new_file_descriptor_to_watch);
void handleregreq_auth(int conn_id, int id, void* request, void* packet);
void handleRegistrationRequest(int conn_id, int id, void* request, char* packet);
void handlecase3_get(int conn_id, int id, void* request, void* packet);
void handlecase5_get(int conn_id, int id, void* request, void* packet);
void handlecase3(int conn_id, int id, void* request, char* packet);
void handlecase5(int conn_id, int id, void* request, char* packet);

