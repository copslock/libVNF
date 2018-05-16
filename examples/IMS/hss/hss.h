int get_scscf(uint64_t imsi,string &scscfaddress,uint64_t &scscfport);
void handle_autninfo_req(Packet &pkt, uint64_t imsi);
void get_autn_info(uint64_t imsi, uint64_t &key, uint64_t &rand_num);
void handleregreq_auth(int conn_id, int id, void* request, void* packet);
void handleRegistrationRequest(int conn_id, int id, void* request, char* packet);
void handlecase3(int conn_id, int id, void* request, char* packet);

