#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <getopt.h>
#include <infiniband/verbs.h>

#include "cuda.h"
#include "cuda_runtime.h"

#include "ibv_utils.h"
#include "pkt_gen.h"
#include "utils.h"

/*
Print out help information.
*/
void print_recv_helper()
{
    printf("Usage:\n");
    printf("    RecvDemo     Receiver Demo at 400Gbps\n\n");
    printf("Options:\n");
    printf("    -d, NIC dev number. '0' means mlx5_0.\n");
    printf("    --nsge, number of sge. By default, nsge = 1.\n");
    printf("    --smac, source MAC address.\n");
    printf("    --dmac, destination MAC address.\n");
    printf("    --sip, source IP address.\n");
    printf("    --dip, destination IP address.\n");
    printf("    --sport, source port number.\n");
    printf("    --dport, destination port number.\n");
    printf("    --gpu, allocate memory on GPU. the memory is allocated on the host by default.\n");
    printf("    --disable-recv, disable recv.\n");
    printf("    --help, -h,  print out the helper information.\n");
}


int main(int argc, char *argv[]){

    int num_dev = 0;
    uint64_t ns_elapsed;
    struct timespec ts_start;
    struct timespec ts_now;
    int total_recv;
    int total_recv_pre;
    int msgs_completed;
    float bandwidth;
    struct recv_args args;
    init_recv_args(&args);
    parse_recv_args(&args, argc, argv);
    if(args.help_info)
    {
        print_recv_helper();
        return 0;
    }
    print_recv_info(&args);
    
    struct ibv_utils_res ibv_res;
    memset(&ibv_res, 0, sizeof(struct ibv_utils_res));
    // get values from args
    ibv_res.recv_nsge = args.nsge;
    ibv_res.send_nsge = args.nsge;
    printf("Start to recv...\n");
    // get ib device list
    num_dev = get_ib_devices();
    printf("The number of ib devices is %d.\n", num_dev);
    // open ib device by id
    int ret = 0;
    ret = open_ib_device(args.device_id, &ibv_res);
    if (ret < 0) {
        printf("Failed to open IB device.\n");
        return -1;
    }
    printf("Open IB device successfully.\n");

    // only implement recv here
    ret = create_ib_res(&ibv_res, 0, 512);
    if (ret < 0) {
        printf("Failed to create ib resources.\n");
        return -2;
    }
    else
    {
        printf("Create IB resources successfully.\n");
    }

    // init ib resources
    ret = init_ib_res(&ibv_res);
    if (ret < 0) {
        printf("Failed to init ib resources.\n");
        return -3;
    }
    else
    {
        printf("Init IB resources successfully.\n");
    }

    // register memory;
    void *buf;
    uint32_t buf_size = PKT_LEN * 512 * ibv_res.recv_nsge;
    if (args.use_gpu) {
        int state;
        unsigned int flag = 1;
        printf("Allocate memory on GPU.\n");
        cudaSetDevice(args.gpu_id);
        state = cudaMalloc((void **) &buf, buf_size);
        if(state == 0)
            printf("Allocate GPU memory successfully!\n");
        else
        {
            printf("Failed to allocate GPU memory.\n");
            exit(1);
        }
        state = cuPointerSetAttribute(&flag, CU_POINTER_ATTRIBUTE_SYNC_MEMOPS, (uintptr_t)buf);
        if(state == 0)
            printf("Pinned GPU memory successfully!\n");
        else
        {
            printf("Failed to pin GPU memory.\n");
            exit(1);
        }
    }
    else
    {
        printf("Allocate memory on host.\n");
        // buf = malloc(buf_size);
        cudaMallocHost((void **)&buf, buf_size);
        if (buf == NULL) {
            printf("Failed to allocate memory.\n");
            return -4;
        }
        else
        {
            printf("Allocate memory successfully.\n");
        }
    }
    
    ret = register_memory(&ibv_res, buf, buf_size, PKT_LEN);
    if (ret < 0) {
        printf("Failed to register memory.\n");
        return -4;
    }
    else
    {
        printf("Register memory successfully.\n");
    }

    // create flow
    ret = create_flow(&ibv_res, (struct ibv_pkt_info *)&args.pkt_info);
    if (ret < 0) {
        printf("Failed to create flow.\n");
        return -5;
    }
    else
    {
        printf("Create flow successfully.\n");
    }

    // recv
    while (1) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &ts_now);
		ns_elapsed = ELAPSED_NS(ts_start, ts_now);
		if(ns_elapsed > UPDATE_MS * 1000 * 1000)
		{
			ts_start = ts_now;
			if(total_recv != total_recv_pre)
			{
                bandwidth = MEASURE_BANDWIDTH((total_recv - total_recv_pre) * PKT_LEN, ns_elapsed);
                total_recv_pre = total_recv;
				printf("total_recv: %-10d Bandwidth: %6.3f Gbps\n",total_recv, bandwidth);
			}
		}
        if(args.disable_recv == 0)msgs_completed = ib_recv(&ibv_res);
        if (msgs_completed < 0) {
            printf("Failed to recv.\n");
            return -6;
        }
        total_recv += msgs_completed;
    }
    if(args.use_gpu)
        cudaFree(buf);
    else
        //free(buf);
        cudaFreeHost(buf);
    free_recv_args(&args);
    destroy_ib_res(&ibv_res);
    close_ib_device(&ibv_res);
    return 0;
}