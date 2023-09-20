#include <infiniband/verbs.h>

#include <stdio.h>
#include <stdlib.h>

int main()
{
    struct ibv_device **dev_list;
    int ndev;
    // get the device list
    dev_list = ibv_get_device_list(&ndev);
    if (!dev_list) {
        perror("Failed to get devices list");
        exit(1);
    }
    printf("Number of Device: %d\n", ndev);
    printf("%s\n", dev_list[0]->name);
    printf("%d\n", dev_list[2]->node_type);
    printf("%d\n", dev_list[1]->transport_type);

    struct ibv_device *ib_dev;
    ib_dev = dev_list[2];
    char *dev_name;
    dev_name = ibv_get_device_name(ib_dev);
    printf("dev_name: %s\n",dev_name);

    uint64_t dev_guid;
    dev_guid = ibv_get_device_guid(ib_dev);
    printf("dev_guid: %lx\n", dev_guid);

    return 0;
}