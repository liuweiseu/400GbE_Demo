#include <infiniband/verbs.h>

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/time.h>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <getopt.h>
#include <string.h>

struct args {
    uint8_t device_id;
    uint8_t help_info;
};
/*
Parsse the command line arguments.
*/
void parse_args(struct args *args, int argc, char *argv[])
{
    int c;
    int long_option_index = -1;
    struct option long_options[] = {
        {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
        {0, 0, 0, 0}
    };
    while(1)
    {
        c = getopt_long(argc, argv, "d:h", long_options, &long_option_index);
        switch (c)
        {
            case 'd':
                sscanf(optarg, "%hhd", &args->device_id);
                break;
            case 'h':
                args->help_info = 1;
                break;
            case -1:
                return;
            default:
                printf("Invalid option.\n");
                args->help_info = 1;
                return;
        }
    }
}

/*
Print out device attribute.
*/
void print_dev_attr(struct ibv_device_attr *device_attr)
{
    printf("    |-- Device attr:\n");
    printf("        |-- fw_ver: %s\n", device_attr->fw_ver);
    printf("        |-- node_guid: 0x%llx\n", device_attr->node_guid);
    printf("        |-- sys_image_guid: 0x%llx\n", device_attr->sys_image_guid);
    printf("        |-- max_mr_size: %" PRIu64 "\n", device_attr->max_mr_size);
    printf("        |-- page_size_cap: %" PRIu64 "\n", device_attr->page_size_cap);
    printf("        |-- vendor_id: %d\n", device_attr->vendor_id);
    printf("        |-- vendor_part_id: %d\n", device_attr->vendor_part_id);
    printf("        |-- hw_ver: %d\n", device_attr->hw_ver);
    printf("        |-- max_qp: %d\n", device_attr->max_qp);
    printf("        |-- max_qp_wr: %d\n", device_attr->max_qp_wr);
    printf("        |-- device_cap_flags: %d\n", device_attr->device_cap_flags);
    printf("        |-- max_sge: %d\n", device_attr->max_sge);
    printf("        |-- max_sge_rd: %d\n", device_attr->max_sge_rd);
    printf("        |-- max_cq: %d\n", device_attr->max_cq);
    printf("        |-- max_cqe: %d\n", device_attr->max_cqe);
    printf("        |-- max_mr: %d\n", device_attr->max_mr);
    printf("        |-- max_pd: %d\n", device_attr->max_pd);
    printf("        |-- max_qp_rd_atom: %d\n", device_attr->max_qp_rd_atom);
    printf("        |-- max_ee_rd_atom: %d\n", device_attr->max_ee_rd_atom);
    printf("        |-- max_res_rd_atom: %d\n", device_attr->max_res_rd_atom);
    printf("        |-- max_qp_init_rd_atom: %d\n", device_attr->max_qp_init_rd_atom);
    printf("        |-- max_ee_init_rd_atom: %d\n", device_attr->max_ee_init_rd_atom);
    printf("        |-- atomic_cap: %d\n", device_attr->atomic_cap);
    printf("        |-- max_ee: %d\n", device_attr->max_ee);
    printf("        |-- max_rdd: %d\n", device_attr->max_rdd);
    printf("        |-- max_mw: %d\n", device_attr->max_mw);
    printf("        |-- max_raw_ipv6_qp: %d\n", device_attr->max_raw_ipv6_qp);
    printf("        |-- max_raw_ethy_qp: %d\n", device_attr->max_raw_ethy_qp);
    printf("        |-- max_mcast_grp: %d\n", device_attr->max_mcast_grp);
    printf("        |-- max_mcast_qp_attach: %d\n", device_attr->max_mcast_qp_attach);
    printf("        |-- max_total_mcast_qp_attach: %d\n", device_attr->max_total_mcast_qp_attach);
    printf("        |-- max_ah: %d\n", device_attr->max_ah);
    printf("        |-- max_fmr: %d\n", device_attr->max_fmr);
    printf("        |-- max_map_per_fmr: %d\n", device_attr->max_map_per_fmr);
    printf("        |-- max_srq: %d\n", device_attr->max_srq);
    printf("        |-- max_srq_wr: %d\n", device_attr->max_srq_wr);
    printf("        |-- max_srq_sge: %d\n", device_attr->max_srq_sge);
    printf("        |-- max_pkeys: %d\n", device_attr->max_pkeys);
    printf("        |-- local_ca_ack_delay: %d\n", device_attr->local_ca_ack_delay);
    printf("        |-- phys_port_cnt: %d\n", device_attr->phys_port_cnt);
}

/*
Print out port attribute.
*/
void print_port_attr(struct ibv_port_attr *port_attr)
{
    printf("    |-- Port attr:\n");
    printf("        |-- state: %d\n", port_attr->state);
    printf("        |-- max_mtu: %d\n", port_attr->max_mtu);
    printf("        |-- active_mtu: %d\n", port_attr->active_mtu);
    printf("        |-- gid_tbl_len: %d\n", port_attr->gid_tbl_len);
    printf("        |-- port_cap_flags: %d\n", port_attr->port_cap_flags);
    printf("        |-- max_msg_sz: %d\n", port_attr->max_msg_sz);
    printf("        |-- bad_pkey_cntr: %d\n", port_attr->bad_pkey_cntr);
    printf("        |-- qkey_viol_cntr: %d\n", port_attr->qkey_viol_cntr);
    printf("        |-- pkey_tbl_len: %d\n", port_attr->pkey_tbl_len);
    printf("        |-- lid: %d\n", port_attr->lid);
    printf("        |-- sm_lid: %d\n", port_attr->sm_lid);
    printf("        |-- lmc: %d\n", port_attr->lmc);
    printf("        |-- max_vl_num: %d\n", port_attr->max_vl_num);
    printf("        |-- sm_sl: %d\n", port_attr->sm_sl);
    printf("        |-- subnet_timeout: %d\n", port_attr->subnet_timeout);
    printf("        |-- init_type_reply: %d\n", port_attr->init_type_reply);
    printf("        |-- active_width: %d\n", port_attr->active_width);
    printf("        |-- active_speed: %d\n", port_attr->active_speed);
    printf("        |-- phys_state: %d\n", port_attr->phys_state);
	printf("        |-- link_layer: %d\n", port_attr->link_layer);
	printf("        |-- flags: %d\n", port_attr->flags);
	printf("        |-- port_cap_flags2: %d\n", port_attr->port_cap_flags2);
	printf("        |-- active_speed: %d\n", port_attr->active_speed);
}

/*
Print out gid.
*/
void print_gid(union ibv_gid *gid)
{
    printf("    |-- GID Info:\n");
    printf("        |-- raw[0-7]: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",gid->raw[0],
                                                            gid->raw[1],
                                                            gid->raw[2],
                                                            gid->raw[3],
                                                            gid->raw[4],
                                                            gid->raw[5],
                                                            gid->raw[6],
                                                            gid->raw[7]);
    printf("        |-- raw[7:15]: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",gid->raw[8],
                                                            gid->raw[9],
                                                            gid->raw[10],
                                                            gid->raw[11],
                                                            gid->raw[12],
                                                            gid->raw[13],
                                                            gid->raw[14],
                                                            gid->raw[15]);
    printf("        |-- global.subnet_prefix: 0x%llx\n", gid->global.subnet_prefix);
    printf("        |-- global.interface_id: 0x%llx\n", gid->global.interface_id);
}

/*
Print out pkey.
*/
void print_pkey(uint16_t pkey)
{   
    printf("    |-- Pkey Info:\n");
    printf("        |-- pkey: 0x%x\n", pkey);
}

/*
Print out the helper information.
*/
void print_helper()
{
    printf("Usage:\n");
    printf("    NICInfo     Print out NIC information.\n\n");
    printf("Options:\n");
    printf("    -d, NIC dev number. '0' means mlx5_0.\n");
    printf("    --help, -h,  print out the helper information.\n");
}

int main(int argc, char *argv[])
{
    int dev_num = 0;
    struct args args;
    args.device_id = 0;
    args.help_info = 0;
    parse_args(&args, argc, argv);
    dev_num = args.device_id;
    if (args.help_info)
    {
        print_helper();
        return 0;
    }
    // get device list
    struct ibv_device **dev_list;
    int ndev;
    dev_list = ibv_get_device_list(&ndev);
    if (!dev_list) {
        perror("Failed to get devices list");
        exit(1);
    }
    printf("Number of Device: %d\n", ndev);

    // get device name
    struct ibv_device *ib_dev;
    ib_dev = dev_list[dev_num];
    const char *dev_name;
    dev_name = ibv_get_device_name(ib_dev);
    printf("Device Name: %s\n",dev_name);

    // get device guid
    uint64_t dev_guid;
    dev_guid = ibv_get_device_guid(ib_dev);
    printf("    |-- Device GUID: %lx\n", dev_guid);
    // get node type
    const char *ib_node_type;
    ib_node_type = ibv_node_type_str(dev_list[dev_num]->node_type);
    printf("    |-- Node Type: %s\n", ib_node_type);

    //open device
    struct ibv_context *context;
    context = ibv_open_device(ib_dev);
	printf("    |-- Transport Type: %d\n", context->device->transport_type);
    int state;
    // query the device and print the device attr
    struct ibv_device_attr device_attr;
    state = ibv_query_device(context, &device_attr);
    print_dev_attr(&device_attr);
    
    // query the port
    struct ibv_port_attr port_attr;
    uint8_t port_num = 1;
    state = ibv_query_port(context, port_num, &port_attr);   
    print_port_attr(&port_attr);

    // query gid
    union ibv_gid gid;
    int index = 0;
    state = ibv_query_gid(context, port_num, index, &gid);
    print_gid(&gid);

    // query pkey
    uint16_t pkey;
    state = ibv_query_pkey(context, port_num, index, &pkey);
    print_pkey(pkey);

    return 0;
}