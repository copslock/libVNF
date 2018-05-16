all:	rd_ep.out

G++ = g++ -std=c++0x -std=c++11 -std=gnu++0x -ggdb

libpacket.o:	libpacket.cpp libpacket.h utils.h
	$(G++) -c -o libpacket.o libpacket.cpp

redis_ep_server.o: redis_ep_server.cpp libpacket.h 
	$(G++) -c -o redis_ep_server.o redis_ep_server.cpp

utils.o:	utils.cpp utils.h
	$(G++) -c -o utils.o utils.cpp

RD_EP_P = redis_ep_server.o libpacket.o utils.o
RD_EP_R = $(G++) -o rd_ep.out $(RD_EP_P) -levent -lkvs_redis

rd_ep.out:	$(RD_EP_P)
	$(RD_EP_R)

clean:
	rm -f *~ *.o *.out

