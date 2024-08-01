#ifndef __IBV_UTILS_H__
#define __IBV_UTILS_H__

#define MAX_DEV_NUM 2

int get_ib_devices();
int open_ib_device(struct ibv_context **context, int device_id);
int open_ib_device_by_name(struct ibv_context **context, const char *device_name);
int create_ib_res();
int register_memory();
int init_ib_res();
int ib_send();
int ib_recv();
int destroy_ib_qps();
int close_ib_device(struct ibv_context *context);

#endif