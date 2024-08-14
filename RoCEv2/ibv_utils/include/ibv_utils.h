#ifndef __IBV_UTILS_H__
#define __IBV_UTILS_H__

// define a structure for packet information
struct ibv_pkt_info {
    uint8_t src_mac[6];
    uint8_t dst_mac[6];
    uint32_t src_ip;
    uint32_t dst_ip;
    uint16_t src_port;
    uint16_t dst_port;
};

struct ibv_utils_res {
    struct ibv_device *dev;
    struct ibv_context *context;
    struct ibv_pd *pd;
    struct ibv_cq *cq;
    struct ibv_comp_channel *recv_cc;
    struct ibv_qp *qp;
    struct ibv_mr *mr;
    struct ibv_sge *sge;
    struct ibv_send_wr *send_wr;
    struct ibv_recv_wr *recv_wr;
    struct ibv_wc *wc;
    struct ibv_recv_wr *bad_recv_wr;
    struct ibv_send_wr *bad_send_wr;
    int send_wr_num;
    int recv_wr_num;
    int recv_nsge;
    int send_nsge;
};

// APIs
void ibv_utils_info(const char *msg);
void ibv_utils_error(const char *msg);
void ibv_utils_warn(const char *msg);
int get_ib_devices();
int open_ib_device(uint8_t device_id, struct ibv_utils_res *ib_res);
int open_ib_device_by_name(const char *device_name);
int create_ib_res(struct ibv_utils_res *ib_res, int send_wr_num, int recv_wr_num);
int init_ib_res(struct ibv_utils_res *ib_res);
int register_memory(struct ibv_utils_res *ib_res, void *addr, size_t total_length, size_t chunck_size);
int create_flow(struct ibv_utils_res *ib_res, struct ibv_pkt_info *pkt_info);
int ib_send(struct ibv_utils_res *ib_res);
int ib_recv(struct ibv_utils_res *ib_res);
int destroy_ib_res(struct ibv_utils_res *ib_res);
int close_ib_device(struct ibv_utils_res *ib_res);

#endif