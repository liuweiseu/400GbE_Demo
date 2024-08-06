#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>

#include "ibv_utils.h"
#include "pkt_gen.h"

#define ELAPSED_NS(start,stop) \
    (((int64_t)stop.tv_sec-start.tv_sec)*1000*1000*1000+(stop.tv_nsec-start.tv_nsec))
#define UPDATE_MS	1000

#define SRC_MAC {0xa0, 0x88, 0xc2, 0x0d, 0x5e, 0x28}
#define DST_MAC {0x94, 0x6d, 0xae, 0xac, 0xf8, 0x38}
#define SRC_IP  0x0203a8c0
#define DST_IP  0x0c03a8c0
#define SRC_PORT 0xd405
#define DST_PORT 0xd405
#define ETH_TYPE {0x08, 0x00}

int device_id = 0;
uint8_t src_mac[6] = SRC_MAC;
uint8_t dst_mac[6] = DST_MAC;

uint64_t ns_elapsed;
struct timespec ts_start;
struct timespec ts_now;
int total_recv;
int total_recv_pre;
int msgs_completed;

struct args {
    int device_id;
    struct pkt_info pkt_info;
    int gpu;
};

/*
Print out help information.
*/
void print_helper()
{
    printf("Usage:\n");
    printf("    RecvDemo     Receiver demo at 400Gbps\n\n");
    printf("Options:\n");
    printf("    -h, print out the helper information.\n");
    printf("    -d, NIC dev number. '0' means mlx5_0.\n");
    printf("    --gpu, allocate memory on GPU. the memory is allocated on the host by default.\n");
}

