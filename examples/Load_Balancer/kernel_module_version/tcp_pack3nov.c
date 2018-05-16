#define DEBUG
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include<linux/init.h>
#include<linux/hashtable.h>
#include <linux/kobject.h> 
#include <linux/sysfs.h> 
#include <linux/printk.h> 
#include <linux/fs.h> 
#include <linux/string.h>
#include <net/ip.h>

#define PTCP_WATCH_PORT     80  /* HTTP port */
#define MAX_CONFIG_ELEMENTS 16
static struct nf_hook_ops nfho;

struct keyvalue1{
unsigned long key1;
u32 value;

};
struct mystruct {
        struct keyvalue1 keyvalue;
        struct hlist_node my_hash_list;
} ;
//#define MAX_CONFIG_ELEMENTS 16
static struct kobject *example_kobject;
static int configs[MAX_CONFIG_ELEMENTS];  //for keyoff
static int c_count; //for keyoff

static int req_configs[MAX_CONFIG_ELEMENTS];  //for reqoff
static int req_count; //for reqoff
static int reqval_configs[MAX_CONFIG_ELEMENTS];  //for reqval
static int reqval_count; //for reqval
static int reqrange_configs[MAX_CONFIG_ELEMENTS];  //for reqrange
static int reqrange_count; //for reqrange
static u32 backip_configs[MAX_CONFIG_ELEMENTS];  //for backip
static int backip_count; //for backip
static int ip_ind = 0; //for rr in backip_configs
int mod_val_tcp=0;
u32 conv(char [],int);
//userspace start
static ssize_t keyoff_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
       int ctr = 0,i;
       for(i=0; i < c_count; ++i)
         ctr += sprintf(buf + ctr, "%d ", configs[i]);
       return ctr;
}

static ssize_t keyoff_store(struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count)
{
        int i, accval=0, ctr=0;
        printk(KERN_INFO "in str = %s\n",buf);
        for(i=0; i < count && ctr < MAX_CONFIG_ELEMENTS; ++i){
               if(buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0'){
                  configs[ctr++] = accval;
                  accval=0;
               }else if(buf[i] >= '0' && buf[i] <= '9'){
                  accval = accval * 10 + buf[i] - '0';
               }else{
                   printk(KERN_INFO "chr=%d i=%d \n",buf[i],i);
                   return -EINVAL;
               }
        }
        c_count = ctr;
        return count;
}

static struct kobj_attribute keyoff_attribute =__ATTR(keyoff, 0600, keyoff_show, keyoff_store);

static ssize_t reqoff_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
       int ctr = 0,i;
       for(i=0; i < req_count; ++i)
         ctr += sprintf(buf + ctr, "%d ", req_configs[i]);
       return ctr;
}

static ssize_t reqoff_store(struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count)
{
        int i, accval=0, ctr=0;
        printk(KERN_INFO "in str = %s\n",buf);
        for(i=0; i < count && ctr < MAX_CONFIG_ELEMENTS; ++i){
               if(buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0'){
                  req_configs[ctr++] = accval;
                  accval=0;
               }else if(buf[i] >= '0' && buf[i] <= '9'){
                  accval = accval * 10 + buf[i] - '0';
               }else{
                   printk(KERN_INFO "chr=%d i=%d \n",buf[i],i);
                   return -EINVAL;
               }
        }
        req_count = ctr;
        return count;
}

static struct kobj_attribute reqoff_attribute =__ATTR(reqoff, 0600, reqoff_show, reqoff_store);

static ssize_t reqval_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
       int ctr = 0,i;
       for(i=0; i < reqval_count; ++i)
         ctr += sprintf(buf + ctr, "%d ", reqval_configs[i]);
       return ctr;
}

