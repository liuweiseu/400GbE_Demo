#include <infiniband/verbs.h>
#include <poll.h> 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <netinet/in.h>

#include "ibv_utils.h"

#define WR_N 512
#define POLL_N 16

struct ibv_device **ib_global_devs;
int num_ib_devices;

int send_completed = WR_N;
int recv_completed = WR_N;

/*
Print information message
*/
void ibv_utils_info(const char *msg)
{
    fprintf(stdout, "[%s %s] IBV-UTILS INFO: %s \n", __DATE__, __TIME__, msg);
}

/*
Print error message
*/
void ibv_utils_error(const char *msg)
{
    fprintf(stderr, "[%s %s] IBV-UTILS ERROR: %s \n", __DATE__, __TIME__, msg);
}

/*
Print warning message
*/
void ibv_utils_warn(const char *msg)
{
    fprintf(stderr, "[%s %s] IBV-UTILS WARN: %s \n", __DATE__, __TIME__, msg);
}
/* 
get the ib device list
*/
int get_ib_devices()
{
    //get device list
    ib_global_devs = ibv_get_device_list(&num_ib_devices);
    ibv_utils_warn("Getting IB devices list.");
    if (!ib_global_devs) {
        ibv_utils_error("Failed to get IB devices list.\n");
        return -1;
    }
    return num_ib_devices;
}

