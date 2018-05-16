This is the mTCP version of MME component of EPC ported to library.
Refer to https://github.com/networkedsystemsIITB/NFV_LTE_EPC/NFV_LTE_EPC-3.0 for details and setup of other components.
Do the library setup as listed in the user_manual.pdf.

Setup:
File defport.h contains default ports and ip addresses. Change ips here.
Setup each component on separate VMs. For the MME component change the number of cores = VM cores in lib.h and server.conf. Also, change the interface in server.conf.
make clean
make
run sudo ./mme