static ssize_t reqval_store(struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count)
{
        int i, accval=0, ctr=0;
        printk(KERN_INFO "in str = %s\n",buf);
        for(i=0; i < count && ctr < MAX_CONFIG_ELEMENTS; ++i){
               if(buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0'){
                  reqval_configs[ctr++] = accval;
                  accval=0;
               }else if(buf[i] >= '0' && buf[i] <= '9'){
                  accval = accval * 10 + buf[i] - '0';
               }else{
                   printk(KERN_INFO "chr=%d i=%d \n",buf[i],i);
                   return -EINVAL;
               }
        }
        reqval_count = ctr;
        return count;
}

static struct kobj_attribute reqval_attribute =__ATTR(reqval, 0600, reqval_show, reqval_store);

static ssize_t backip_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
       int ctr = 0,i;
	
       for(i=0; i < backip_count; i++)
         ctr += sprintf(buf + ctr, "%ld ", backip_configs[i]);
       return ctr;
//	return sprintf(buf, "%ld\n", backip_configs[1]);
}

static ssize_t backip_store(struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count)
{
        int i, ctr=0,j=0;
	u32 ip_conv;
//	*buf="127.0.0.1";
//	ip_conv = conv(buf);
	char buf_temp[MAX_CONFIG_ELEMENTS];
//        printk(KERN_INFO "in str = %d\n",ip_conv);
        for(i=0; i < count && ctr < MAX_CONFIG_ELEMENTS; ++i){
               if(buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0'){
		  ip_conv = 0;
//		  printk(KERN_INFO "j = %d\n",strlen(buf_temp));
//		  j++;
//		  buf_temp[j]='p';
		  
		  buf_temp[j]=' ';
		  //j++;
		  printk(KERN_INFO "j = %d %s\n",j,buf_temp);
		  
		  ip_conv = conv(buf_temp, j);
//		  ip_conv = htonl(buf_temp);
		  j=0;
		  printk(KERN_INFO "in backip %ld\n", ip_conv);
                  backip_configs[ctr++] = ip_conv;
		  
		  //printk(KERN_INFO "in backip_config %ld\n", backip_configs[ctr-1]);
                  //accval=0;
               }
	       else{
		    buf_temp[j] = buf[i];
		    j++;
                   //printk(KERN_INFO "chr=%d i=%d \n",buf[i],i);
                   //return -EINVAL;
               }

		  //else if(buf[i] >= '0' && buf[i] <= '9'){
                  //accval = accval * 10 + buf[i] - '0';}
        }
        backip_count = ctr;
//	for(i=0; i < backip_count; i++)
//	printk(KERN_INFO "backip_entry%ld\n ", backip_configs[ctr-1]);
        return count;
}

static struct kobj_attribute backip_attribute =__ATTR(backip, 0600, backip_show, backip_store);

static ssize_t reqrange_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
       int ctr = 0,i;

       for(i=0; i < reqrange_count; i++)
         ctr += sprintf(buf + ctr, "%ld ", reqrange_configs[i]);
       return ctr;
}

static ssize_t reqrange_store(struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count)
{
	int i, accval=0, ctr=0;
        printk(KERN_INFO "in str = %s\n",buf);
        for(i=0; i < count && ctr < MAX_CONFIG_ELEMENTS; ++i){
               if(buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\0'){
                  reqrange_configs[ctr++] = accval;
                  accval=0;
               }else if(buf[i] >= '0' && buf[i] <= '9'){
                  accval = accval * 10 + buf[i] - '0';
               }else{
                   printk(KERN_INFO "chr=%d i=%d \n",buf[i],i);
                   return -EINVAL;
               }
        }
        reqrange_count = ctr;
        return count;
}

