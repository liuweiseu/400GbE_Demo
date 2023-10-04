#include <stdio.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

int get_interface_info(char *interface_name, uint8_t *mac, uint8_t *ip)
{
    struct ifreq ifr = {0};
    int fd;
    int errsv = 0;


    if(!interface_name) {
        errno = EINVAL;
        return 1;
    }

    
    strncpy(ifr.ifr_name , interface_name , IFNAMSIZ-1);
    ifr.ifr_name[IFNAMSIZ-1] = '\0';

    if((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return 2;
    }

    errsv = errno; // Save errno from ioctl()
    // get mac address
    if((ioctl(fd, SIOCGIFHWADDR, &ifr)))
        return 3;
    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
    //get ip address
    if(ioctl(fd, SIOCGIFADDR, &ifr))
        return 4;
    memcpy(ip, ifr.ifr_addr.sa_data+2, 4);

    close(fd);
    errno = errsv;
    return 0;
}

void main()
{
    char interface_name[] = "ens4np0";
    uint8_t mac[6], ip[4];
    int s = 0;
    s = get_interface_info(interface_name, mac, ip);
    printf("state: %d\n", s);
    printf("mac: %x-%x-%x-%x-%x-%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("ip: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
}