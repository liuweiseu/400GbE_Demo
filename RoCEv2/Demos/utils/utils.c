
#include<stdint.h>
#include "utils.h"

/*
Initialize the recv arguments.
*/
void init_recv_args(struct recv_args *args)
{
    memset(args, 0, sizeof(struct recv_args));
}
/*
Parsse the command line arguments for recv.
*/
void parse_recv_args(struct recv_args *args, int argc, char *argv[])
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
Print out recv information.
*/
void print_recv_info(struct recv_args *args){
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

/*
Free the recv arguments.
*/
void free_recv_args(struct recv_args *args)
{
    // nothing to free for now
    return;
}

/*
Initialize the send arguments.
*/
void init_send_args(struct send_args *args)
{
    memset(args, 0, sizeof(struct send_args));
    // by default, send one stream
    args->streams = 1;
    args->pkt_info = (struct pkt_info *)malloc(args->streams * sizeof(struct pkt_info));
    // by default, send one group of packet
    args->npkt_grp = 1;
    // by default, send 512 packets per group
    args->npkt_per_grp = 512;
    args->inf = 0;
}

/*
Parsse the command line arguments for send.
*/
void parse_send_args(struct send_args *args, int argc, char *argv[])
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
        {.name = "streams", .has_arg = required_argument, .flag = NULL, .val = 262},
        {.name = "inf", .has_arg = no_argument, .flag = NULL, .val = 263},
        {.name = "help", .has_arg = no_argument, .flag = NULL, .val = 'h'},
        {0, 0, 0, 0}
    };
    char *tmp_optarg;
    while(1)
    {
        //c = getopt_long(argc, argv, "S:D:s:d:p:P:gh", long_options, &long_option_index);
        c = getopt_long(argc, argv, "d:g:n:N:h", long_options, &long_option_index);
        switch (c)
        {
            case 'd':
                sscanf(optarg, "%hhd", &args->device_id);
                break;
            case 'n':
                sscanf(optarg, "%d", &args->npkt_per_grp);
                break;
            case 'N':
                sscanf(optarg, "%d", &args->npkt_grp);
                break;
            case 256:
                tmp_optarg = args->streams > 1 ? strtok(optarg, ",") : optarg;
                for(int i = 0; i < args->streams; i++)
                {   
                    sscanf(tmp_optarg, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
                                &args->pkt_info[i].src_mac[0], 
                                &args->pkt_info[i].src_mac[1],
                                &args->pkt_info[i].src_mac[2],
                                &args->pkt_info[i].src_mac[3],
                                &args->pkt_info[i].src_mac[4],
                                &args->pkt_info[i].src_mac[5]);
                    if(args->streams > 1)tmp_optarg = strtok(NULL, ",");
                }
                break;
            case 257:
                tmp_optarg = args->streams > 1 ? strtok(optarg, ",") : optarg;
                for(int i = 0; i < args->streams; i++)
                {
                    sscanf(tmp_optarg, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
                                &args->pkt_info[i].dst_mac[0],
                                &args->pkt_info[i].dst_mac[1],
                                &args->pkt_info[i].dst_mac[2],
                                &args->pkt_info[i].dst_mac[3],
                                &args->pkt_info[i].dst_mac[4],
                                &args->pkt_info[i].dst_mac[5]);
                    if(args->streams > 1)tmp_optarg = strtok(NULL, ",");
                }
                break;
            case 258: 
                tmp_optarg = args->streams > 1 ? strtok(optarg, ",") : optarg;
                for(int i = 0; i < args->streams; i++)
                {
                    sscanf(tmp_optarg, "%hhd.%hhd.%hhd.%hhd", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);
                    args->pkt_info[i].src_ip = (tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];
                    if(args->streams > 1)tmp_optarg = strtok(NULL, ",");
                }
                break;
            case 259:
                tmp_optarg = args->streams > 1 ? strtok(optarg, ",") : optarg;
                for(int i = 0; i < args->streams; i++)
                {
                    sscanf(tmp_optarg, "%hhd.%hhd.%hhd.%hhd", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);
                    args->pkt_info[i].dst_ip = (tmp[3] << 24) | (tmp[2] << 16) | (tmp[1] << 8) | tmp[0];
                    if(args->streams > 1)tmp_optarg = strtok(NULL, ",");
                }
                break;
            case 260:
                tmp_optarg = args->streams > 1 ? strtok(optarg, ",") : optarg;
                for(int i = 0; i < args->streams; i++)
                {
                    sscanf(tmp_optarg, "%hd", &args->pkt_info[i].src_port);
                    if(args->streams > 1)tmp_optarg = strtok(NULL, ",");
                }
                break;
            case 261:
                tmp_optarg = args->streams > 1 ? strtok(optarg, ",") : optarg;
                for(int i = 0; i < args->streams; i++)
                {
                    sscanf(tmp_optarg, "%hd", &args->pkt_info[i].dst_port);
                    if(args->streams > 1)tmp_optarg = strtok(NULL, ",");
                }
                break;
            case 262:
                sscanf(optarg, "%hhd", &args->streams);
                // free the old pkt_info
                free(args->pkt_info);
                args->pkt_info = (struct pkt_info *)malloc(args->streams * sizeof(struct pkt_info));
                break;
            case 263:
                args->inf = 1;
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
Print out send information.
*/
void print_send_info(struct send_args *args){
    printf("**********************************************\n");
    printf("Send Config Information:\n");
    printf("    device_id: %d\n", args->device_id);
    for(int i = 0; i < args->streams; i++)
    {
        printf("    Stream %d:\n", i);
        printf("        src_mac: %02x:%02x:%02x:%02x:%02x:%02x\n", 
                        args->pkt_info[i].src_mac[0], 
                        args->pkt_info[i].src_mac[1], 
                        args->pkt_info[i].src_mac[2], 
                        args->pkt_info[i].src_mac[3], 
                        args->pkt_info[i].src_mac[4], 
                        args->pkt_info[i].src_mac[5]);
        printf("        dst_mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
                        args->pkt_info[i].dst_mac[0],
                        args->pkt_info[i].dst_mac[1],
                        args->pkt_info[i].dst_mac[2],
                        args->pkt_info[i].dst_mac[3],
                        args->pkt_info[i].dst_mac[4],
                        args->pkt_info[i].dst_mac[5]);
        uint8_t tmp[4];
        tmp[3] = (args->pkt_info[i].src_ip >> 24) & 0xff;
        tmp[2] = (args->pkt_info[i].src_ip >> 16) & 0xff;
        tmp[1] = (args->pkt_info[i].src_ip >> 8) & 0xff;
        tmp[0] = args->pkt_info[i].src_ip & 0xff;
        printf("        src_ip: %d.%d.%d.%d\n", tmp[0], tmp[1], tmp[2], tmp[3]);
        tmp[3] = (args->pkt_info[i].dst_ip >> 24) & 0xff;
        tmp[2] = (args->pkt_info[i].dst_ip >> 16) & 0xff;
        tmp[1] = (args->pkt_info[i].dst_ip >> 8) & 0xff;
        tmp[0] = args->pkt_info[i].dst_ip & 0xff;
        printf("        dst_ip: %d.%d.%d.%d\n", tmp[0], tmp[1], tmp[2], tmp[3]);
        printf("        src_port: %d\n", args->pkt_info[i].src_port);
        printf("        dst_port: %d\n", args->pkt_info[i].dst_port);
    }
    if(args->inf)
        printf("    infinite send...\n");
    else
    {
        printf("    packet number per group: %d\n", args->npkt_per_grp);
        printf("    packet group number: %d\n", args->npkt_grp);
    }
    printf("**********************************************\n");
}

/*
Free the send arguments.
*/
void free_send_args(struct send_args *args)
{
    if(args->pkt_info)
        free(args->pkt_info);
    return;
}