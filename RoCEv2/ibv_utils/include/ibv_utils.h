#ifndef __IBV_UTILS_H__
#define __IBV_UTILS_H__

// define the maximum number of ib devices
// a PD, CQ, QP will be created for each device
#define MAX_DEV_NUM 2

// define a structure for packet information
struct pkt_info {
    uint8_t src_mac[6];
    uint8_t dst_mac[6];
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
};

// APIs
int get_ib_devices();
int open_ib_device(int device_id);
int open_ib_device_by_name(const char *device_name);
int create_ib_res(int device_id, int send_wr_num, int recv_wr_num);
int init_ib_res(int device_id);
int register_memory(int device_id, void *addr, size_t total_length, size_t chunck_size);
int create_flow(int device_id, struct pkt_info *pkt_info);
int set_global_res(int device_id);
int ib_send(int device_id);
int ib_recv(int device_id);
int destroy_ib_res(int device_id);
int close_ib_device(int device_id);

#endif