static struct kobj_attribute reqrange_attribute =__ATTR(reqrange, 0600, reqrange_show, reqrange_store);
//userspace end
u32 conv(char ipadr[],int k)
{
    u32 num=0;
    unsigned long accval=0;
//    char tok[15],*ptr,*tok1; 
    int i=0;
    printk(KERN_INFO "ipadr len %d\n",k);
    //k=k+1;
    for(i=0;i<=k;i++){
	if(ipadr[i] == '.' || ipadr[i] == '\0' || ipadr[i]=='\n' || ipadr[i]==' ') {
		printk(KERN_INFO "accval %lu %ld\n",accval, num);
		num=(num << 8) + accval;
		accval=0;
	}
        else if(ipadr[i] >= '0' && ipadr[i] <= '9'){
                  accval = accval * 10 + ipadr[i] - '0';
        }else{
		   printk(KERN_INFO "error");
                   return -EINVAL;
        }
	}
/*	if(i==k){
		printk(KERN_INFO "accval %lu %ld\n",accval, num);
                num=(num << 8) + accval;
                accval=0;
	}*/
	printk(KERN_INFO "num is  %ld\n",num);
/*    while(c1 != '\n' || c1 != '\0')
    {
	tok[i] = c1;
	i++;
	c1 = ipadr[i];
    }
//    tok=strtok(ipadr,".");
    while( tok1 != NULL)
    {
	strsep()
        ret=kstrtol(tok,0,&val);
        num=(num << 8) + val;
  //      tok=strtok(NULL,".");
    }*/
    return(num);
}

//DEFINE_HASHTABLE(map_ip, 32);
//calculate checksum of new packet
static int
in_cksum(u_short *addr, int len)
{
    register int nleft = len;
    register u_short *w = addr;
    register int sum = 0;
    u_short answer = 0;

    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)  {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        *(u_char *)(&answer) = *(u_char *)w ;
        sum += answer;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);         /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return(answer);
}

