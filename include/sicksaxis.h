#ifndef _SICKSAXIS_H_
#define _SICKSAXIS_H_


#define DEBUG(x,...) printf(x,__VA_ARGS__)

#define SS_HEAP_SIZE   4096
#define SS_MAX_DEV     8
#define SS_VENDOR_ID   0x054C
#define SS_PRODUCT_ID  0x0268

struct ss_device {
    int device_id;
    int fd;
};

int ss_init();
struct ss_device *ss_open();
int ss_close(struct ss_device *dev);

#endif
