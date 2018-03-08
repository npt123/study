#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pcap/pcap.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/ipv6.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/if_ether.h>
#include <netpacket/packet.h>

/*
 *	Ethernet Protocol ID
 */

#define ETH_P_LOOP	        0x0060		// Ethernet Loopback packet
#define ETH_P_PUP	        0x0200		// Xerox PUP packet
#define ETH_P_PUPAT	        0x0201		// Xerox PUP Addr Trans packet
#define ETH_P_IP	        0x0800		// Internet Protocol packet
#define ETH_P_X25	        0x0805		// CCITT X.25
#define ETH_P_ARP	        0x0806		// Address Resolution packet
#define	ETH_P_BPQ	        0x08FF		// G8BPQ AX.25 Ethernet Packet	[ NOT AN OFFICIALLY REGISTERED ID ]
#define ETH_P_IEEEPUP	    0x0a00		// Xerox IEEE802.3 PUP packet
#define ETH_P_IEEEPUPAT	    0x0a01		// Xerox IEEE802.3 PUP Addr Trans packet
#define ETH_P_DEC           0x6000      // DEC Assigned proto
#define ETH_P_DNA_DL        0x6001      // DEC DNA Dump/Load
#define ETH_P_DNA_RC        0x6002      // DEC DNA Remote Console
#define ETH_P_DNA_RT        0x6003      // DEC DNA Routing
#define ETH_P_LAT           0x6004      // DEC LAT
#define ETH_P_DIAG          0x6005      // DEC Diagnostics
#define ETH_P_CUST          0x6006      // DEC Customer use
#define ETH_P_SCA           0x6007      // DEC Systems Comms Arch
#define ETH_P_TEB	        0x6558		// Trans Ether Bridging
#define ETH_P_RARP          0x8035		// Reverse Addr Res packet
#define ETH_P_ATALK	        0x809B		// Appletalk DDP
#define ETH_P_AARP	        0x80F3		// Appletalk AARP
#define ETH_P_8021Q	        0x8100      // 802.1Q VLAN Extended Header
#define ETH_P_IPX	        0x8137		// IPX over DIX
#define ETH_P_IPV6	        0x86DD		// IPv6 over bluebook
#define ETH_P_PAUSE	        0x8808		// IEEE Pause frames. See 802.3 31B
#define ETH_P_SLOW	        0x8809		// Slow Protocol. See 802.3ad 43B
#define ETH_P_WCCP	        0x883E		// Web-cache coordination protocol defined in draft-wilson-wrec-wccp-v2-00.txt
#define ETH_P_PPP_DISC	    0x8863		// PPPoE discovery messages
#define ETH_P_PPP_SES	    0x8864		// PPPoE session messages
#define ETH_P_MPLS_UC	    0x8847		// MPLS Unicast traffic
#define ETH_P_MPLS_MC	    0x8848		// MPLS Multicast traffic
#define ETH_P_ATMMPOA	    0x884c		// MultiProtocol Over ATM
#define ETH_P_ATMFATE	    0x8884		// Frame-based ATM Transport over Ethernet
#define ETH_P_PAE	        0x888E		// Port Access Entity (IEEE 802.1X)
#define ETH_P_AOE	        0x88A2		// ATA over Ethernet
#define ETH_P_TIPC	        0x88CA		// TIPC
#define ETH_P_1588	        0x88F7		// IEEE 1588 Timesync
#define ETH_P_FCOE	        0x8906		// Fibre Channel over Ethernet
#define ETH_P_FIP 	        0x8914		// FCoE Initialization Protocol
#define ETH_P_EDSA    	    0xDADA		// Ethertype DSA [ NOT AN OFFICIALLY REGISTERED ID ]

/*
 *	Non DIX types. Won't clash for 1500 types.
 */

