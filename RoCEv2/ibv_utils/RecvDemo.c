#include <stdio.h>
#include <stdlib.h>

#include "ibv_utils.h"

void main() {
    int device_id = 0;

    // get ib device list
    get_ib_devices();
    // open ib device by id
    int ret = 0;
    ret = open_ib_device(device_id);
    if (ret < 0) {
        printf("Failed to open IB device.\n");
        return;
    }
    printf("Open IB device successfully.\n");

    ret = create_ib_res();
}