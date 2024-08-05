#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

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

uint8_t src_mac[6] = SRC_MAC;
uint8_t dst_mac[6] = DST_MAC;

uint64_t ns_elapsed;
struct timespec ts_start;
struct timespec ts_now;
int total_recv;
int total_recv_pre;
int msgs_completed;

void main() {
    int device_id = 0;
    int num_dev = 0;
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

    // init ib resources
    ret = init_ib_res(device_id);
    if (ret < 0) {
        printf("Failed to init ib resources.\n");
        return;
    }

    // register memory
    uint8_t *buf = (uint8_t *)malloc(PKT_LEN * 512);
    ret = register_memory(device_id, buf, PKT_LEN * 512, PKT_LEN);
    if (ret < 0) {
        printf("Failed to register memory.\n");
        return;
    }

    // create flow
    struct pkt_info pkt_info;
    memcpy(pkt_info.src_mac, src_mac, 6);
    memcpy(pkt_info.dst_mac, dst_mac, 6);
    pkt_info.src_ip = SRC_IP;
    pkt_info.dst_ip = DST_IP;
    pkt_info.src_port = SRC_PORT;
    pkt_info.dst_port = DST_PORT;
    create_flow(device_id, &pkt_info);

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
				printf("total_recv-0: %d\n",total_recv);
			}
		}

        msgs_completed = ib_recv(device_id);
        if (msgs_completed < 0) {
            printf("Failed to recv.\n");
            return;
        }
        total_recv += msgs_completed;
    }
}