#define ETH_P_802_3	        0x0001		// Dummy type for 802.3 frames
#define ETH_P_AX25	        0x0002		// Dummy protocol id for AX.25
#define ETH_P_ALL	        0x0003		// Every packet
#define ETH_P_802_2	        0x0004		// 802.2 frames
#define ETH_P_SNAP	        0x0005		// Internal only
#define ETH_P_DDCMP         0x0006      // DEC DDCMP: Internal only
#define ETH_P_WAN_PPP       0x0007      // Dummy type for WAN PPP frames
#define ETH_P_PPP_MP        0x0008      // Dummy type for PPP MP frames
#define ETH_P_LOCALTALK     0x0009		// Localtalk pseudo type
#define ETH_P_CAN	        0x000C		// Controller Area Network
#define ETH_P_PPPTALK	    0x0010		// Dummy type for Atalk over PPP
#define ETH_P_TR_802_2	    0x0011		// 802.2 frames
#define ETH_P_MOBITEX	    0x0015		// Mobitex
#define ETH_P_CONTROL	    0x0016		// Card specific control frames
#define ETH_P_IRDA	        0x0017		// Linux-IrDA
#define ETH_P_ECONET	    0x0018		// Acorn Econet
#define ETH_P_HDLC	        0x0019		// HDLC frames
#define ETH_P_ARCNET	    0x001A		// 1A for ArcNet
#define ETH_P_DSA	        0x001B		// Distributed Switch Arch
#define ETH_P_TRAILER	    0x001C		// Trailer switch tagging
#define ETH_P_PHONET	    0x00F5		// Nokia Phonet frames
#define ETH_P_IEEE802154    0x00F6		// IEEE802.15.4 frame

void print_status();
void process_packet(u_char *, const struct pcap_pkthdr *, const u_char *);
void print_ethernet_header(const u_char *, int);
void print_arp_packet(const u_char *, int);
void print_ip_header(const u_char *, int);
void print_icmp_packet(const u_char *, int);
void print_tcp_packet(const u_char *, int);
void print_udp_packet(const u_char *, int);
void print_ipv6_header(const u_char *, int);
void print_ospf_header(const u_char *, int);
void print_data(const u_char *, int);
void print_http(const u_char *, int);

FILE *logfile;
struct sockaddr_in socksrc, sockdest;
struct sockaddr_in6 socksrc6, sockdest6;
char source6[INET6_ADDRSTRLEN], dest6[INET6_ADDRSTRLEN];
int arp=0, icmp=0, igmp=0, ip=0, tcp=0, egp=0, igp=0, udp=0, ipv6=0, esp=0, ospf=0, ip_others=0, others=0, total=0, i, j;

int main() {
    pcap_if_t *alldevsp, *dev;
    pcap_t *handle;

    char errbuf[100], *devname, devs[100][100];
    int count = 1, n;

    printf ("Finding available devices ... ");
    if (pcap_findalldevs(&alldevsp, errbuf)) {
        printf("Error finding devices : %s", errbuf);
        exit(EXIT_FAILURE);
    }
    printf("Done\n");

    printf("Available devices:\n");
    for (dev = alldevsp; dev != NULL; dev = dev->next) {
        printf ("%d. %s - %s\n", count, dev->name, dev->description);
        if (dev->name != NULL) {
            strcpy(devs[count], dev->name);
        }
        count++;
    }

    printf("Select the device. Please enter the number of the device you choose: ");
    scanf("%d", &n);
    devname = devs[n];

    printf("Select the mode:\n");
    printf("[1] Packet Captor\n");
    printf("[2] Sniffer\n");
    scanf("%d", &n);
    if (n == 2) {
        struct sockaddr_ll sockll;
        sockll.sll_family = AF_PACKET;
        sockll.sll_protocol = ETH_P_IP;
    }
    

    printf("Opening device %s ...", devname);
    handle = pcap_open_live(devname, 65536, 1, 0, errbuf);

    if (handle == NULL) {
        fprintf(stderr, "Couldn't open device %s: %s\n", devname, errbuf);
        exit(EXIT_FAILURE);
    }
    printf("Done\n");

    logfile = fopen("log.txt", "w");
    if (logfile == NULL) {
        printf("Couldn't create file.\n");
        exit(EXIT_FAILURE);
    }

    pcap_loop(handle, -1, process_packet, NULL);

    return 0;
}

