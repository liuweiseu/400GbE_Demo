#include <infiniband/verbs.h>
#include <poll.h> 

#include <stdio.h>
#include <stdlib.h>

#include "ibv_utils.h"

#define WR_N 512
#define POLL_N 16

struct ibv_context *ib_global_context[MAX_DEV_NUM];
struct ibv_pd *ib_global_pd[MAX_DEV_NUM];
struct ibv_cq *ib_global_cq[MAX_DEV_NUM];
struct ibv_qp *ib_global_qp[MAX_DEV_NUM];
struct ibv_mr *ib_global_mr[MAX_DEV_NUM];
struct ibv_device **ib_global_devs;
int num_ib_devices;
int send_global_wr_num[MAX_DEV_NUM];
int recv_global_wr_num[MAX_DEV_NUM];

// This is a gloabl variable to record the device id
// In the ib_send and ib_recv function, the device id will be used
int global_device_id = 0;
struct ibv_qp *global_qp = NULL;
struct ibv_cq *global_cq = NULL;
struct ibv_wc global_wc[WR_N];
struct ibv_sge global_sg_entry[WR_N];
struct ibv_recv_wr global_wr, *global_bad_wr;

int send_completed = WR_N;
int recv_completed = WR_N;

/* 
get the ib device list
*/
int get_ib_devices()
{
    //get device list
    ib_global_devs = ibv_get_device_list(&num_ib_devices);
    if (!ib_global_devs) {
        fprintf(stderr, "Failed to get IB devices list.\n");
        return -1;
    }
    return num_ib_devices;
}

/* 
open ib device by id
*/
int open_ib_device(int device_id)
{
    //open ib device by id
    struct ibv_device *ib_dev;
    ib_dev = ib_global_devs[device_id];
    ib_global_context[device_id] = ibv_open_device(ib_dev);
    // check if the device is opened successfully
    if(!ib_global_context[device_id]) {
        fprintf(stderr, "Failed to open IB device.\n");
        return -1;
    }
    else
        return 0;
}

/* 
open ib device by name
*/
int open_ib_device_by_name(const char *device_name)
{
    // TODO: open ib device by name
}

/* 
create ib resources and so on, including pd, cq, qp 
*/
// for sending purpose, send_wr_num is the number of send work requests
// for receiving purpose, recv_wr_num is the number of receive work requests
int create_ib_res(int device_id, int send_wr_num, int recv_wr_num)
{
    // save the wr number to global variable
    send_global_wr_num[device_id] = send_wr_num;
    recv_global_wr_num[device_id] = recv_wr_num;
    int wr_num = send_wr_num > recv_wr_num ? send_wr_num : recv_wr_num;
    // create pd
    ib_global_pd[device_id] = ibv_alloc_pd(ib_global_context[device_id]);
    if (!ib_global_pd[device_id]) {
        fprintf(stderr, "Failed to allocate PD.\n");
        return -1;
    }
    // create cq
    ib_global_cq[device_id] = ibv_create_cq(ib_global_context[device_id], wr_num, NULL, NULL, 0);
    if(!ib_global_cq[device_id]){
        fprintf(stderr, "Couldn't create CQ.\n");
        return -2;
    }
    // create qp
    // TODO: add options for qp type
    struct ibv_cq *cq = ib_global_cq[device_id];
    struct ibv_qp_init_attr qp_init_attr = {
        .qp_context = NULL,
        .send_cq = cq,
        .recv_cq = cq,
        .cap = {
            .max_send_wr = send_wr_num,
            .max_recv_sge = 1,
            .max_recv_wr = recv_wr_num
        },
        .qp_type = IBV_QPT_RAW_PACKET,
    };
    ib_global_qp[device_id] = ibv_create_qp(ib_global_pd[device_id], &qp_init_attr);
    if(!ib_global_qp[device_id]){
        fprintf(stderr, "Couldn't create QP.\n");
        return -3;
    }
    return 0;
}

/*
modify qp status from INIT to RTR, then RTS
*/
int init_ib_res(int device_id)
{
    // get the qp by device id
    struct ibv_qp *qp = ib_global_qp[device_id];

    int state;
    // Initialize the QP
    struct ibv_qp_attr qp_attr;
    int qp_flags;
    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_flags = IBV_QP_STATE | IBV_QP_PORT;
    qp_attr.qp_state = IBV_QPS_INIT;
    // TODO: port number should be set according to the actual port number
    qp_attr.port_num = 1;
    state = ibv_modify_qp(qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        fprintf(stderr, "Failed to init qp.\n");
        return -1;
    }
    // Move the QP to RTR
    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_flags = IBV_QP_STATE;
    qp_attr.qp_state = IBV_QPS_RTR;
    state = ibv_modify_qp(qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        fprintf(stderr, "Failed to modify qp to RTR.\n");
        return -2;
    }
    // move the QP to RTS
    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_flags = IBV_QP_STATE;
    qp_attr.qp_state = IBV_QPS_RTS;
    state = ibv_modify_qp(qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        fprintf(stderr, "Failed to modify qp to RTS.\n");
        return -3;
    }
    // if the initialization is successful, return 0
    return 0;
}

