#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 1
#define IP "127.0.0.1"
#define PACKETSIZE	64
#define PKT struct paketa
#define SS struct sockaddr
#define MILLION 1000000
#define THOUSAND 1000

PKT{
	struct icmphdr hdr;
	char msg[1];										
};


//regular checksum 
unsigned short checksum(void *b, int len)			
{	unsigned short *buf = b;
	unsigned int sum=0;
	unsigned short result;
	for ( sum = 0; len > 1; len -= 2 )
		sum += *buf++;
	if ( len == 1 )
		sum += *(unsigned char*)buf;
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	result = ~sum;
	return result;
}


void ping(struct sockaddr_in *addr_s)
{	
	struct timeval start, end;	//measuring the time					
	PKT	pkt;											
	struct sockaddr_in address_r;											
	int sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP);	//raw socket with icmp protocol				
	if ( sock < 0 )
	{
		perror("socket");
		return;
	}

    int flag=FALSE;
    while(TRUE){
        memset(&pkt, '\0', sizeof(pkt));				
	    pkt.hdr.type = ICMP_ECHO;		//fill the packeta header 				
	    pkt.hdr.un.echo.id = getpid();							
	    pkt.hdr.checksum = checksum(&pkt, sizeof(pkt));	
	    gettimeofday(&start, NULL);	//for time									
	    if (sendto(sock, &pkt, sizeof(pkt), 0, (SS*)addr_s, sizeof(*addr_s)) <= 0 ){//sender
		    perror("sending packet");
            	    flag=FALSE;
        }

	    else {//if the packet saccessfully sent, then recive it
            printf("successfully sent ping\n");	
            flag=TRUE;
        }									
        int len=sizeof(address_r);	
	    if (recvfrom(sock, &pkt, sizeof(pkt), 0, (SS*)&address_r, &len) > 0 ){
		    gettimeofday(&end, NULL);								
	    }
        else{
            perror("recive");
            exit(1);
        }
        if(flag){//if the pkt sent so we can recive her
            if(!(pkt.hdr.type ==69 && pkt.hdr.code==0))  //check if the messeage correctly recived
            { 
                printf("there is a problem!\nPacket received with ICMP type %d code %d \n", pkt.hdr.type, pkt.hdr.code);               
            } 
            else
            { 
                printf("successfully recived reply\n");  
                break;
            }    
        }
}
        printf("milliseconds: %ld\n", ((end.tv_sec - start.tv_sec)* MILLION +(end.tv_usec - start.tv_usec))/THOUSAND);
        printf("microseconds: %ld\n", (end.tv_sec - start.tv_sec)* MILLION +(end.tv_usec - start.tv_usec));
}



int main()
{
	struct hostent *hostName;		
	struct sockaddr_in addr_s;	//the sender	
	memset(&addr_s, '\0', sizeof(addr_s));										
	hostName = gethostbyname(IP);			
	addr_s.sin_family = hostName->h_addrtype;		
	addr_s.sin_addr.s_addr = *(long*)hostName->h_addr;
	ping(&addr_s);								
	return 0;
}