void print_status() {
    printf("ARP: %d  ICMP: %d  IGMP: %d  IP: %d  TCP: %d  EGP: %d  IGP: %d  UDP: %d  IPv6: %d  ESP: %d  OSPF: %d  IPOthers: %d  Others: %d  Total: %d\r",
            arp, icmp, igmp, ip, tcp, egp, igp, udp, ipv6, esp, ospf, ip_others, others, total);
}

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *Buffer) {
    int Size = header->len;

    for (int i = 0; i < Size; i++) {
        fprintf(logfile, "%02X", (unsigned int)Buffer[i]);
    }
    fprintf(logfile, "\n");

    struct ethhdr *eth = (struct ethhdr *)Buffer;
    ++total;

    if (ntohs(eth->h_proto) == ETH_P_ARP || ntohs(eth->h_proto) == ETH_P_RARP) {
        ++arp;
        print_arp_packet(Buffer, Size);
    } else if (ntohs(eth->h_proto) == ETH_P_IP) {
        struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));

        switch (iph->protocol) {
            case 1:
                ++icmp;
                print_icmp_packet(Buffer, Size);
                break;

            case 2:
                ++igmp;
                break;

            case 4:
                ++ip;
                break;

            case 6:
                ++tcp;
                print_tcp_packet(Buffer, Size);
                break;

            case 8:
                ++egp;
                break;

            case 9:
                ++igp;
                break;

            case 17:
                ++udp;
                print_udp_packet(Buffer, Size);
                break;http://colorlife.iteye.com/blog/689990

            case 41:
                ++ipv6;
                print_ipv6_header(Buffer, Size);
                break;

            case 50:
                ++esp;
                break;

            case 89:
                ++ospf;
                print_ospf_header(Buffer, Size);
                break;

            default:
                ++ip_others;
                break;
        }
    } else {
        ++others;
    }
    print_status();
}

void print_ethernet_header(const u_char *Buffer, int Size) {
    struct ethhdr *eth = (struct ethhdr *)Buffer;

    fprintf(logfile, "\nEthernet Header\n");
    fprintf(logfile, "  |-Protocol           : %u\n", (unsigned short)eth->h_proto);
    fprintf(logfile, "  |-Destination Address: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
            eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);
    fprintf(logfile, "  |-Source Address     : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
            eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);
}

void print_arp_packet(const u_char *Buffer, int Size) {
    struct ether_arp *arp = (struct ether_arp *)(Buffer + sizeof(struct ethhdr));

    fprintf(logfile, "\n\n-------------------- ARP Packet --------------------\n");

    print_ethernet_header(Buffer, Size);

    fprintf(logfile, "ARP Header\n");
    fprintf(logfile, "  |-Hardware Type(HTYPE):     %u\n", ntohs(arp->arp_hrd));
    fprintf(logfile, "  |-Protocol Type(PTYPE):     %X\n", arp->arp_pro);
    fprintf(logfile, "  |-ARP Operation:            %u\n", ntohs(arp->arp_op));
    fprintf(logfile, "ARP Body\n");
    fprintf(logfile, "  |-Source Hardware Address:  %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
            arp->arp_sha[0], arp->arp_sha[1], arp->arp_sha[2], arp->arp_sha[3], arp->arp_sha[4], arp->arp_sha[5]);
    fprintf(logfile, "  |-Source Protocol Address:  %d.%d.%d.%d\n",
            arp->arp_spa[0], arp->arp_spa[1], arp->arp_spa[2], arp->arp_spa[3]);
    fprintf(logfile, "  |-Target Hardware Address:  %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",
            arp->arp_tha[0], arp->arp_tha[1], arp->arp_tha[2], arp->arp_tha[3], arp->arp_tha[4], arp->arp_tha[5]);
    fprintf(logfile, "  |-Target Protocol Address:  %d.%d.%d.%d\n",
            arp->arp_tpa[0], arp->arp_tpa[1], arp->arp_tpa[2], arp->arp_tpa[3]);

    fprintf(logfile, "\n                    DATA Dump                    \n");

    fprintf(logfile, "Ethernet Header\n");
    print_data(Buffer, sizeof(struct ethhdr));
    fprintf(logfile, "ARP Header\n");
    print_data(Buffer+sizeof(struct ethhdr), sizeof(struct arphdr));
    fprintf(logfile, "ARP Body\n");
    print_data(Buffer+sizeof(struct ethhdr)+sizeof(struct arphdr), sizeof(struct ether_arp)-sizeof(struct arphdr));
    fprintf(logfile, "Data Payload\n");
    print_data(Buffer+sizeof(struct ethhdr)+sizeof(struct ether_arp), Size-sizeof(struct ethhdr)-sizeof(struct ether_arp));

    fprintf(logfile, "\n----------------------------------------\n");
}

