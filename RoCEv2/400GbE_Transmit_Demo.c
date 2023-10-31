#include <infiniband/verbs.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "packet.h"

#define WR_N 1024

#define DELAY	0

unsigned char dst_mac[6] = {0x94, 0x6d, 0xae, 0xac, 0xf8, 0x38};
unsigned char dst_mac1[6] = {0x94, 0x6d, 0xae, 0xac, 0xf8, 0x39};

unsigned char src_mac[6] = {0xa0, 0x88, 0xc2, 0x0d, 0x5e, 0x28};
unsigned char eth_type[2] = {0x08, 0x00};
unsigned char ip_hdrs[12] = {0x45, 0x00, 0x1f, 0x54, 0x00, 0x00, 0x40, 0x00, 0x40, 0x11, 0xaf, 0xb6};
unsigned char src_ip[4] = {192,168,3,2};
unsigned char dst_ip[4] = {192,168,3,12};
unsigned char udp_hdr[8] = {0x05, 0xd4, 0x05, 0xd4, 0x1f, 0x40, 0x00, 0x00};

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

void print_helper()
{
    printf("Usage:\n");
    printf("    400GbE_TDemo     Transmit demo at 400Gbps\n\n");
    printf("Options:\n");
    printf("    -h, print out the helper information.\n");
    printf("    -v, print out the query information.\n");
    printf("    -d, device number. '0' means mlx5_0.\n");
    printf("    --md, multi dst addresses.\n");
	printf("    -n, the number of work request. the default values 1; the max value is 16384.\n");
    printf("    -N, the number of groups of work request. the number in each wr group is 16384.\n");
    printf("    --inf, keeping sending out data.\n");
    printf("    --woip, send raw ethernet packet without IP setting.\n");
}

