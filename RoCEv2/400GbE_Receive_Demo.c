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

void print_dev_attr(struct ibv_device_attr *device_attr)
{
    printf("****************************Device attr*****************************\n");
    printf("fw_ver: %s\n", device_attr->fw_ver);
    printf("node_guid: 0x%llx\n", device_attr->node_guid);
    printf("sys_image_guid: 0x%llx\n", device_attr->sys_image_guid);
    printf("max_mr_size: %" PRIu64 "\n", device_attr->max_mr_size);
    printf("page_size_cap: %" PRIu64 "\n", device_attr->page_size_cap);
    printf("vendor_id: %d\n", device_attr->vendor_id);
    printf("vendor_part_id: %d\n", device_attr->vendor_part_id);
    printf("hw_ver: %d\n", device_attr->hw_ver);
    printf("max_qp: %d\n", device_attr->max_qp);
    printf("max_qp_wr: %d\n", device_attr->max_qp_wr);
    printf("device_cap_flags: %d\n", device_attr->device_cap_flags);
    printf("max_sge: %d\n", device_attr->max_sge);
    printf("max_sge_rd: %d\n", device_attr->max_sge_rd);
    printf("max_cq: %d\n", device_attr->max_cq);
    printf("max_cqe: %d\n", device_attr->max_cqe);
    printf("max_mr: %d\n", device_attr->max_mr);
    printf("max_pd: %d\n", device_attr->max_pd);
    printf("max_qp_rd_atom: %d\n", device_attr->max_qp_rd_atom);
    printf("max_ee_rd_atom: %d\n", device_attr->max_ee_rd_atom);
    printf("max_res_rd_atom: %d\n", device_attr->max_res_rd_atom);
    printf("max_qp_init_rd_atom: %d\n", device_attr->max_qp_init_rd_atom);
    printf("max_ee_init_rd_atom: %d\n", device_attr->max_ee_init_rd_atom);
    printf("atomic_cap: %d\n", device_attr->atomic_cap);
    printf("max_ee: %d\n", device_attr->max_ee);
    printf("max_rdd: %d\n", device_attr->max_rdd);
    printf("max_mw: %d\n", device_attr->max_mw);
    printf("max_raw_ipv6_qp: %d\n", device_attr->max_raw_ipv6_qp);
    printf("max_raw_ethy_qp: %d\n", device_attr->max_raw_ethy_qp);
    printf("max_mcast_grp: %d\n", device_attr->max_mcast_grp);
    printf("max_mcast_qp_attach: %d\n", device_attr->max_mcast_qp_attach);
    printf("max_total_mcast_qp_attach: %d\n", device_attr->max_total_mcast_qp_attach);
    printf("max_ah: %d\n", device_attr->max_ah);
    printf("max_fmr: %d\n", device_attr->max_fmr);
    printf("max_map_per_fmr: %d\n", device_attr->max_map_per_fmr);
    printf("max_srq: %d\n", device_attr->max_srq);
    printf("max_srq_wr: %d\n", device_attr->max_srq_wr);
    printf("max_srq_sge: %d\n", device_attr->max_srq_sge);
    printf("max_pkeys: %d\n", device_attr->max_pkeys);
    printf("local_ca_ack_delay: %d\n", device_attr->local_ca_ack_delay);
    printf("phys_port_cnt: %d\n", device_attr->phys_port_cnt);
    printf("********************************************************************\n");
}

void print_port_attr(struct ibv_port_attr *port_attr)
{
    printf("****************************port attr*******************************\n");
    printf("state: %d\n", port_attr->state);
    printf("max_mtu: %d\n", port_attr->max_mtu);
    printf("active_mtu: %d\n", port_attr->active_mtu);
    printf("gid_tbl_len: %d\n", port_attr->gid_tbl_len);
    printf("port_cap_flags: %d\n", port_attr->port_cap_flags);
    printf("max_msg_sz: %d\n", port_attr->max_msg_sz);
    printf("bad_pkey_cntr: %d\n", port_attr->bad_pkey_cntr);
    printf("qkey_viol_cntr: %d\n", port_attr->qkey_viol_cntr);
    printf("pkey_tbl_len: %d\n", port_attr->pkey_tbl_len);
    printf("lid: %d\n", port_attr->lid);
    printf("sm_lid: %d\n", port_attr->sm_lid);
    printf("lmc: %d\n", port_attr->lmc);
    printf("max_vl_num: %d\n", port_attr->max_vl_num);
    printf("sm_sl: %d\n", port_attr->sm_sl);
    printf("subnet_timeout: %d\n", port_attr->subnet_timeout);
    printf("init_type_reply: %d\n", port_attr->init_type_reply);
    printf("active_width: %d\n", port_attr->active_width);
    printf("active_speed: %d\n", port_attr->active_speed);
    printf("phys_state: %d\n", port_attr->phys_state);
    printf("********************************************************************\n");
}

