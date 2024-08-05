#ifndef __PKT_GEN_H__
#define __PKT_GEN_H__

#define PKT_LEN 8192

struct udp_pkt{
    uint8_t dst_mac[6];
    uint8_t src_mac[6];
    uint8_t eth_type[2];
    uint8_t ip_hdrs[12];
    uint8_t src_ip[4];
    uint8_t dst_ip[4];
    uint8_t udp_hdr[8];
    uint8_t payload[PKT_LEN - 42];
};

void set_dest_mac(struct udp_pkt *pkt, uint8_t *mac);
void set_src_mac(struct udp_pkt *pkt, uint8_t *mac);
void set_eth_type(struct udp_pkt *pkt, uint8_t *eth_type);
void set_ip_hdrs(struct udp_pkt *pkt, uint8_t *ip_hdrs);
void set_src_ip(struct udp_pkt *pkt, uint8_t *ip);
void set_dst_ip(struct udp_pkt *pkt, uint8_t *ip);
void set_udp_src_port(struct udp_pkt *pkt, uint16_t port);
void set_udp_dst_port(struct udp_pkt *pkt, uint16_t port);
void set_payload(struct udp_pkt *pkt, uint8_t *payload, int len);

#endif