int main(int argc, char *argv[])
{
    int dev_num = 0;
    int verbose = 0;
    int inf = 0;
    int n_wr = 1;
    int N_wr = 0;
    int woip = 0;
	int md = 0;
    for(int i=1; i < argc;)
    {
        if(!strcmp(argv[i],"-d"))
        {
            i++;
            dev_num = atoi(argv[i]);
        }
        if(!strcmp(argv[i], "-v"))
            verbose = 1;
        if(!strcmp(argv[i], "--inf"))
            inf = 1;
        if(!strcmp(argv[i], "-n"))
        {
            i++;
            n_wr = atoi(argv[i]); 
        }
        if(!strcmp(argv[i], "-N"))
        {
            i++;
            N_wr = atoi(argv[i]); 
        }
		if(!strcmp(argv[i], "--md"))
			md = 1;
        if(!strcmp(argv[i], "--woip"))
            woip = 1;
        if(!strcmp(argv[i], "-h"))
        {
            print_helper();
            return 0;
        }
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
    ib_node_type = ibv_node_type_str(dev_list[dev_num]->node_type);
    if(verbose)
        printf("node_type: %s\n", ib_node_type);

    // open the device
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
    cq = ibv_create_cq(context, WR_N, NULL, NULL, 0);
    if(!cq){
        fprintf(stderr, "Couldn't create CQ.\n");
        exit(1);
    }

    // create qp
    struct ibv_qp *qp;
    struct ibv_qp_init_attr qp_init_attr = {
        .qp_context = NULL,
        .send_cq = cq,
        .recv_cq = cq,
        .cap = {
            .max_send_wr = WR_N,
            .max_send_sge = 1,
            .max_recv_wr = 0
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

    // move the ring to send
    memset(&qp_attr, 0, sizeof(qp_attr));
    qp_flags = IBV_QP_STATE;
    qp_attr.qp_state = IBV_QPS_RTS;
    state = ibv_modify_qp(qp, &qp_attr, qp_flags);
    if(state < 0)
    {
        fprintf(stderr, "Failed to modify qp to RTS.\n");
        exit(1);
    }
    else
        printf("QP is ready to send data!\n");

    // Allocate Memory
    void *buf;
    int buf_size = PACKET_SIZE * WR_N;
    buf = malloc(PACKET_SIZE*WR_N);
    struct packet *pkt;
    for(int i=0; i < WR_N; i++)
    {
        pkt = (struct packet *)(buf+i*PACKET_SIZE);
        //memcpy(pkt->dst_mac, dst_mac, 6);
		
		if(i%2==0)
			memcpy(pkt->dst_mac, dst_mac, 6);
        else
		{
			if(md == 0)
				memcpy(pkt->dst_mac, dst_mac, 6);
			else
				memcpy(pkt->dst_mac, dst_mac1, 6);
		}
		
		memcpy(pkt->src_mac, src_mac, 6);
        if(woip == 0)
        {
            memcpy(pkt->eth_type, eth_type, 2);
            memcpy(pkt->ip_hdrs, ip_hdrs, 12);
            memcpy(pkt->src_ip, src_ip, 4);
            memcpy(pkt->dst_ip, dst_ip, 4);
            memcpy(pkt->udp_hdr, udp_hdr, 8);
        }
        pkt->payload[0] = 0x55;
        pkt->payload[1] = i&0xff;
        pkt->payload[2] = i >> 8;
    }

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
    struct ibv_send_wr wr[WR_N], *bad_wr;
    for(int i=0; i< WR_N; i++)
    {
        memset(&wr[i], 0, sizeof(wr[i]));
        wr[i].num_sge = 1;
        wr[i].sg_list= &sg_entry[i];
        wr[i].next = NULL;
        wr[i].opcode = IBV_WR_SEND;
        wr[i].wr_id = i;
        wr[i].send_flags |= IBV_SEND_SIGNALED;
    }

    int msc;
    struct ibv_wc wc[WR_N];
    // ready to send
    int i, j;
    int ns = 0;
    if(inf)
    {
		while(1)
		{
			for(i = 0; i < WR_N; i++)
			{
				state = ibv_post_send(qp, &wr[i], &bad_wr);	
				if (state < 0) {
					fprintf(stderr, "failed in post send\n");
					exit(1);
				}
			}
			ns = 0;
			while(ns < WR_N)
			{
				msc = ibv_poll_cq(cq, WR_N, wc);
				ns += msc;
			}
				
		}

		/*
        while(1)
        {
            msc = ibv_poll_cq(cq, WR_N, wc);
            if(msc > 0)
			{
				for(i = 0; i < msc; i++)
				{
					state = ibv_post_send(qp, &wr[wc[i].wr_id], &bad_wr);
					//for(int k=0;k<DELAY;k++);
					if (state < 0) {
						fprintf(stderr, "failed in post send\n");
						exit(1);
					}
				}
			}
        } 
		*/
    }
    else
    {
        if(N_wr != 0){
            for(j = 0; j<N_wr; j++)
            {
                for(i = 0; i < WR_N; i++)
                {
                    state = ibv_post_send(qp, &wr[i], &bad_wr);
					//for(int k=0; k<DELAY; k++);
                    if (state < 0) {
                        fprintf(stderr, "failed in post send\n");
                        exit(1);
                    }
                } 
                // we need to wait until all the wr are completed.
                while(ns < WR_N)
                {
                    msc = ibv_poll_cq(cq, WR_N, wc);
                    ns += msc;
                }
                ns = 0;
            }
        }
        else
        {
            for(i = 0; i < n_wr; i++)
                {
                    state = ibv_post_send(qp, &wr[i], &bad_wr);
                    if (state < 0) {
                        fprintf(stderr, "failed in post send\n");
                        exit(1);
                    }
                } 
                // we need to wait until all the wr are completed.
                while(ns < n_wr)
                {
                    msc = ibv_poll_cq(cq, n_wr, wc);
                    ns += msc;
                }
        }
    }
    
    // dealloc pd
    ibv_dealloc_pd(pd);
    // destory cq
    ibv_destroy_cq(cq);
    // dereg mr
    ibv_dereg_mr(mr);
    // destory qp
    ibv_destroy_qp(qp);
    // close the device
    state = ibv_close_device(context);
    printf("Dev close.\n");
    return 0;
}