/* 
open ib device by id
*/
int open_ib_device(uint8_t device_id, struct ibv_utils_res *ib_res)
{
    //open ib device by id
    ib_res->dev = ib_global_devs[device_id];
    struct ibv_context *context = ib_res->context;
    ib_res->context = ibv_open_device(ib_res->dev);
    // check if the device is opened successfully
    if(!ib_res->context) {
        ibv_utils_error("Failed to open IB device.\n");
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
int create_ib_res(struct ibv_utils_res *ib_res, int send_wr_num, int recv_wr_num)
{
    ibv_utils_warn("Creating IB resources.");
        // Initialize the variables in the struct
    ibv_utils_warn("Initializing IB structure.");
    // by default, the variables are set to 0
    // if the user doesn't set the number of sge, set it to 1
    if(ib_res->recv_nsge == 0)
    {
        ib_res->recv_nsge = 1;
    }
    if(ib_res->send_nsge == 0)
    {
        ib_res->send_nsge = 1;
    }
    // save the wr number to global variable
    int wr_num = send_wr_num > recv_wr_num ? send_wr_num : recv_wr_num;
    ib_res->send_wr_num = send_wr_num;
    ib_res->recv_wr_num = recv_wr_num;
    // create pd
    ibv_utils_warn("Creating pd.");
    ib_res->pd = ibv_alloc_pd(ib_res->context);
    if (!ib_res->pd) {
        ibv_utils_error("Failed to allocate PD.\n");
        return -1;
    }
    // create cq
    ibv_utils_warn("Creating cq.");
    //ib_res->recv_cc = ibv_create_comp_channel(ib_res->context);
    ib_res->cq = ibv_create_cq(ib_res->context, wr_num, NULL, NULL, 0);
    if(!ib_res->cq){
        ibv_utils_error("Couldn't create CQ.\n");
        return -2;
    }
    /*
    if(ibv_req_notify_cq(ib_res->cq, 0)) {
        ibv_utils_error("ibv_req_notify_cq");
        return -3;
    } 
    */
    // create qp
    // TODO: add options for qp type
    struct ibv_qp_init_attr qp_init_attr = {
        .qp_context = NULL,
        .send_cq = ib_res->cq,
        .recv_cq = ib_res->cq,
        .cap = {
            .max_send_wr = send_wr_num,
            .max_send_sge = ib_res->send_nsge,
            .max_recv_wr = recv_wr_num,
            .max_recv_sge = ib_res->recv_nsge,
        },
        .qp_type = IBV_QPT_RAW_PACKET,
    };
    ibv_utils_warn("Creating qp.");
    ib_res->qp = ibv_create_qp(ib_res->pd, &qp_init_attr);
    if(!ib_res->qp){
        ibv_utils_error("Couldn't create QP.\n");
        return -4;
    }

    // allocate memory for sge, send_wr, recv_wr, wc
    int max_sge = ib_res->send_nsge > ib_res->recv_nsge ? ib_res->send_nsge : ib_res->recv_nsge;
    ib_res->sge = (struct ibv_sge *)malloc(wr_num * sizeof(struct ibv_sge)*max_sge);
    if(!ib_res->sge){
        ibv_utils_error("Failed to allocate memory for sge.\n");
        return -5;
    }
    if(ib_res->send_wr_num > 0)
    {
        ib_res->send_wr = (struct ibv_send_wr *)malloc(ib_res->send_wr_num * sizeof(struct ibv_send_wr));
        if(!ib_res->send_wr){
            ibv_utils_error("Failed to allocate memory for send_wr.\n");
            return -6;
        }
    }
    if(ib_res->recv_wr_num > 0)
    {
        ib_res->recv_wr = (struct ibv_recv_wr *)malloc(ib_res->recv_wr_num * sizeof(struct ibv_recv_wr));
        if(!ib_res->recv_wr){
            ibv_utils_error("Failed to allocate memory for recv_wr.\n");
            return -7;
        }
    }
    ib_res->wc = (struct ibv_wc *)malloc(wr_num * sizeof(struct ibv_wc));
    if(!ib_res->wc){
        ibv_utils_error("Failed to allocate memory for wc.\n");
        return -8;
    }
    return 0;
}

/*
modify qp status from INIT to RTR, then RTS
*/
int init_ib_res(struct ibv_utils_res *ib_res)
{
    ibv_utils_warn("Initializing IB resources.");
    int state;
    // Initialize the QP
    struct ibv_qp_attr qp_attr;
    int qp_flags;
    ibv_utils_warn("modify QP to INIT.");
    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_flags = IBV_QP_STATE | IBV_QP_PORT;
    qp_attr.qp_state = IBV_QPS_INIT;
    // TODO: port number should be set according to the actual port number
    qp_attr.port_num = 1;
    state = ibv_modify_qp(ib_res->qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        ibv_utils_error("Failed to init qp.\n");
        return -1;
    }
    // Move the QP to RTR
    ibv_utils_warn("modify QP to RTR.");
    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_flags = IBV_QP_STATE;
    qp_attr.qp_state = IBV_QPS_RTR;
    state = ibv_modify_qp(ib_res->qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        ibv_utils_error("Failed to modify qp to RTR.\n");
        return -2;
    }
    // move the QP to RTS
    ibv_utils_warn("modify QP to RTS.");
    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_flags = IBV_QP_STATE;
    qp_attr.qp_state = IBV_QPS_RTS;
    state = ibv_modify_qp(ib_res->qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        ibv_utils_error("Failed to modify qp to RTS.\n");
        return -3;
    }
    // if the initialization is successful, return 0
    return 0;
}

int register_memory(struct ibv_utils_res *ib_res, void *addr, size_t total_length, size_t chunck_size)
{
    ibv_utils_warn("Registering memory.");
    // register memory
    // TODO: add more access control
    ib_res->mr = ibv_reg_mr(ib_res->pd, addr, total_length, IBV_ACCESS_LOCAL_WRITE);
    if(!ib_res->mr){
        ibv_utils_error("Failed to register memory.\n");
        return -1;
    }
    // create sge
    // TODO: the number of sge should be set according to the user's requirement
    // TODO: Do we have to set the send_wr_num and recv_wr_num to be the same??
    int max_sge = ib_res->send_nsge > ib_res->recv_nsge ? ib_res->send_nsge : ib_res->recv_nsge;
    int wr_num = total_length / chunck_size / max_sge;
    if((wr_num != ib_res->send_wr_num) && (wr_num != ib_res->recv_wr_num))
    {
        ibv_utils_error("The number of wr is not equal to the number of sge.\n");
        return -2;
    }
    for(int i=0; i< wr_num * max_sge; i++)
    {
        ib_res->sge[i].addr = (uint64_t)(addr+i*chunck_size);
        ib_res->sge[i].length = chunck_size ;
        ib_res->sge[i].lkey = ib_res->mr->lkey;
    }
    for(int i = 0; i < wr_num; i++)
    {
        ib_res->wc[i].wr_id = i;
    }
    return 0;
}

/*
create flow for packet filtering
*/
int create_flow(struct ibv_utils_res *ib_res, struct ibv_pkt_info *pkt_info)
{
    ibv_utils_warn("Creating flow.");
    struct ibv_qp *qp = ib_res->qp;
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
    flow_attr.spec_udp.val.dst_port = htons(pkt_info->dst_port);
    flow_attr.spec_udp.val.src_port = htons(pkt_info->src_port);

    // create flow
    struct ibv_flow *flow;
    flow = ibv_create_flow(qp, &flow_attr.attr);
    if(!flow){
        ibv_utils_error("Couldn't attach steering flow.\n");
        return -1;
    }

    return 0;
}

int ib_send(struct ibv_utils_res *ibv_res)
{
    int i = 0, state = 0, ns = 0, msc = 0;
    // TODO: implement the ib_send function
    memset(ibv_res->send_wr, 0, sizeof(struct ibv_send_wr));
    for(int i = 0; i < ibv_res->send_wr_num; i++)
    {
        ibv_res->send_wr[i].wr_id = i;
        ibv_res->send_wr[i].sg_list = &ibv_res->sge[i*ibv_res->send_nsge];
        ibv_res->send_wr[i].num_sge = ibv_res->send_nsge;
        if(i == ibv_res->send_wr_num - 1)
            ibv_res->send_wr[i].next = NULL;
        else
            ibv_res->send_wr[i].next = &ibv_res->send_wr[i+1];
        ibv_res->send_wr[i].opcode = IBV_WR_SEND;
        ibv_res->send_wr[i].send_flags |= IBV_SEND_SIGNALED;
    }
    state = ibv_post_send(ibv_res->qp, ibv_res->send_wr, &ibv_res->bad_send_wr);
    if(state < 0)
    {
        ibv_utils_error("Failed to post send.\n");
        return -1;
    }
    while(ns < ibv_res->send_wr_num)
    {
        msc = ibv_poll_cq(ibv_res->cq, POLL_N, ibv_res->wc);
        ns += msc;
    }
    return 0;
}

int ib_recv(struct ibv_utils_res *ibv_res)
{  
    if(recv_completed > 0)
    {
        for(int i = 0; i < recv_completed; i++)
        {
            ibv_res->recv_wr->wr_id = ibv_res->wc[i].wr_id;
            ibv_res->recv_wr->sg_list = &ibv_res->sge[ibv_res->wc[i].wr_id*ibv_res->recv_nsge];
            ibv_res->recv_wr->num_sge = ibv_res->recv_nsge;
            ibv_res->recv_wr->next = NULL;
            ibv_post_recv(ibv_res->qp, ibv_res->recv_wr, &ibv_res->bad_recv_wr);
        }   
    }
    recv_completed = ibv_poll_cq(ibv_res->cq, POLL_N, ibv_res->wc);
    
    return recv_completed;
}

int destroy_ib_res(struct ibv_utils_res *ib_res)
{
    // get the ib res by device id, and then dealloc them
    struct ibv_pd *pd = ib_res->pd;
    ibv_dealloc_pd(pd);
    struct ibv_cq *cq = ib_res->cq;
    ibv_destroy_cq(cq);
    struct ibv_mr *mr = ib_res->mr;
    ibv_dereg_mr(mr);
    struct ibv_qp *qp = ib_res->qp;
    ibv_destroy_qp(qp);
    free(ib_res->sge);
    if(ib_res->send_wr_num > 0)free(ib_res->send_wr);
    if(ib_res->recv_wr_num > 0)free(ib_res->recv_wr);
    free(ib_res->wc);
    return 0;
}

int close_ib_device(struct ibv_utils_res *ib_res)
{
    struct ibv_context *context = ib_res->context;
    ibv_close_device(context);
}