#include <errno.h>
#include <resolv.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/wait.h>
#include<pcap.h>
#include<stdio.h>
#include<stdlib.h> 
#include<string.h> 
#include<arpa/inet.h> // for inet_ntoa()
#include<net/ethernet.h> //ethernet header
#include<netinet/ip_icmp.h>	//icmp header
#include<netinet/ip.h>	//ip header


void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    printf("got a new ICMP meassege:\n");
    struct iphdr *ip = (struct iphdr*)(packet + sizeof(struct ethhdr));//ip header
    unsigned short iphdrlen = ip->ihl * 4;
	struct icmphdr *icmph = (struct icmphdr *)(packet + iphdrlen  + sizeof(struct ethhdr));//icmp header
	int header_size =  sizeof(struct ethhdr) + iphdrlen + sizeof(icmph);
    struct sockaddr_in source,dest;//for src and dest of the meassege
    memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = ip->saddr;//for the src ip
	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = ip->daddr;//for the dest ip
    printf("Source IP: %s" ,inet_ntoa(source.sin_addr));
	printf(" Destination IP: %s" ,inet_ntoa(dest.sin_addr));
    printf(" code:%d",(unsigned int)(icmph->code));
    printf(" Type:%d\n",(unsigned int)(icmph->type));
    
    }


int main(){
    pcap_t *handle;//handle the interface
    char errbuf[PCAP_ERRBUF_SIZE];//buffer for errors
    struct bpf_program fp;//
    char filter_exp[]="icmp";
    bpf_u_int32 net;
    handle= pcap_open_live("lo" , 8192,1,1000,errbuf);//(interface , length,promiscuos mode ,time out,errbuf)
    pcap_compile(handle,&fp, filter_exp,0, net);//compile into fp with icmp
    if(pcap_setfilter(handle,&fp)!=0){//set the filter to be icmp
        pcap_perror(handle, "Error:");
        exit(1);
    }
    pcap_loop(handle, -1, got_packet, NULL);//enter to a loop, for every pokcet send for proccssing to got_pocket

    pcap_close(handle);//close handle

    return 0;

}




