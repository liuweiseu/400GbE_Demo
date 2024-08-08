
#include<stdint.h>
#include "utils.h"

/*
Parsse the command line arguments.
*/
void parse_args(struct args *args, int argc, char *argv[])
{
    int c;
    int long_option_index = -1;
    uint8_t tmp[4];
    struct option long_options[] = {
        {.name = "smac", .has_arg = required_argument, .flag = NULL, .val = 256},
        {.name = "dmac", .has_arg = required_argument, .flag = NULL, .val = 257},
        {.name = "sip", .has_arg = required_argument, .flag = NULL, .val = 258},
        {.name = "dip", .has_arg = required_argument, .flag = NULL, .val = 259},
        {.name = "sport", .has_arg = required_argument, .flag = NULL, .val = 260},
        {.name = "dport", .has_arg = required_argument, .flag = NULL, .val = 261},
        {.name = "disable-recv", .has_arg = no_argument, .flag = NULL, .val = 262},
        {.name = "gpu", .has_arg = required_argument, .flag = NULL, .val = 'g'},
        {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
        {0, 0, 0, 0}
    };
    while(1)
    {
        //c = getopt_long(argc, argv, "S:D:s:d:p:P:gh", long_options, &long_option_index);
        c = getopt_long(argc, argv, "d:g:h", long_options, &long_option_index);
        switch (c)
        {
            case 'd':
                sscanf(optarg, "%hhd", &args->device_id);
                break;
            case 256:
                sscanf(optarg, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                                &args->pkt_info.src_mac[0], 
                                &args->pkt_info.src_mac[1],
                                &args->pkt_info.src_mac[2],
                                &args->pkt_info.src_mac[3],
                                &args->pkt_info.src_mac[4],
                                &args->pkt_info.src_mac[5]);
                break;
            case 257:
                sscanf(optarg, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                                &args->pkt_info.dst_mac[0],
                                &args->pkt_info.dst_mac[1],
                                &args->pkt_info.dst_mac[2],
                                &args->pkt_info.dst_mac[3],
                                &args->pkt_info.dst_mac[4],
                                &args->pkt_info.dst_mac[5]);
                break;
            case 258:       
                sscanf(optarg, "%hhd.%hhd.%hhd.%hhd", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);
                args->pkt_info.src_ip = (tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];
                break;
            case 259:
                sscanf(optarg, "%hhd.%hhd.%hhd.%hhd", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);
                args->pkt_info.dst_ip = (tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];
                break;
            case 260:
                sscanf(optarg, "%hd", &args->pkt_info.src_port);
                break;
            case 261:
                sscanf(optarg, "%hd", &args->pkt_info.dst_port);
                break;
            case 262:
                args->disable_recv = 1;
                break;
            case 'g':
                args->use_gpu = 1;
                args->gpu_id = atoi(optarg);
                break;
            case 'h':
                print_helper();
                break;
            case -1:
                return;
            default:
                printf("Invalid option.\n");
                print_helper();
                return;
        }
    }
}

/*
Print out device information.
*/
void print_dev_info(struct args *args){
    printf("**********************************************\n");
    printf("Recv Config Information:\n");
    printf("    device_id: %d\n", args->device_id);
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
    if(args->use_gpu)
        printf("    use_gpu: %d, gpu_id: %d\n", args->use_gpu, args->gpu_id);
    printf("    disable_recv: %d\n", args->disable_recv);
    printf("**********************************************\n");
}