void print_gid(union ibv_gid *gid)
{
    printf("*******************************gid info*****************************\n");
    printf("raw[0-7]: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",gid->raw[0],
                                                            gid->raw[1],
                                                            gid->raw[2],
                                                            gid->raw[3],
                                                            gid->raw[4],
                                                            gid->raw[5],
                                                            gid->raw[6],
                                                            gid->raw[7]);
    printf("raw[7:15]: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",gid->raw[8],
                                                            gid->raw[9],
                                                            gid->raw[10],
                                                            gid->raw[11],
                                                            gid->raw[12],
                                                            gid->raw[13],
                                                            gid->raw[14],
                                                            gid->raw[15]);
    printf("global.subnet_prefix: 0x%llx\n", gid->global.subnet_prefix);
    printf("global.interface_id: 0x%llx\n", gid->global.interface_id);
    printf("********************************************************************\n");
}

void print_pkey(uint16_t pkey)
{   
    printf("******************************pkey info*****************************\n");
    printf("pkey: 0x%x\n", pkey);
    printf("********************************************************************\n");
}

int main(int argc, char *argv[])
{
    int dev_num = 3;
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

    const char *dev_name;
    dev_name = ibv_get_device_name(ib_dev);
    printf("dev_name: %s\n",dev_name);

    uint64_t dev_guid;
    dev_guid = ibv_get_device_guid(ib_dev);
    if(verbose)
        printf("dev_guid: %lx\n", dev_guid);

    const char *ib_node_type;
    ib_node_type = ibv_node_type_str(dev_list[2]->node_type);
    if(verbose)
        printf("node_type: %s\n", ib_node_type);

    //open device
    struct ibv_context *context;
    context = ibv_open_device(ib_dev);
    printf("Dev opened.\n");

    int state;
    // query the device and print the device attr
    struct ibv_device_attr device_attr;
    state = ibv_query_device(context, &device_attr);
    if(verbose)
        print_dev_attr(&device_attr);
    
    // query the port
    struct ibv_port_attr port_attr;
    uint8_t port_num = 1;
    state = ibv_query_port(context, port_num, &port_attr);   
    if(verbose)
        print_port_attr(&port_attr);

    // query gid
    union ibv_gid gid;
    int index = 0;
    state = ibv_query_gid(context, port_num, index, &gid);
    if(verbose)
        print_gid(&gid);

    // query pkey
    uint16_t pkey;
    state = ibv_query_pkey(context, port_num, index, &pkey);
    if(verbose)
        print_pkey(pkey);

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
    struct ibv_comp_channel *recv_cc=NULL;
    recv_cc = ibv_create_comp_channel(context);
    cq = ibv_create_cq(context, CQE, NULL, recv_cc, 0);
    if(!cq){
        fprintf(stderr, "Couldn't create CQ.\n");
        exit(1);
    }
    
    if(ibv_req_notify_cq(cq, 0)) {
        perror("ibv_req_notify_cq");
        exit(1);
    } 
    // create qp
    struct ibv_qp *qp;
    struct ibv_qp_init_attr qp_init_attr = {
        .qp_context = NULL,
        .send_cq = cq,
        .recv_cq = cq,
        .cap = {
            .max_send_wr = 0,
            .max_recv_sge = 1,
            .max_recv_wr = CQE
        },
        .qp_type = IBV_QPT_RAW_PACKET,
    };
    qp = ibv_create_qp(pd, &qp_init_attr);

    // Initialize the QP
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
    
    memset(&qp_attr, 0, sizeof(qp_attr));

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
    uint8_t *buf_char = (uint8_t*)buf;
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
        sg_entry[i].addr = (uint64_t)(buf)+i*PACKET_SIZE;
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
    struct ibv_wc wc[WR_N];

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

        msgs_completed = ibv_poll_cq(cq, WR_N, wc);
        if(msgs_completed > 0)
        {
            for(int i = 0; i < msgs_completed; i++)
            {
                printf("message %ld received size %d\n", wc[i].wr_id, wc[i].byte_len);
                printf("data[0-1]: 0x%x, %d\n", buf_char[wc[i].wr_id*PACKET_SIZE+42],buf_char[wc[i].wr_id*PACKET_SIZE+43]);
                wr.wr_id = wc[i].wr_id;
                wr.sg_list = &sg_entry[wc[i].wr_id];
                ibv_post_recv(qp, &wr, &bad_wr);
            }
        }else if(msgs_completed < 0)
        {
            printf("Polling error\n");
            exit(1);
        }
    }
    printf("We are done\n");
    return 0;
    
}