int register_memory(int device_id, void *addr, size_t total_length, size_t chunck_size)
{
    // get the qp by device id
    struct ibv_pd *pd = ib_global_pd[device_id];

    // register memory
    // TODO: add more access control
    struct ibv_mr *mr = ib_global_mr[device_id];
    mr = ibv_reg_mr(pd, addr, total_length, IBV_ACCESS_LOCAL_WRITE);
    if(!mr){
        fprintf(stderr, "Failed to register memory.\n");
        return -1;
    }
    // create sge
    // TODO: the number of sge should be set according to the user's requirement
    for(int i=0; i< WR_N; i++)
    {
        global_sg_entry[i].addr = (uint64_t)(addr)+i*chunck_size;
        global_sg_entry[i].length = chunck_size ;
        global_sg_entry[i].lkey = mr->lkey;
    }
    return 0;
}

/*
create flow for packet filtering
*/
int create_flow(int device_id, struct pkt_info *pkt_info)
{
    // get the qp by device id
    struct ibv_qp *qp = ib_global_qp[device_id];

    // TODO: add more flexible for the flow control
    // Register steering rule to intercept packet to DEST_MAC and place packet in ring pointed by ->qp
    struct raw_eth_flow_attr {
    struct ibv_flow_attr attr;
    struct ibv_flow_spec_eth spec_eth;
    struct ibv_flow_spec_ipv4 spec_ipv4;
    struct ibv_flow_spec_tcp_udp spec_udp;
    } __attribute__((packed)) flow_attr = {
        .attr = {
            .comp_mask = 0,
            .type = IBV_FLOW_ATTR_NORMAL,
            .size = sizeof(flow_attr),
            .priority = 0,
            .num_of_specs = 3,
            .port = 1,
            .flags = 0,
        },
        .spec_eth = {
            .type = IBV_FLOW_SPEC_ETH,
            .size = sizeof(struct ibv_flow_spec_eth),
            .val = {
                .dst_mac = {0,0,0,0,0,0},
                .src_mac = {0,0,0,0,0,0},
                .ether_type = 0,
                .vlan_tag = 0,
            },
            .mask = {
                .dst_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                .src_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
                .ether_type = 0,
                .vlan_tag = 0,
            }
        },
        .spec_ipv4 = {
            .type = IBV_FLOW_SPEC_IPV4,
            .size = sizeof(struct ibv_flow_spec_ipv4),
            .val = {
                .src_ip = 0,
                .dst_ip = 0,
            },
            .mask = {
                .src_ip = 0xffffffff,
                .dst_ip = 0xffffffff,
            }
        },
        .spec_udp = {
            .type = IBV_FLOW_SPEC_UDP,
            .size = sizeof(struct ibv_flow_spec_tcp_udp),
            .val = {
                .dst_port = 0,
                .src_port = 0,
            },
            .mask = 
            {
                .dst_port = 0xffff,
                .src_port = 0xffff,
            } 
        }
    };

    // copy the packet information to the flow_attr
    memcpy(flow_attr.spec_eth.val.dst_mac, pkt_info->dst_mac, 6);
    memcpy(flow_attr.spec_eth.val.src_mac, pkt_info->src_mac, 6);
    //flow_attr.spec_eth.val.ether_type = 0x0800;
    flow_attr.spec_ipv4.val.dst_ip = pkt_info->dst_ip;
    flow_attr.spec_ipv4.val.src_ip = pkt_info->src_ip;
    flow_attr.spec_udp.val.dst_port = pkt_info->dst_port;
    flow_attr.spec_udp.val.src_port = pkt_info->src_port;

    // create flow
    struct ibv_flow *flow;
    flow = ibv_create_flow(qp, &flow_attr.attr);
    if(!flow){
        fprintf(stderr, "Couldn't attach steering flow.\n");
        return -1;
    }

    return 0;
}

int set_global_res(int device_id)
{
    // set the global device id, qp and cq
    global_device_id = device_id;
    global_qp = ib_global_qp[device_id];
    global_cq = ib_global_cq[device_id];
    for(int i = 0; i < WR_N; i++) global_wc[i].wr_id = i;
}

int ib_send(int device_id)
{
    // TODO: implement the ib_send function
}

int ib_recv(int device_id)
{  
    for(int i = 0; i < recv_completed; i++)
    {
        global_wr.wr_id = global_wc[i].wr_id;
        global_wr.sg_list = &global_sg_entry[i];
        global_wr.num_sge = 1;
        global_wr.next = NULL;
        ibv_post_recv(global_qp, &global_wr, &global_bad_wr);
    }   
    recv_completed = ibv_poll_cq(global_cq, POLL_N, global_wc);
    return recv_completed;
}

int destroy_ib_res(int device_id)
{
    // get the ib res by device id, and then dealloc them
    struct ibv_pd *pd = ib_global_pd[device_id];
    ibv_dealloc_pd(pd);
    struct ibv_cq *cq = ib_global_cq[device_id];
    ibv_destroy_cq(cq);
    struct ibv_mr *mr = ib_global_mr[device_id];
    ibv_dereg_mr(mr);
    struct ibv_qp *qp = ib_global_qp[device_id];
    ibv_destroy_qp(qp);
}

int close_ib_device(int device_id)
{
    struct ibv_context *context = ib_global_context[device_id];
    ibv_close_device(context);
}