void print_ip_header(const u_char *Buffer, int Size) {
    print_ethernet_header(Buffer, Size);

    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));

    memset(&socksrc, 0, sizeof(socksrc));
    socksrc.sin_addr.s_addr = iph->saddr;
    memset(&sockdest, 0, sizeof(sockdest));
    sockdest.sin_addr.s_addr = iph->daddr;

    fprintf(logfile, "\nIP Header\n");
    fprintf(logfile, "  |-IP Version            : %d\n", (unsigned int)iph->version);
    fprintf(logfile, "  |-IP Header Length      : %d Bytes\n", ((unsigned int)(iph->ihl)*4));
    fprintf(logfile, "  |-Type of Service       : %d\n", (unsigned int)iph->tos);
    fprintf(logfile, "  |-IP Total Length       : %d Bytes\n", (unsigned int)iph->tot_len);
    fprintf(logfile, "  |-Identification        : %d\n", ntohs(iph->id));
    fprintf(logfile, "  |-Fragment offset field : %d\n", (unsigned int)iph->frag_off);
    fprintf(logfile, "  |-Reserved ZERO Field   : 0\n");
    //fprintf(logfile, "  |-Don't Fragment Field  : %d\n", ip_dont_fragment);
    //fprintf(logfile, "  |-More Fragment Field   : %d\n", ip_more_fragment);
    fprintf(logfile, "  |-Time To Live(TTL)     : %d\n", (unsigned int)iph->ttl);
    fprintf(logfile, "  |-Protocol              : %d\n", (unsigned int)iph->protocol);
    fprintf(logfile, "  |-Checksum              : %d\n", ntohs(iph->check));
    fprintf(logfile, "  |-Source IP             : %s\n", inet_ntoa(socksrc.sin_addr));
    fprintf(logfile, "  |-Destination IP        : %s\n", inet_ntoa(sockdest.sin_addr));
}

