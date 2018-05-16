The load-balancer is built over netmap. Install netmap and other setup required for library as provided in the user_manual.pdf.
Installation of mTCP is not required as load-balancer works directly over netmap.

Change the backend IPs in load_balancer.cpp.
compile the code using load_balancer.sh.
run:sudo ./load_balancer

Run cleint and server as required.
