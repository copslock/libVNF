
#-----------------Compiler------------------#

G++ = g++ -std=c++0x -std=c++11 -std=gnu++0x -ggdb -O3 -fpermissive

#-------------------------------------------#
#hss: hss.cpp hss.h packet.h diameter.h gtp.h packet.cpp packet.h s1ap.h utils.h uecontext.h packet.o utils.o sip.o uecontext.o 
#	$(G++) hss.cpp packet.o utils.o s1ap.o gtp.o diameter.o sip.o  uecontext.o -o hss
	
all:  ransim.out scscf

uecontext.o : uecontext.cpp uecontext.h
	$(G++)  -c -o uecontext.o  uecontext.cpp

packet.o:	packet.cpp packet.h utils.h sip.h lib.h
	$(G++) -c -o packet.o packet.cpp

sip.o: sip.cpp sip.h
	$(G++) -c -o sip.o sip.cpp

utils.o: utils.cpp utils.h packet.h packet.cpp
	$(G++) -c utils.cpp -o utils.o 

network.o:	network.cpp network.h packet.h utils.h 
	$(G++) -c -o network.o network.cpp

ran.o:	network.h packet.h ran.cpp ran.h sctp_client.h sync.h telecom.h utils.h
	$(G++) -c -o ran.o ran.cpp

ran_simulator.o:	network.h packet.h ran.h ran_simulator.cpp ran_simulator.h sctp_client.h sync.h telecom.h utils.h
	$(G++) -c -o ran_simulator.o ran_simulator.cpp

sctp_client.o:	network.h packet.h sctp_client.cpp sctp_client.h utils.h
	$(G++) -c -o sctp_client.o sctp_client.cpp

security.o:packet.h security.cpp security.h utils.h
	$(G++) -c -o security.o security.cpp -lcrypto

sync.o:	sync.cpp sync.h utils.h
	$(G++) -c -o sync.o sync.cpp

telecom.o:	telecom.cpp telecom.h utils.h
	$(G++) -c -o telecom.o telecom.cpp

libpacket.o:       libpacket.cpp libpacket.h 
	$(G++) -c -o libpacket.o libpacket.cpp

lib.o:       lib.cpp lib.h libpacket.h
	$(G++) -c -o lib.o lib.cpp

scscf: scscf.cpp scscf.h
	$(G++) scscf.cpp lib.o libpacket.o utils.o sip.o uecontext.o packet.o security.o sync.o telecom.o -lcrypto -pthread -lboost_system -lboost_serialization -o scscf

#-------------------------------------------#
RAN_P = security.o network.o packet.o lib.o libpacket.o ran.o ran_simulator.o sctp_client.o sync.o telecom.o utils.o sip.o uecontext.o 
RAN_R = $(G++) -o ransim.out $(RAN_P) -pthread -lcrypto -lboost_system
#------------------Cleaner------------------#

clean:
	rm -f scscf *~ *.o *.out

#-------------------------------------------#
ransim.out:	$(RAN_P)
	$(RAN_R)

#--------------Special Commands-------------#

# make -k (To keep going on even after encountering errors in making a former target)

#-------------------------------------------#
