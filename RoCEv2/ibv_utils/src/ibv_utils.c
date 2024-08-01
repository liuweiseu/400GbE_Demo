#include <infiniband/verbs.h>
#include <stdio.h>
#include <stdlib.h>

#include "ibv_utils.h"

#define WR_N 512

struct ibv_context *ib_global_context[MAX_DEV_NUM];
struct ibv_pd *ib_global_pd[MAX_DEV_NUM];
struct ibv_cq *ib_global_cq[MAX_DEV_NUM];
struct ibv_qp *ib_global_qp[MAX_DEV_NUM];
struct ibv_device **ib_global_devs;
int *num_ib_devices;
int send_global_wr_num[MAX_DEV_NUM];
int recv_global_wr_num[MAX_DEV_NUM];
struct ibv_recv_wr global_wr, *global_bad_wr;

/* 
get the ib device list
*/
int get_ib_devices()
{
    //get device list
    ib_global_devs = ibv_get_device_list(num_ib_devices);
    if (!ib_global_devs) {
        fprintf(stderr, "Failed to get IB devices list.\n");
        return -1;
    }
    return *num_ib_devices;
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
    // create pd
    ib_global_pd[device_id] = ibv_alloc_pd(ib_global_context[device_id]);
    if (!ib_global_pd[device_id]) {
        fprintf(stderr, "Failed to allocate PD.\n");
        return -1;
    }
    // create cq
    ib_global_cq[device_id] = ibv_create_cq(ib_global_context[device_id], send_wr_num, NULL, NULL, 0);
    if(!ib_global_cq[device_id]){
        fprintf(stderr, "Couldn't create CQ.\n");
        return -2;
    }
    // create 
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
    struct ibv_mr *mr = ibv_reg_mr(pd, addr, total_length, IBV_ACCESS_LOCAL_WRITE);
    if(!mr){
        fprintf(stderr, "Failed to register memory.\n");
        return -1;
    }
    // create sge
    // TODO: the number of sge should be set according to the user's requirement
    struct ibv_sge sg_entry[WR_N];
        for(int i=0; i< WR_N; i++)
    {
        sg_entry[i].addr = (uint64_t)(addr)+i*chunck_size;
        sg_entry[i].length = chunck_size ;
        sg_entry[i].lkey = mr->lkey;
    }
    return 0;
}



int ib_send()
{

}

int ib_recv()
{

}

int destroy_ib_qps()
{

}

int close_ib_device(struct ibv_context *context)
{

}