/*
Parsse the command line arguments.
*/
void parse_args(struct args *args, int argc, char *argv[])
{
    int c;
    int long_option_index = -1;
    uint8_t tmp[4];
    struct option long_options[] = {
        {.name = "smac", .has_arg = required_argument, .flag = NULL, .val = 'S'},
        {.name = "dmac", .has_arg = required_argument, .flag = NULL, .val = 'D'},
        {.name = "sip", .has_arg = required_argument, .flag = NULL, .val = 's'},
        {.name = "dip", .has_arg = required_argument, .flag = NULL, .val = 'd'},
        {.name = "sport", .has_arg = required_argument, .flag = NULL, .val = 'p'},
        {.name = "dport", .has_arg = required_argument, .flag = NULL, .val = 'P'},
        {.name = "gpu", .has_arg = no_argument, .flag = NULL, .val = 'g'},
        {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
        {0, 0, 0, 0}
    };
    while(1)
    {
        //c = getopt_long(argc, argv, "S:D:s:d:p:P:gh", long_options, &long_option_index);
        c = getopt_long(argc, argv, "S:D:s:d:p:P:gh", long_options, &long_option_index);
        switch (c)
        {
            case 'S':
                sscanf(optarg, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                                &args->pkt_info.src_mac[0], 
                                &args->pkt_info.src_mac[1],
                                &args->pkt_info.src_mac[2],
                                &args->pkt_info.src_mac[3],
                                &args->pkt_info.src_mac[4],
                                &args->pkt_info.src_mac[5]);
                break;
            case 'D':
                sscanf(optarg, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                                &args->pkt_info.dst_mac[0],
                                &args->pkt_info.dst_mac[1],
                                &args->pkt_info.dst_mac[2],
                                &args->pkt_info.dst_mac[3],
                                &args->pkt_info.dst_mac[4],
                                &args->pkt_info.dst_mac[5]);
                break;
            case 's':       
                sscanf(optarg, "%hhd.%hhd.%hhd.%hhd", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);
                args->pkt_info.src_ip = (tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];
                break;
            case 'd':
                sscanf(optarg, "%hhd.%hhd.%hhd.%hhd", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);
                args->pkt_info.dst_ip = (tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];
                break;
            case 'p':
                sscanf(optarg, "%hd", &args->pkt_info.src_port);
                break;
            case 'P':
                sscanf(optarg, "%hd", &args->pkt_info.dst_port);
                break;
            case 'g':
                break;
            case 'h':
                print_helper();
                break;
            case -1:
                return;
            default:
                printf("Invalid option.\n");
                print_helper();
                break;
        }
    }
}

/*
Print out device information.
*/
void print_dev_info(struct args *args){
    printf("Device information:\n");
    printf("    device_id: %d\n", device_id);
    printf("    src_mac: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                args->pkt_info.src_mac[0], 
                args->pkt_info.src_mac[1], 
                args->pkt_info.src_mac[2], 
                args->pkt_info.src_mac[3], 
                args->pkt_info.src_mac[4], 
                args->pkt_info.src_mac[5]);
    printf("    dst_mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
                args->pkt_info.dst_mac[0],
                args->pkt_info.dst_mac[1],
                args->pkt_info.dst_mac[2],
                args->pkt_info.dst_mac[3],
                args->pkt_info.dst_mac[4],
                args->pkt_info.dst_mac[5]);
    uint8_t tmp[4];
    tmp[3] = (args->pkt_info.src_ip >> 24) & 0xff;
    tmp[2] = (args->pkt_info.src_ip >> 16) & 0xff;
    tmp[1] = (args->pkt_info.src_ip >> 8) & 0xff;
    tmp[0] = args->pkt_info.src_ip & 0xff;
    printf("    src_ip: %d.%d.%d.%d\n", tmp[0], tmp[1], tmp[2], tmp[3]);
    tmp[3] = (args->pkt_info.dst_ip >> 24) & 0xff;
    tmp[2] = (args->pkt_info.dst_ip >> 16) & 0xff;
    tmp[1] = (args->pkt_info.dst_ip >> 8) & 0xff;
    tmp[0] = args->pkt_info.dst_ip & 0xff;
    printf("    dst_ip: %d.%d.%d.%d\n", tmp[0], tmp[1], tmp[2], tmp[3]);
    printf("    src_port: %d\n", args->pkt_info.src_port);
    printf("    dst_port: %d\n", args->pkt_info.dst_port);
}

void main(int argc, char *argv[]){

    int num_dev = 0;
    struct args args;
    args.device_id = 0;
    args.gpu = 0;
    parse_args(&args, argc, argv);
    print_dev_info(&args);

    return;
    printf("Start to recv.\n");
    // get ib device list
    num_dev = get_ib_devices();
    printf("The number of ib devices is %d.\n", num_dev);
    // open ib device by id
    int ret = 0;
    ret = open_ib_device(device_id);
    if (ret < 0) {
        printf("Failed to open IB device.\n");
        return;
    }
    printf("Open IB device successfully.\n");

    // only implement recv here
    ret = create_ib_res(device_id, 0, 512);
    if (ret < 0) {
        printf("Failed to create ib resources.\n");
        return;
    }
    else
    {
        printf("Create IB resources successfully.\n");
    }

    // init ib resources
    ret = init_ib_res(device_id);
    if (ret < 0) {
        printf("Failed to init ib resources.\n");
        return;
    }
    else
    {
        printf("Init IB resources successfully.\n");
    }

    // register memory
    uint8_t *buf = (uint8_t *)malloc(PKT_LEN * 512);
    ret = register_memory(device_id, buf, PKT_LEN * 512, PKT_LEN);
    if (ret < 0) {
        printf("Failed to register memory.\n");
        return;
    }
    else
    {
        printf("Register memory successfully.\n");
    }

    // create flow
    struct pkt_info pkt_info;
    memcpy(pkt_info.src_mac, src_mac, 6);
    memcpy(pkt_info.dst_mac, dst_mac, 6);
    pkt_info.src_ip = SRC_IP;
    pkt_info.dst_ip = DST_IP;
    pkt_info.src_port = SRC_PORT;
    pkt_info.dst_port = DST_PORT;
    ret = create_flow(device_id, &pkt_info);
    if (ret < 0) {
        printf("Failed to create flow.\n");
        return;
    }
    else
    {
        printf("Create flow successfully.\n");
    }
    // set global resources
    set_global_res(device_id);

    // recv
    while (1) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &ts_now);
		ns_elapsed = ELAPSED_NS(ts_start, ts_now);
		if(ns_elapsed > UPDATE_MS * 1000 * 1000)
		{
			ts_start = ts_now;
			if(total_recv != total_recv_pre)
			{
				total_recv_pre = total_recv;
				printf("total_recv: %d\n",total_recv);
			}
		}
        msgs_completed = ib_recv(device_id);
        if (msgs_completed < 0) {
            printf("Failed to recv.\n");
            return;
        }
        total_recv += msgs_completed;
    }
    free(buf);
    destroy_ib_res(device_id);
    close_ib_device(device_id);
}