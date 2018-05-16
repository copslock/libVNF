#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include "common.h"
#include <vector>
#define MAX_THREADS 5
#define THC 1000
using namespace std;

int duration;

struct thread_data{
   int id;
   int status;
};


void printinput()
{
	printf("Run : ./a.out <num-threads> <duration-seconds>\n");	
}

long long diff(timespec start, timespec end)
{
	timespec temp;
	long long ret;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	ret = (temp.tv_sec * 1000000000) + temp.tv_nsec;
	return ret;
}




void *action(void *arg)
{
		
	int socketfd, portno, n;
	struct sockaddr_in rcvr_addr;
	
	time_t start,end;
	double elapsed;
	int my_duration = duration;
	timespec time1, time2;
	long long lat,t;
	vector<long long> callat;
	struct thread_data *my_data;
	my_data = (struct thread_data *) arg;
	int i = my_data->id;

	char buf[100];
	strcpy(buf,"heelo");		
//		srand(time(NULL));
//		int r;
//		r = rand() % 10000; 
//		r += i;

//		 rcvr = gethostbyname("169.254.9.33");
		portno = 5000;
		rcvr_addr.sin_family = AF_INET;
//		rcvr_addr.sin_addr.s_addr = inet_addr("169.254.9.33");  //3 core b
		rcvr_addr.sin_addr.s_addr = inet_addr("169.254.9.88");  //lb
//		rcvr_addr.sin_addr.s_addr = inet_addr("169.254.9.28"); //1 core b
		rcvr_addr.sin_port = htons(portno);
	//	 socketfd = socket(AF_INET, SOCK_STREAM, 0);
/*                if (socketfd < 0) {
                        cout<<"Error: Opening Socket"<<'\n';
                        exit(-1);
                 }
		  if(connect(socketfd, (struct sockaddr*)&rcvr_addr, sizeof(rcvr_addr)) < 0 ) {
                        cout<<"Error : connecting"<<'\n';
                        exit(-1);
                } //oct18 2 ifs*/
	start = time(NULL);
	t = 0;
	while(1)
	{
		end=time(NULL);
        	elapsed = difftime(end,start);
	//        cout<<"start "<<start<<" end "<<end<<" elapsed "<<elapsed<<" duration "<<duration<<endl;
        	if(elapsed >= my_duration)
                	break;
		t++;
		if(t == 50)
		{
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);	
		}
	//	usleep(2000);
	        socketfd = socket(AF_INET, SOCK_STREAM, 0);
        	if (socketfd < 0) {
                	cout<<"Error: Opening Socket"<<'\n';
	                exit(-1);
       		 }

		/*      bzero((char *) &rcvr_addr, sizeof(rcvr_addr));
		        rcvr_addr.sin_family = AF_INET;
		      	bcopy((char *)rcvr->h_addr, (char *)&rcvr_addr.sin_addr.s_addr, rcvr->h_length);
        		rcvr_addr.sin_port = htons(portno);
		*/
        	if(connect(socketfd, (struct sockaddr*)&rcvr_addr, sizeof(rcvr_addr)) < 0 ) {
                	cout<<"Error : connecting"<<'\n';
	                exit(-1);
	        } 
		//      cout<<"\n\t\t\tconnecting"<<'\n';
	        //strcpy(buf,"heelo");
		//      sprintf(buf, "%d", r);
		//      sleep(1);
        	n = write(socketfd, buf, 5);
	        if(n <= 0){
	                cout<<"Error : Wirite Error"<<'\n';
			close(socketfd);
	                //exit(-1);
	                continue;
	        }

		//      cout<<"Sent: \n";
	        n = read(socketfd, buf, 8);
        	if( n <= 0) {
                	cout<<"Error: Read Error"<<'\n';
			close(socketfd);
	                //exit(-1);
        	        continue;
	        } //oct18
        	cout<<"\tRcvd : \t"<<buf<<'\n';
        //	cout<<"\tRcvd"<<socketfd<<endl;  //sep25
	    	//  sleep(1);
	        //}
		//      }
	        close(socketfd);  //oct18
		if(t == 50)
		{
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
		lat = diff(time1, time2);
		//cout<<lat<<endl;
		callat.push_back(lat);
		t = 0;
		}
	
	}
	//cout<<"T: Exiting"<<i<<'\n';
	/*lat = 0;
	for(t = 0; t < callat.size(); t++)
	{
		lat += callat[t];
	}
	//cout<<"Latency for "<<t<<" : "<<lat<<endl;
	lat = lat/t;
	cout<<"Latency for "<<i<<" :"<<lat<<';'<<endl;*/
	//cout<<"Thread exiting "<<i<<endl;
	lat = 0;
	for(t = 0; t < callat.size(); t++)
	{
		lat += callat[t];
	}
	//cout<<"Latency for "<<t<<" : "<<lat<<endl;
	lat = lat/t;
//	sleep(2);
	cout<<"Latency for "<<i<<" :"<<lat<<';'<<endl;
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	int i,n,numth,rc;
	
	if(argc != 3)
	{
		printinput();
		exit(0);
	}


	numth = atoi(argv[1]);
	duration = atoi(argv[2]);

	pthread_t th[THC];
	struct thread_data td[THC];

	pthread_attr_t attr;
	void *status;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


	
	
	for(i = 0; i < numth;i++)
	{	td[i].id = i;
		//td[i].status = 0;
		rc = pthread_create(&th[i], &attr, action, (void *)&td[i]);
		if(rc)
			{
				cout<<"Error thread"<<endl;
			}
	}
	

	for(i = 0; i < numth; i++)
		{
			//td[i].status = 1;
			rc = pthread_join(th[i], &status);
		
		    if (rc){
		       cout << "Error:unable to join," << rc << endl;
		       exit(-1);
		    }
			//cout << " M: Joined with status " << status << endl;		
   		}

	pthread_exit(NULL);
		
}