void print_icmp_packet(const u_char *Buffer, int Size) {
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl * 4;

    struct icmphdr *icmph = (struct icmphdr *)(Buffer + iphdrlen + sizeof(struct ethhdr));

    int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(icmph);

    fprintf(logfile, "\n\n-------------------- ICMP Packet --------------------\n");

    print_ip_header(Buffer, Size);

    fprintf(logfile, "\nICMP Header\n");
    fprintf(logfile, "  |-Type:     %d", (unsigned int)(icmph->type));

    switch ((unsigned int)icmph->type) {
        case 0:
            fprintf(logfile, " (Echo Reply)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 3:
            fprintf(logfile, " (Destination Unreachable)\n");
            fprintf(logfile, "  |-Code:     %d ", (unsigned int)(icmph->code));
            switch ((unsigned int)(icmph->code)) {
                case 0:
                    fprintf(logfile, "(Destination network unreachable)\n");
                    break;

                case 1:
                    fprintf(logfile, "(Destination host unreachable)\n");
                    break;

                case 2:
                    fprintf(logfile, "(Destination protocol unreachable)\n");
                    break;

                case 3:
                    fprintf(logfile, "(Destination port unreachable)\n");
                    break;

                case 4:
                    fprintf(logfile, "(Fragmentation required, and DF flag set)\n");
                    break;

                case 5:
                    fprintf(logfile, "(Source route failed)\n");
                    break;

                case 6:
                    fprintf(logfile, "(Destination network unknown)\n");
                    break;

                case 7:
                    fprintf(logfile, "(Destination host unknown)\n");
                    break;

                case 8:
                    fprintf(logfile, "(Source host isolated)\n");
                    break;

                case 9:
                    fprintf(logfile, "(Network administratively prohibited)\n");
                    break;

                case 10:
                    fprintf(logfile, "(Host administratively prohibited)\n");
                    break;

                case 11:
                    fprintf(logfile, "(Network unreachable for ToS)\n");
                    break;

                case 12:
                    fprintf(logfile, "(Host unreachable for ToS)\n");
                    break;

                case 13:
                    fprintf(logfile, "(Communication administratively prohibited)\n");
                    break;

                case 14:
                    fprintf(logfile, "(Host Precedence Violation)\n");
                    break;

                case 15:
                    fprintf(logfile, "(Precdence cutoff in effect)\n");
            }
            break;

        case 4:
            fprintf(logfile, " (Source Quench)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 5:
            fprintf(logfile, " (Redirect Message)\n");
            fprintf(logfile, "  |-Code:     %d ", (unsigned int)(icmph->code));
            switch ((unsigned int)(icmph->code)) {
                case 0:
                    fprintf(logfile, "(Redirect Datagram for the Network)\n");
                    break;

                case 1:
                    fprintf(logfile, "(Redirect Datagram for the Host)\n");
                    break;

                case 2:
                    fprintf(logfile, "(Redirect Datagram for the Tos & network)\n");
                    break;

                case 3:
                    fprintf(logfile, "(Redirect Datagram for the Tos & host)\n");
                    break;
            }
            break;

        case 8:
            fprintf(logfile, " (Echo Request)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 9:
            fprintf(logfile, " (Router Advertisement)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 10:
            fprintf(logfile, " (Router Solicitation)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 11:
            fprintf(logfile, " (Time Excceded)\n");
            fprintf(logfile, "  |-Code:     %d ", (unsigned int)(icmph->code));
            switch ((unsigned int)(icmph->code)) {
                case 0:
                    fprintf(logfile, "(TTL expired in transit)\n");
                    break;

                case 1:
                    fprintf(logfile, "(Fragment reassembly time execeeded)\n");
                    break;
            }
           break;

        case 12:
            fprintf(logfile, " (Parameter Problem: Bad IP Header)\n");
            fprintf(logfile, "  |-Code:     %d ", (unsigned int)(icmph->code));
            switch ((unsigned int)(icmph->code)) {
                case 0:
                    fprintf(logfile, "(Pointer indicates the error)\n");
                    break;

                case 1:
                    fprintf(logfile, "(Missing a required option)\n");
                    break;

                case 2:
                    fprintf(logfile, "(Bad length)\n");
                    break;
            }
            break;

        case 13:
            fprintf(logfile, " (Timestamp)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 14:
            fprintf(logfile, " (Timestamp Reply)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 15:
            fprintf(logfile, " (Information Request)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 16:
            fprintf(logfile, " (Information Reply)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 17:
            fprintf(logfile, " (Address Mask Request)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 18:
            fprintf(logfile, " (Address Mask Reply)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        case 30:
            fprintf(logfile, " (Traceroute)\n");
            fprintf(logfile, "  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;

        default:
            fprintf(logfile, "\n  |-Code:     %d\n", (unsigned int)(icmph->code));
            break;
    }

    fprintf(logfile, "  |-Checksum: %d\n", ntohs(icmph->checksum));

    fprintf(logfile, "\n                    DATA Dump                    \n");

    fprintf(logfile, "\nIP Header\n");
    print_data(Buffer, iphdrlen);

    fprintf(logfile, "ICMP Header\n");
    print_data((Buffer + iphdrlen), sizeof(icmph));

    fprintf(logfile, "Data Payload\n");
    print_data((Buffer + header_size), (Size - header_size));

    fprintf(logfile, "\n----------------------------------------\n");
}

void print_tcp_packet(const u_char *Buffer, int Size) {
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;

    struct tcphdr *tcph = (struct tcphdr *)(Buffer + iphdrlen + sizeof(struct ethhdr));

    int header_size = sizeof(struct ethhdr) + iphdrlen + tcph->doff*4;

    fprintf(logfile, "\n\n-------------------- TCP Packet --------------------\n");

    print_ip_header(Buffer, Size);

    fprintf(logfile, "\nTCP Header\n");
    fprintf(logfile, "  |-Source Port               : %u\n", ntohs(tcph->source));
    fprintf(logfile, "  |-Destination Port          : %u\n", ntohs(tcph->dest));
    fprintf(logfile, "  |-Sequence Number           : %u\n", ntohl(tcph->seq));
    fprintf(logfile, "  |-Acknowledge Number        : %u\n", ntohl(tcph->ack_seq));
    fprintf(logfile, "  |-Header Length(Data Offset): %d BYTES\n", (unsigned int)tcph->doff*4);
    fprintf(logfile, "  |-URGent Flag               : %d\n", (unsigned int)tcph->urg);
    fprintf(logfile, "  |-ACKnowledgement Flag      : %d\n", (unsigned int)tcph->ack);
    fprintf(logfile, "  |-PuSH Flag                 : %d\n", (unsigned int)tcph->psh);
    fprintf(logfile, "  |-ReSeT Flag                : %d\n", (unsigned int)tcph->rst);
    fprintf(logfile, "  |-SYNchronization Flag      : %d\n", (unsigned int)tcph->syn);
    fprintf(logfile, "  |-FINis Flag                : %d\n", (unsigned int)tcph->fin);
    fprintf(logfile, "  |-Window                    : %d\n", (unsigned int)tcph->window);
    fprintf(logfile, "  |-Checksum                  : %d\n", (unsigned int)tcph->check);
    fprintf(logfile, "  |-Urgent Pointer            : %d\n", (unsigned int)tcph->urg_ptr);

    fprintf(logfile, "\n                    DATA Dump                    \n");

    fprintf(logfile, "IP Header\n");
    print_data(Buffer, iphdrlen);

    fprintf(logfile, "TCP Header\n");
    print_data((Buffer + iphdrlen), tcph->doff*4);

    fprintf(logfile, "Data Payload\n");
    print_data((Buffer + header_size), (Size - header_size));

    if (((unsigned int)ntohs(tcph->source)) == 80 || ((unsigned int)ntohs(tcph->source) == 443)) {
        fprintf(logfile, "HTTP Packet\n");
        print_http((Buffer + header_size), (Size - header_size));
    }

    fprintf(logfile, "\n----------------------------------------\n");
}

void print_udp_packet(const u_char *Buffer, int Size) {
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;

    struct udphdr *udph = (struct udphdr *)(Buffer + iphdrlen + sizeof(struct ethhdr));

    int header_size = sizeof(struct ethhdr) + iphdrlen + sizeof(udph);

    fprintf(logfile, "\n\n-------------------- UDP Packet --------------------\n");

    print_ip_header(Buffer, Size);

    fprintf(logfile, "\nUDP Header\n");
    fprintf(logfile, "  |-Source Port     : %d\n", ntohs(udph->source));
    fprintf(logfile, "  |-Destination Port: %d\n", ntohs(udph->dest));
    fprintf(logfile, "  |-UDP Length      : %d\n", ntohs(udph->len));
    fprintf(logfile, "  |-UDP Checksum    : %d\n", ntohs(udph->check));

    fprintf(logfile, "\n                    DATA Dump                    \n");

    fprintf(logfile, "\nIP Header\n");
    print_data(Buffer, iphdrlen);

    fprintf(logfile, "UDP Header\n");
    print_data((Buffer + iphdrlen), sizeof(udph));

    fprintf(logfile, "Data Payload\n");
    print_data((Buffer + header_size), (Size - header_size));

    fprintf(logfile, "\n----------------------------------------\n");
}

void print_ipv6_header(const u_char *Buffer, int Size) {
    fprintf(logfile, "\n\n-------------------- IPv6 Packet --------------------\n");

    print_ethernet_header(Buffer, Size);

    struct ipv6hdr *ipv6h = (struct ipv6hdr *)(Buffer + sizeof(struct ethhdr));

    memset(&socksrc6, 0, sizeof(socksrc6));
    memset(&sockdest6, 0, sizeof(sockdest6));

    fprintf(logfile, "\nIPv6 Header\n");
    fprintf(logfile, "  |-Version:              %d\n", ipv6h->version);
    fprintf(logfile, "  |-Traffic Class:        %X\n", ipv6h->priority);
    fprintf(logfile, "  |-Flow Lable:           %X %X %X\n", ipv6h->flow_lbl[0], ipv6h->flow_lbl[1], ipv6h->flow_lbl[2]);
    fprintf(logfile, "  |-Payload Length:       %d\n", ntohs(ipv6h->payload_len));
    fprintf(logfile, "  |-Next Header:          %d\n", ntohs(ipv6h->nexthdr));
    fprintf(logfile, "  |-Hop Limit:            %d\n", ntohs(ipv6h->hop_limit));
    fprintf(logfile, "  |-Source Address:       %s\n", inet_ntop(AF_INET6, &(ipv6h->saddr), source6, INET6_ADDRSTRLEN));
    fprintf(logfile, "  |-Destination Address:  %s\n", inet_ntop(AF_INET6, &(ipv6h->daddr), dest6, INET6_ADDRSTRLEN));

    fprintf(logfile, "\n----------------------------------------\n");
}

void print_ospf_header(const u_char *Buffer, int Size) {
    unsigned short iphdrlen;

    struct iphdr *iph = (struct iphdr *)(Buffer + sizeof(struct ethhdr));
    iphdrlen = iph->ihl*4;

    struct ospfhdr *ospfh = (struct ospfhdr *)(Buffer + iphdrlen + sizeof(struct ethhdr));

    fprintf(logfile, "\n\n-------------------- OSPF Packet --------------------\n");

    print_ip_header(Buffer, Size);

    fprintf(logfile, "\nOSPF Header\n");
    //fprintf(logfile, "  |-Version: %d\n", ospfh);
}

void print_data(const u_char *Data, int Size) {
    int i, j;
    for (i = 0; i < Size; i++) {
        if (i != 0 && i%16 == 0) {
            fprintf(logfile, "         ");
            for (j = i - 16; j < i; j++) {
                if (Data[j] >= 32 && Data[j] <= 126) {
                    fprintf(logfile, "%c", (unsigned char)Data[j]);
                } else {
                    fprintf(logfile, ".");
                }
            }
            fprintf(logfile, "\n");
        }

        if (i%16 == 0) {
            fprintf(logfile, "   ");
        }
        fprintf(logfile, " %02X", (unsigned int)Data[i]);

        if (i == Size - 1) {
            for (j = 0; j < 15-i%16; j++) {
                fprintf(logfile, "   ");
            }

            fprintf(logfile, "         ");

            for (j = i-i%16; j <= i; j++) {
                if (Data[j] >= 32 && Data[j] <= 126) {
                    fprintf(logfile, "%c", (unsigned int)Data[j]);
                } else {
                    fprintf(logfile, ".");
                }
            }

            fprintf(logfile, "\n");
        }
    }
}

void print_http(const u_char *Data, int Size) {
    for (int i = 0; i < Size; i++) {
        if (Data[i] >= 32 && Data[i] <= 126) {
            fprintf(logfile, "%c", (unsigned char)Data[i]);
        } else if (Data[i] == 13 && Data[i+1] == 10) {
            fprintf(logfile, "\n");
            ++i;
        } else {
            fprintf(logfile, ".");
        }
    }
}
