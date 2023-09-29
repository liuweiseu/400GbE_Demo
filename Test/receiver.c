#include <infiniband/verbs.h>

//#include <infiniband/verbs_exp.h>

#include <stdio.h>

#include <unistd.h>

#include <string.h>

#include <stdlib.h>
#include <poll.h> 
#include <errno.h>

#define PORT_NUM 1

 

#define ENTRY_SIZE 9000 /* The maximum size of each received packet - set to jumbo frame */

#define RQ_NUM_DESC 512 /* The maximum receive ring length without processing */

 

/* The MAC we are listening to. In case your setup is via a network switch, you may need to change the MAC address to suit the network port MAC */

//#define DEST_MAC {0x00, 0x01, 0x02, 0x03, 0x04, 0x05}

#define SRC_MAC {0xa0, 0x88, 0xc2, 0x0d, 0x5e, 0x28}
#define DEST_MAC {0x94, 0x6d, 0xae, 0xac, 0xf8, 0x38}


int main() {

struct ibv_device **dev_list;

struct ibv_device *ib_dev;

struct ibv_context *context;

struct ibv_pd *pd;

int ret;

 

/* Get the list of offload capable devices */

dev_list = ibv_get_device_list(NULL);

if (!dev_list) {

perror("Failed to get IB devices list");

exit(1);

}

 

/* 1. Get Device */

/* In this example, we will use the first adapter (device) we find on the list (dev_list[0]) . You may change the code in case you have a setup with more than one adapter installed. */

ib_dev = dev_list[3];

const char *dev_name;
dev_name = ibv_get_device_name(ib_dev);
printf("dev_name: %s\n",dev_name);

if (!ib_dev) {

fprintf(stderr, "IB device not found\n");

exit(1);

}

  fprintf(stderr,"%s before opening device errno: %d\n",__FUNCTION__, errno);

/* 2. Get the device context */

/* Get context to device. The context is a descriptor and needed for resource tracking and operations */

context = ibv_open_device(ib_dev);
fprintf(stderr,"%s after opening another device errno: %d\n",__FUNCTION__, errno);

if (!context) {

fprintf(stderr, "Couldn't get context for %s\n",

ibv_get_device_name(ib_dev));

exit(1);

}

 

/* 3. Allocate Protection Domain */

/* Allocate a protection domain to group memory regions (MR) and rings */

pd = ibv_alloc_pd(context);

if (!pd) {

fprintf(stderr, "Couldn't allocate PD\n");

exit(1);

}

 

/* 4. Create Complition Queue (CQ) */

struct ibv_cq *cq;
struct ibv_comp_channel      * recv_cc=NULL;
int ind = 0;
recv_cc = ibv_create_comp_channel(context);
cq = ibv_create_cq(context, RQ_NUM_DESC, NULL, recv_cc, 0);

if (!cq) {

fprintf(stderr, "Couldn't create CQ %d\n", errno);

exit (1);

}

if(ibv_req_notify_cq(cq, 0)) {
    perror("ibv_req_notify_cq");
    return NULL;
  } 

/* 5. Initialize QP */

struct ibv_qp *qp;

struct ibv_qp_init_attr qp_init_attr = {

.qp_context = NULL,

/* report receive completion to cq */

.send_cq = cq,

.recv_cq = cq,

 

.cap = {

/* no send ring */

.max_send_wr = 0,

/* maximum number of packets in ring */

.max_recv_wr = RQ_NUM_DESC,

/* only one pointer per descriptor */

.max_recv_sge = 1,

},

.qp_type = IBV_QPT_RAW_PACKET,

};

 

 

/* 6. Create Queue Pair (QP) - Receive Ring */

qp = ibv_create_qp(pd, &qp_init_attr);

if (!qp) {

fprintf(stderr, "Couldn't create RSS QP\n");

exit(1);

}

printf("qu_num: %d\n",qp->qp_num);

/* 7. Initialize the QP (receive ring) and assign a port */

struct ibv_qp_attr qp_attr;

int qp_flags;

memset(&qp_attr, 0, sizeof(qp_attr));

qp_flags = IBV_QP_STATE | IBV_QP_PORT;

qp_attr.qp_state = IBV_QPS_INIT;

qp_attr.port_num = 1;

ret = ibv_modify_qp(qp, &qp_attr, qp_flags);

if (ret < 0) {

fprintf(stderr, "failed modify qp to init\n");

exit(1);

}

memset(&qp_attr, 0, sizeof(qp_attr));

 

/* 8. Move ring state to ready to receive, this is needed in order to be able to receive packets */

qp_flags = IBV_QP_STATE;

qp_attr.qp_state = IBV_QPS_RTR;

ret = ibv_modify_qp(qp, &qp_attr, qp_flags);

if (ret < 0) {

fprintf(stderr, "failed modify qp to receive\n");

exit(1);

}

 

/* 9. Allocate Memory */

int buf_size = ENTRY_SIZE*RQ_NUM_DESC; /* maximum size of data to be accessed by hardware */

void *buf;
unsigned char *buf_char;
buf = malloc(buf_size);
buf_char = (unsigned char*)buf;
if (!buf) {

fprintf(stderr, "Coudln't allocate memory\n");

exit(1);

}

 

/* 10. Register the user memory so it can be accessed by the HW directly */

struct ibv_mr *mr;

mr = ibv_reg_mr(pd, buf, buf_size, IBV_ACCESS_LOCAL_WRITE);

if (!mr) {

fprintf(stderr, "Couldn't register mr\n");

exit(1);

}

 

/* 11. Attach all buffers to the ring */

int n;

struct ibv_sge sg_entry;

struct ibv_recv_wr wr, *bad_wr;

 

/* pointer to packet buffer size and memory key of each packet buffer */

sg_entry.length = ENTRY_SIZE;

sg_entry.lkey = mr->lkey;

 

/*

* descriptor for receive transaction - details:

* - how many pointers to receive buffers to use

* - if this is a single descriptor or a list (next == NULL single)

*/

wr.num_sge = 1;

wr.sg_list = &sg_entry;

wr.next = NULL;

 

for (n = 0; n < RQ_NUM_DESC; n++) {

/* each descriptor points to max MTU size buffer */

sg_entry.addr = (uint64_t)buf + ENTRY_SIZE*n;

 

/* index of descriptor returned when packet arrives */

wr.wr_id = n;

 

/* post receive buffer to ring */

ibv_post_recv(qp, &wr, &bad_wr);

}

 

/* 12. Register steering rule to intercept packet to DEST_MAC and place packet in ring pointed by ->qp */

struct raw_eth_flow_attr {
struct ibv_flow_attr attr;
struct ibv_flow_spec_eth spec_eth;
} __attribute__((packed)) flow_attr = {
.attr = {
  .comp_mask = 0,
  .type = IBV_FLOW_ATTR_NORMAL,
  .size = sizeof(flow_attr),
  .priority = 0,
  .num_of_specs = 1,
  .port = PORT_NUM,
  .flags = 0,
},
.spec_eth = {
  //.type = IBV_EXP_FLOW_SPEC_ETH,
  .type = IBV_FLOW_SPEC_ETH,
  .size = sizeof(struct ibv_flow_spec_eth),
  //.size = sizeof (struct IBV_FLOW_SPEC_ETH),
  .val = {
    .dst_mac = DEST_MAC,
    .src_mac = SRC_MAC,
    .ether_type = 0,
    .src_mac = SRC_MAC,
    .vlan_tag = 0,
  },
  .mask = {
    .dst_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  .src_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF},
  .ether_type = 0,
  .vlan_tag = 0,
  }
  }
};

 

 