static unsigned int hook_func_in(void *priv,
                                   struct sk_buff *skb,
                    const struct nf_hook_state *state)
{
/*
static unsigned int hook_func_in(const struct nf_hook_ops *ops,
                                   struct sk_buff *skb,
                                   const struct net_device *in,
                                   const struct net_device *out,
                                   int (*okfn)(struct sk_buff *))
{*/
    struct iphdr *iph;          /* IPv4 header */
    struct tcphdr *tcph;        /* TCP header */
    struct udphdr *udph;
    u16 sport, dport;
    unsigned int sport1, dport1;           /* Source and destination ports */
    u32 saddr, daddr;           /* Source and destination addresses */
    unsigned char *user_data;   /* TCP data begin pointer */
    unsigned char *tail;        /* TCP data end pointer */
    unsigned char *it;          /* TCP data iterator */
    unsigned char *it_key;   //key iterator

    unsigned char *user_data_udp;   /* UDP data begin pointer */
    unsigned char *tail_udp;        /* UDP data end pointer */
    unsigned char *it_udp;          /* UDP data iterator */
    int mod_val;
  //  unsigned char buff_pack[5];
//    char *pend;
    int i=0,j=0,k=0,curr_ind=0,offind=0,curr_ind_key=0;
    unsigned long val1;
    int count_char=0,count_char_key=0;
    /* Network packet is empty, seems like some problem occurred. Skip it */
    if (!skb)
        return NF_ACCEPT;

    iph = ip_hdr(skb);          /* get IP header */

    /* Skip if it's not TCP packet */
    /*if (iph->protocol != IPPROTO_TCP)
        return NF_ACCEPT;*/
   if (iph->protocol == IPPROTO_TCP){
//	char self_buff[MAX_CONFIG_ELEMENTS],ran_buff[MAX_CONFIG_ELEMENTS];
	unsigned long msg_type=5555;
        unsigned long key = 0;
	int ip_checksum = 0;
	tcph = tcp_hdr(skb); 
	saddr = ntohl(iph->saddr);
	daddr = ntohl(iph->daddr);
	sport = ntohs(tcph->source);
	dport = ntohs(tcph->dest);
        if(dport==5000){
//	printk("print_tcp len: %ld  %ld data:\n", saddr, dport);
        user_data = (unsigned char *)((unsigned char *)tcph + (tcph->doff * 4));
	tail = skb_tail_pointer(skb);
	count_char = 0; //count payload char
	count_char_key = 0;
	j=0;
	k=0;
	//buff_pack[0]='\0';
	if(req_configs[0]!=5555){
	for (it = user_data; it != tail; ++it) {
	        //char c = *(char *)it;
	
        	//if (c == '\0')
	         //   break;

//        	printk("%c", c);
//		printk("\n count_char %d",count_char);
		//find the request type in the packet payload req_config contains offset(byte wise) in packet which would contain msg_type
		for(offind=0;offind<reqval_count;offind++){
		if(count_char==req_configs[offind]){
		//	if( k < 8){
                              msg_type = *it;
			      curr_ind = offind;
			      break;
                  //      }else{
                    //         unsigned long msg_old = msg_type;
                      //       if(msg_type + *it < msg_old)
                        //         msg_type = msg_type >> 1;
                          //   msg_type = msg_type + *it;
                       // } 
                        //k++;
//		printk("msg_type %lu\n",msg_type);
		}
		}
		//verify if this msg_type packet we have to further check
		if(msg_type==reqval_configs[curr_ind]){
		/*for(i=0; i < c_count; ++i)
			printk(KERN_INFO "\nkey char %d", configs[i]);*/
		//converting bit stream to number to be used as key**********************************
		/*
			Loop over the entire bit stream and find the key between the key offset taken as input fron user
		*/
			for(it_key = user_data; it_key != tail; ++it_key){
			if(curr_ind==0)
				curr_ind_key=0;
			else
				curr_ind_key = curr_ind+1;  //check feb2017
			//if(curr_ind_key+1 < c_count && count_char_key>=configs[curr_ind_key] && count_char_key<=configs[curr_ind_key+1]){
			if(curr_ind_key+1 < c_count && count_char_key>configs[curr_ind_key] && count_char_key<=configs[curr_ind_key+1]){
				//buff_pack[j] = *it;
                	        key = key << 8 | *it_key;
                        	while(key > UINT_MAX){
		                             unsigned ovflow = key >> 32;
                		             key &= 0xffffffffUL;
		                             key = key + ovflow;
                	        }      
#if 0
                        if( j < 8){
                              key = key << 8 | *it;
                        }else{
                             unsigned long oldkey = key;
                             if(key + *it < oldkey)
                                 key = key >> 1;
                             key = key + *it;     
                        }
#endif	
				//j++; //feb9
                        
//			printk("%02X",buff_pack[count_char-2]);
			}
			if(count_char_key>configs[curr_ind_key+1]){
				break; //inner user_data loop key extraction done
			}
			count_char_key++;
			}
			break; //outer user_data loop job done?
		}
		
		count_char++;
		//printk("%X\n", *it);
//		printk("%c",buff_pack[count_char]);
	    }   //end of user_data for loop
	   
                WARN_ON(key > UINT_MAX);	
		if(msg_type==reqval_configs[curr_ind]){
					 //Put range count here
					 int it_range;
					//imsi based backend choose (full stateful) 
					 if(reqrange_configs[0]!=5555){
						for(it_range=0;it_range<reqrange_count;it_range++){
							if(it_range%2==0 && (it_range+1)<reqrange_count){
								if(msg_type>=reqrange_configs[it_range] && msg_type<=reqrange_configs[it_range+1]){
									key = key + reqrange_configs[it_range];
									break; //out of reqrange for loop
								}
							}
						}
						//put an if loop to check if break was done or not.. do mod_val only when break done?
						mod_val = key % backip_count;
					 }
					 else{
			     		 	mod_val = key % backip_count;
					 }
                                         iph->daddr = htonl(backip_configs[mod_val]);
                                         iph->check = 0;
					 //check if checksum correct feb2017
                                         ip_checksum = in_cksum ((unsigned short*) iph, 4 * iph->ihl);
                                         iph->check = ip_checksum;	
		}  //if msg_type=1
	}   //end of if for req type=0 check
	else{
		//send all data coming @ 5000 to backend node  method: rr
				 	key = sport;
	                                 //iph->daddr = htonl(current1->keyvalue.value);
					mod_val = key % backip_count;
					 //printk(KERN_INFO "mod_val is %d",mod_val_tcp);
					//printk(KERN_INFO "checksum before in pre%d\n",iph->check);
					//test
					//if(mod_val==backip_count)
					//	mod_val=0;
					//iph->check = 0; //oct19
                                         //ip_checksum = in_cksum ((unsigned short*) iph, 4 * iph->ihl);
                                         //iph->check = ip_checksum;
                                        //ip_send_check (iph);  //oct19
					//printk(KERN_INFO "checksum after in pre test %d\n",iph->check);
					//test end
                                         iph->daddr = htonl(backip_configs[mod_val]);
					 //mod_val_tcp++;
        	                         iph->check = 0;
                	                 //ip_checksum = in_cksum ((unsigned short*) iph, 4 * iph->ihl);
                        	         //iph->check = ip_checksum;
					ip_send_check (iph);

				/*iph->daddr = htonl(backip_configs[0]);
                                iph->check = 0;
                                ip_checksum = in_cksum ((unsigned short*) iph, 4 * iph->ihl);
                                iph->check = ip_checksum;*/
                                //printk(KERN_INFO "checksum after in pre%d\n",iph->check);
	}
				//user_data = (unsigned char *)((unsigned char *)tcph + (tcph->doff * 4));
				/*for (it = user_data; it != tail; ++it) {
					printk("%X", *it);
				}
				printk("%n");*/
		
/*	uint8_t *tmp = user_data+16;
	uint8_t *tmp2 = user_data+24;

	uint8_t v1 = *tmp;
	uint8_t v2 = *tmp2;
	uint16_t v = (((uint16_t)v1)<<8) + (v2);*/
//(uint32_t)*tmp + ;
	
       
//	    printk("\n\n buff_entry:%lu %x %x",v2,*tmp,*(tmp+1));
/*	for (i=0;i<2;i++)
{
	printk("%X",tmp[i]);
}*/
		/*printk("\n buff_pack 0 %X\n",buff_pack[0]);
	    for(i=0;i<5;i++){
	    
                printk("%02X",buff_pack[i]);
   	     }*/

//	    printk("\n third byte %X",buff_pack[4]);
//	    val1 = buff_pack[15];
	    /*for(i=1;i<16;i++){
		    val1 = val1 + (buff_pack[i]<<16);
	    }*/
	  // char buff_pack1[9]={'1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	   //char buff_pack2[8]={'2','4','A','F','F','F','F','F'};
	   //char buff_pack3[8];
	   //for(i=0; i<8; i++)
	    //    buff_pack3[i] = (char)(buff_pack1[i] ^ buff_pack2[i]);
//	   for(i=0;i<8;i++){

//                printk("%02X ",buff_pack3[i]);
  //           }i
	   val1 = 0;
	   //unsigned char byte2[8]={'0','0','E','6','F','3','B','4'};
           //pend=NULL;
//	   val1 = strto1(buff_pack,&pend,16);
	  //ret1 = kstrtoul(buff_pack,16,&val1);
	//ret1 =  kstrtouint(buff_pack,16,&val1);
//	   ret1 = kstrtou32(buff_pack3,16,&val1);
	  // printk("\nvalue is %d %lu size of val1 %d\n", val1, sizeof(val1));

	}  //dport==5000
	/*if(sport==5000){
		//iph->saddr = htonl(self_addr);
				//self_addr = conv(self_buff,15);
				//iph->saddr = htonl(self_addr);
				ran_addr = conv(ran_buff,15);
				iph->daddr = htonl(ran_addr);
                                //printk(KERN_INFO "checksum before %d\n",iph->check);
                                //printk(KERN_INFO "checksum before %d\n",iph->check);
                                iph->check = 0;
                                //printk(KERN_INFO "checksum mid %d\n",iph->check);
                                ip_checksum = in_cksum ((unsigned short*) iph, 4 * iph->ihl);
                                iph->check = ip_checksum;
                                printk(KERN_INFO "checksum after in pre%d\n",iph->check);


	}*/  //uncomment
	
   }
   else if (iph->protocol == 17){
	unsigned long msg_type=5555;
        unsigned long key = 0;
        int ip_checksum = 0;
	udph = udp_hdr(skb);        /* get UDP header */
	saddr = ntohl(iph->saddr);
    	daddr = ntohl(iph->daddr);
    	sport1 = (unsigned int)ntohs(udph->source);
    	dport1 = (unsigned int)ntohs(udph->dest);
//	printk("print_udp len: sport %d  dport %d daadr_u32 %u daar_ip %pI4h data:\n", sport1, dport1, daddr, &daddr);
//	printk("print_udp len: saddr_u32 %u saddr_ip %pI4h data:\n", saddr, &saddr);
	user_data_udp = (unsigned char *)((unsigned char *)udph + 8);
	tail_udp = skb_tail_pointer(skb);
	for (it_udp = user_data_udp; it_udp != tail_udp; ++it_udp) {
        char c = *(char *)it_udp;

        if (c == '\0')
            break;

        //printk("%c", c);
    	}
    	//printk("\n\n");
   }
           /* get TCP header */
    
   // udph = (struct udphdr *)(skb_transport_header(skb));

    /* Convert network endianness to host endiannes */
    
    
    /* Watch only port of interest */
    //if (sport != PTCP_WATCH_PORT)
    //    return NF_ACCEPT;

    /* Calculate pointers for begin and end of TCP packet data */
//    user_data = (unsigned char *)((unsigned char *)tcph + (tcph->doff * 4));
  //  tail = skb_tail_pointer(skb);

    

    /* ----- Print all needed information from received TCP packet ------ */
	
    /* Print UDP packet data (payload) */
    
    

    /* Show only HTTP packets */
   /* if (user_data[0] != 'H' || user_data[1] != 'T' || user_data[2] != 'T' ||
            user_data[3] != 'P') {
        return NF_ACCEPT;
    }*/

    /* Print packet route */
    //pr_debug("print_tcp: %pI4h:%d -> %lu\n", &saddr, sport, &daddr, dport);

    /* Print TCP packet data (payload) */
   // pr_debug("print_tcp: data:\n");
    /*for (it = user_data; it != tail; ++it) {
        char c = *(char *)it;

        if (c == '\0')
            break;

        printk("%c", c);
    }
    printk("\n\n");
*/
    return NF_ACCEPT;
}

