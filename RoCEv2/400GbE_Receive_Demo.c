#include <infiniband/verbs.h>
#include <poll.h> 

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "packet.h"

#define CQE 512
#define WR_N 512

#define SRC_MAC {0xa0, 0x88, 0xc2, 0x0d, 0x5e, 0x28}
#define DST_MAC {0x94, 0x6d, 0xae, 0xac, 0xf8, 0x38}
#define ETH_TYPE {0x08, 0x00}

int main(int argc, char *argv[])
{
    int dev_num = 2;
    int verbose = 0;
    for(int i=1; i < argc;)
    {
        if(!strcmp(argv[i],"-d"))
        {
            i++;
            dev_num = atoi(argv[i]);
        }
        if(!strcmp(argv[i], "-v"))
            verbose = 1;
        i++;
    }
    struct ibv_device **dev_list;
    int ndev;
    // get the device list
    dev_list = ibv_get_device_list(&ndev);
    if (!dev_list) {
        perror("Failed to get devices list");
        exit(1);
    }
    printf("Number of Device: %d\n", ndev);

    struct ibv_device *ib_dev;
    ib_dev = dev_list[dev_num];

    //open device
    struct ibv_context *context;
    context = ibv_open_device(ib_dev);
    printf("Dev opened.\n");

        // create protection domain
    struct ibv_pd *pd;
    pd = ibv_alloc_pd(context);
    if(!pd)
    {
        fprintf(stderr, "Couldn't allocate PD.\n");
        exit(1);
    }

    // create completion queue
    struct ibv_cq *cq;
    cq = ibv_create_cq(context, CQE, NULL, NULL, 0);
    if(!cq){
        fprintf(stderr, "Couldn't create CQ.\n");
        exit(1);
    }
    struct ibv_comp_channel *recv_cc=NULL;
    recv_cc = ibv_create_comp_channel(context);

    // create qp
    struct ibv_qp *qp;
    struct ibv_qp_init_attr qp_init_attr = {
        .qp_context = NULL,
        .send_cq = cq,
        .recv_cq = cq,
        .cap = {
            .max_send_wr = 0,
            .max_send_sge = 1,
            .max_recv_wr = CQE
        },
        .qp_type = IBV_QPT_RAW_PACKET,
    };
    qp = ibv_create_qp(pd, &qp_init_attr);

    // Initialize the QP
    int state;
    struct ibv_qp_attr qp_attr;
    int qp_flags;
    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_flags = IBV_QP_STATE | IBV_QP_PORT;
    qp_attr.qp_state = IBV_QPS_INIT;
    qp_attr.port_num = 1;
    state = ibv_modify_qp(qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        fprintf(stderr, "Failed to init qp.\n");
        exit(1);
    }
    else
        printf("QP init successfully!\n");
    
    // move the ring to receiver
    qp_flags = IBV_QP_STATE;
    qp_attr.qp_state = IBV_QPS_RTR;
    state = ibv_modify_qp(qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        fprintf(stderr, "Failed to modify qp to RTR.\n");
        exit(1);
    }
    else
        printf("QP is ready to receiver data!\n");

    // Allocate Memory
    void *buf;
    int buf_size = PACKET_SIZE * CQE;
    buf = malloc(PACKET_SIZE*CQE);
    // register memory
    struct ibv_mr *mr;
    mr = ibv_reg_mr(pd, buf, buf_size, IBV_ACCESS_LOCAL_WRITE);
    if(!mr){
        fprintf(stderr, "Failed to register mr.\n");
        exit(1);
    }
    else
        printf("Register mr successfully!\n");

    // create sge
    struct ibv_sge sg_entry[WR_N];
    for(int i=0; i< WR_N; i++)
    {
        sg_entry[i].addr = (uint64_t)(buf+i*PACKET_SIZE);
        sg_entry[i].length = PACKET_SIZE;
        sg_entry[i].lkey = mr->lkey;
    }

    // create wr
    struct ibv_recv_wr wr, *bad_wr;
    for(int i=0; i< WR_N; i++)
    {
        memset(&wr, 0, sizeof(wr));
        wr.num_sge = 1;
        /* each descriptor points to max MTU size buffer */
        sg_entry[i].addr = (uint64_t)buf + PACKET_SIZE*i;
        wr.sg_list= &sg_entry[i];
        wr.next = NULL;
        wr.wr_id = i;
        /* index of descriptor returned when packet arrives */
        wr.wr_id = i;
        /* post receive buffer to ring */
        ibv_post_recv(qp, &wr, &bad_wr);
    }

    // Register steering rule to intercept packet to DEST_MAC and place packet in ring pointed by ->qp
    struct raw_eth_flow_attr {
    struct ibv_flow_attr attr;
    struct ibv_flow_spec_eth spec_eth;
    } __attribute__((packed)) flow_attr = {
    .attr = {
    .comp_mask = 0,
    .type = IBV_FLOW_ATTR_NORMAL,
    .size = sizeof(flow_attr),
    .priority = 0,
    .num_of_specs = 1,
    .port = 1,
    .flags = 0,
    },
    .spec_eth = {
    .type = IBV_FLOW_SPEC_ETH,
    .size = sizeof(struct ibv_flow_spec_eth),
    .val = {
        .dst_mac = DST_MAC,
        .src_mac = SRC_MAC,
        .ether_type = 0,
        .vlan_tag = 0,
    },
    .mask = {
        .dst_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    .src_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
    .ether_type = 0,
    .vlan_tag = 0,
    }
    }
    };

    //create steering rule
    struct ibv_flow *eth_flow;
    eth_flow = ibv_create_flow(qp, &flow_attr.attr);
    if (!eth_flow) {
        fprintf(stderr, "Couldn't attach steering flow\n");
        exit(1);
    }

    //
    int msgs_completed;
    struct ibv_wc wc;

    struct ibv_cq *ev_cq;
    int ev_cq_ctx;

    printf("ready to go!\n");

    struct pollfd pfd;
    int poll_rc = 0;

    while(1)
    {
        pfd.fd = recv_cc->fd;
        pfd.events = POLLIN;
        pfd.revents = 0;

        poll_rc = poll(&pfd, 1, 50);
        if(poll_rc <= 0)
            continue;

        state = ibv_get_cq_event(recv_cc, &ev_cq, (void **)&ev_cq_ctx);
        if(state)
        {
            perror("ibv_get_cq_event");
            exit(1);
        }

        ibv_ack_cq_events(ev_cq, 1);

        if(ibv_req_notify_cq(cq, 0))
        {
            perror("ibv_req_notify_cq");
            exit(1);
        }

        msgs_completed = ibv_poll_cq(cq, 1, &wc);
        if(msgs_completed > 0)
        {
            printf("message %ld received size %d\n", wc.wr_id, wc.byte_len);
            wr.wr_id = wc.wr_id;
            wr.sg_list = &sg_entry[wc.wr_id];
            ibv_post_recv(qp, &wr, &bad_wr);
        }else if(msgs_completed < 0)
        {
            printf("Polling error\n");
            exit(1);
        }
    }
    printf("We are done\n");
    return 0;
    
}