/* 13. Create steering rule */

struct ibv_flow *eth_flow;
eth_flow = ibv_create_flow(qp, &flow_attr.attr);
if (!eth_flow) {
  fprintf(stderr, "Couldn't attach steering flow\n");
  exit(1);
}

 

/* 14. Wait for CQ event upon message received, and print a message */

int msgs_completed;

struct ibv_wc wc;

struct ibv_cq *ev_cq;
  int ev_cq_ctx;

printf("ready to go!\n");

struct pollfd pfd;
int poll_rc = 0;
while(1) {

    // Setup for poll
  pfd.fd = recv_cc->fd;
  pfd.events = POLLIN;
  pfd.revents = 0;

  // poll completion channel's fd with given timeout
  poll_rc = poll(&pfd, 1, 50);
  //printf("poll_rc: %d\n",poll_rc);
  if(poll_rc <= 0) {
    // Timeout or error
    continue;
  }
  

 // Get the completion event
 ret = ibv_get_cq_event(recv_cc ,&ev_cq, (void**)&ev_cq_ctx);
  if(ret) {
    perror("ibv_get_cq_event");
    return NULL;
  }

  // Ack the event
  ibv_ack_cq_events(ev_cq, 1);

  if(ibv_req_notify_cq(cq, 0)) {
    perror("ibv_req_notify_cq");
    return NULL;
  }

/* wait for completion */

msgs_completed = ibv_poll_cq(cq, 1, &wc);

if (msgs_completed > 0) {

/*

* completion includes:

* -status of descriptor

* -index of descriptor completing

* -size of the incoming packets

*/

printf("message %ld received size %d\n", wc.wr_id, wc.byte_len);

sg_entry.addr = (uint64_t)buf + wc.wr_id*ENTRY_SIZE;

wr.wr_id = wc.wr_id;

/* after processed need to post back buffer */

ibv_post_recv(qp, &wr, &bad_wr);

printf("buf0-5: %02x %02x %02x %02x %02x %02x\n", buf_char[0],buf_char[1],buf_char[2],buf_char[3],buf_char[4],buf_char[5]);
} else if (msgs_completed < 0) {

printf("Polling error\n");

exit(1);

}

}

 

printf("We are done\n");

 

return 0;

}