static unsigned int hook_func_out(
        const struct nf_hook_ops *ops,
        struct sk_buff *skb,
        const struct net_device *inDevice,
        const struct net_device *outDevice,
        int (*okfn)(struct sk_buff *))
{
 struct iphdr *iph;          /* IPv4 header */
    struct tcphdr *tcph;        /* TCP header */
    struct udphdr *udph;
    u16 sport, dport;
    unsigned int sport1, dport1;           /* Source and destination ports */
    u32 saddr, daddr;           /* Source and destination addresses */
    if (!skb)
        return NF_ACCEPT;

    iph = ip_hdr(skb);          /* get IP header */

    /* Skip if it's not TCP packet */
    /*if (iph->protocol != IPPROTO_TCP)
        return NF_ACCEPT;*/
   if (iph->protocol == IPPROTO_TCP){
	 char self_buff[16] = "10.129.26.101";
        char ran_buff[16] = "192.168.122.117";
        char dummy_buff[16] = "192.168.122.120";
        u32 self_addr,ran_addr,dummy_addr;
        unsigned long key = 0;
        unsigned long msg_type=5555;
        int ip_checksum = 0;
        tcph = tcp_hdr(skb);
        saddr = ntohl(iph->saddr);
        daddr = ntohl(iph->daddr);
        sport = ntohs(tcph->source);
        dport = ntohs(tcph->dest);
	//printk(KERN_INFO "dport is %d %d %ld %ld\n",dport,sport,saddr,daddr);
        if(dport==5000){
		                  //            ran_addr = conv(ran_buff,15);
                                dummy_addr = conv(dummy_buff,15);
                                self_addr = conv(self_buff,15);
                                //iph->saddr = htonl(self_addr);
                                //iph->daddr = htonl(backip_configs[0]);
                                //printk(KERN_INFO "checksum before  in post %d %ld\n",iph->check,self_addr);
                                //iph->check = 0;
                                //printk(KERN_INFO "checksum mid %d\n",iph->check);
                                //ip_checksum = in_cksum ((unsigned short*) iph, 4 * iph->ihl);
                                //iph->check = ip_checksum;
                                //printk(KERN_INFO "checksum after in post %d\n",iph->check);
                                //user_data = (unsigned char *)((unsigned char *)tcph + (tcph->doff * 4));
	}
	 if(sport==5000){
                //iph->saddr = htonl(self_addr);
                                self_addr = conv(self_buff,15);
                                iph->saddr = htonl(self_addr);
                                //printk(KERN_INFO "checksum before %d\n",iph->check);
                                //printk(KERN_INFO "checksum before in post %d\n",iph->check);
                                iph->check = 0;
                                //printk(KERN_INFO "checksum mid %d\n",iph->check);
                                ip_checksum = in_cksum ((unsigned short*) iph, 4 * iph->ihl);
                                iph->check = ip_checksum;
                                //printk(KERN_INFO "checksum after in post%d\n",iph->check);


        }


   }

 return NF_ACCEPT;
}
//static struct kobj_attribute foo_attribute =__ATTR(foo, 0660, foo_show, foo_store);
static int __init ptcp_init(void)
{
    int res,error=0;

    nfho.hook = hook_func_in;    /* hook function */
    nfho.hooknum = NF_INET_PRE_ROUTING;         /* received packets */
    nfho.pf = PF_INET;                          /* IPv4 */
    nfho.priority = NF_IP_PRI_FIRST;            /* max hook priority */
    
    res = nf_register_hook(&nfho);
    if (res < 0) {
        pr_err("print_tcp: error in nf_register_hook()\n");
        return res;
    }
   

    /*res = nf_register_hook(&nfho1);
    if (res < 0) {
        pr_err("print_tcp: error in nf_register_hook()\n");
        return res;
    }*/

    //userspace read part
    

        printk(KERN_INFO "Module initialized successfully \n");

        example_kobject = kobject_create_and_add("kobject_example",
                                                 kernel_kobj);
        if(!example_kobject)
                return -ENOMEM;

        error = sysfs_create_file(example_kobject, &keyoff_attribute.attr);
        if (error) {
		printk(KERN_INFO "failed to create the foo file in /sys/kernel/kobject_example \n");
        return error;
        }
	error = sysfs_create_file(example_kobject, &reqoff_attribute.attr);
        if (error) {
                printk(KERN_INFO "failed to create the reqoff file in /sys/kernel/kobject_example \n");
        return error;
        }
        error = sysfs_create_file(example_kobject, &backip_attribute.attr);
        if (error) {
                printk(KERN_INFO "failed to create the backip file in /sys/kernel/kobject_example \n");
        return error;
        }
	error = sysfs_create_file(example_kobject, &reqval_attribute.attr);
        if (error) {
                printk(KERN_INFO "failed to create the backip file in /sys/kernel/kobject_example \n");
        return error;
        }
	error = sysfs_create_file(example_kobject, &reqrange_attribute.attr);
        if (error) {
                printk(KERN_INFO "failed to create the backip file in /sys/kernel/kobject_example \n");
        return error;
        }
    pr_debug("print_tcp: loaded\n");
    return 0;
}

static void __exit ptcp_exit(void)
{
    nf_unregister_hook(&nfho);
    //nf_unregister_hook(&nfho1);
    kobject_put(example_kobject);
   //my_hash_list *h_ptr;
 
    pr_debug("print_tcp: unloaded\n");
}

module_init(ptcp_init);
module_exit(ptcp_exit);

MODULE_AUTHOR("Sam Protsenko");
MODULE_DESCRIPTION("Module for printing TCP packet data");
MODULE_LICENSE("GPL");

