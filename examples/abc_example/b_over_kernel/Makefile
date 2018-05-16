# TODO: Make this Makefile.in pretty

TARGETS = b
CC=g++ -fpermissive -std=c++11 -g -O3 -Wno-write-strings -Wall
CC1= g++ -std=c++11 -g

LIBS +=  -lpthread -lnuma -lrt

#ifeq ($(shell uname -m),x86_64)
#LIBS += -m64
#endif
ifeq ($V,) # no echo
        export MSG=@echo
        export HIDE=@
else
        export MSG=@\#
        export HIDE=
endif



all: b

utils.o: utils.cpp
	$(MSG) "   CC $<"
	$(HIDE) ${CC} -c $< ${CFLAGS} ${INC}

libpacket.o: libpacket.cpp
	$(MSG) "   CC $<"
	$(HIDE) ${CC} -c $< ${CFLAGS} ${INC}

lib.o: lib.cpp
	$(MSG) "   CC $<"
	$(HIDE) ${CC} -c $< ${CFLAGS} ${INC}

b.o: b.cpp
	$(MSG) "   CC $<"
	$(HIDE) ${CC} -c $< ${CFLAGS} ${INC}
#b: 
#	g++ -std=c++11 -o b.o lib.o libpacket.o utils.o ${MTCP_FLD}/lib/libmtcp.a -pthread -lboost_system
B_P = b.o lib.o libpacket.o utils.o
B_R = ${CC} -o b $(B_P) ${LIBS}  ${UTIL_OBJ} -lboost_system

b:	$(B_P)
	$(B_R)

#b: b.o lib.o libpacket.o utils.o ${MTCP_FLD}/lib/libmtcp.a
#	$(MSG) "   CC $<"
#	$(HIDE) ${CC} -o $@ $< ${LIBS} ${UTIL_OBJ} -lboost_system


clean:
	rm -f *~ *.o ${TARGETS} log_*

distclean: clean
	rm -rf Makefile
