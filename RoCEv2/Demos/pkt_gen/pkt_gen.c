#include <stdio.h>
#include <stdint.h>

#include "pkt_gen.h"

void set_dest_mac(struct udp_pkt *pkt, uint8_t *mac) {
    for (int i = 0; i < 6; i++) {
        pkt->dst_mac[i] = mac[i];
    }
}

void set_src_mac(struct udp_pkt *pkt, uint8_t *mac) {
    for (int i = 0; i < 6; i++) {
        pkt->src_mac[i] = mac[i];
    }
}

void set_eth_type(struct udp_pkt *pkt, uint8_t *eth_type) {
    for (int i = 0; i < 2; i++) {
        pkt->eth_type[i] = eth_type[i];
    }
}

void set_ip_hdrs(struct udp_pkt *pkt, uint8_t *ip_hdrs) {
    for (int i = 0; i < 12; i++) {
        pkt->ip_hdrs[i] = ip_hdrs[i];
    }
}

void set_src_ip(struct udp_pkt *pkt, uint8_t *ip) {
    for (int i = 0; i < 4; i++) {
        pkt->src_ip[i] = ip[i];
    }
}

void set_dst_ip(struct udp_pkt *pkt, uint8_t *ip) {
    for (int i = 0; i < 4; i++) {
        pkt->dst_ip[i] = ip[i];
    }
}

void set_udp_src_port(struct udp_pkt *pkt, uint16_t port) {
    pkt->udp_hdr[0] = (port >> 8) & 0xFF;
    pkt->udp_hdr[1] = port & 0xFF;
}

void set_udp_dst_port(struct udp_pkt *pkt, uint16_t port) {
    pkt->udp_hdr[2] = (port >> 8) & 0xFF;
    pkt->udp_hdr[3] = port & 0xFF;
}

void set_pkt_len(struct udp_pkt *pkt, uint16_t len) {
    set_ip_hdrs(pkt, (uint8_t *)"\x45\x00\x00\x1f\x54\x00\x00\x00\x40\x11\xaf\xb6");
    pkt->udp_hdr[4] = (len >> 8) & 0xFF;
    pkt->udp_hdr[5] = len & 0xFF;
    pkt->ip_hdrs[2] = (len + 20) >> 8;
    pkt->ip_hdrs[3] = (len + 20) & 0xFF;
}

void set_payload(struct udp_pkt *pkt, uint8_t *payload, int len) {
    for (int i = 0; i < len; i++) {
        pkt->payload[i] = payload[i];
    }
}

