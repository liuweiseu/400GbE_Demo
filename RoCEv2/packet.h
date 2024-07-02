#ifndef _PACKET_H_
#define _PACKET_H_

#define PACKET_SIZE 8192
struct packet{
    unsigned char dst_mac[6];
    unsigned char src_mac[6];
    unsigned char eth_type[2];
    unsigned char ip_hdrs[12];
    unsigned char src_ip[4];
    unsigned char dst_ip[4];
    unsigned char udp_hdr[8];
    unsigned char payload[PACKET_SIZE - 42];
};
#endif
