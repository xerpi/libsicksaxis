#include "sicksaxis.h"
#include <gccore.h>


static int _ss_heap_id = -1;
static int _ss_inited = 0;
static int _ss_dev_id_list[SS_MAX_DEV] = {0};

static int _ss_dev_id_list_exists(int id);
static int _ss_dev_id_list_add(int id);

int ss_init()
{
    if (!_ss_inited) {
        _ss_heap_id = iosCreateHeap(SS_HEAP_SIZE);
        _ss_inited = 1;
    }
    return 1;
}


struct ss_device *ss_open()
{
    if (!_ss_inited) return NULL;
    
    usb_device_entry dev_entry[8];
    unsigned char dev_count;
    if (USB_GetDeviceList(dev_entry, 8, USB_CLASS_HID, &dev_count) < 0) {
        return NULL;
    }
    
    if (dev_count == 0) return NULL;
    
    int i;
    for (i = 0; i < dev_count; ++i) {
        if (!_ss_dev_id_list_exists(dev_entry[i].device_id)) {
            _ss_dev_id_list_add(dev_entry[i].device_id);
            
            int fd;
            if (USB_OpenDevice(dev_entry[i].device_id, SS_VENDOR_ID, SS_PRODUCT_ID, &fd)< 0) {
                return NULL;
            }
            
            struct ss_device *dev = iosAlloc(_ss_heap_id, sizeof(struct ss_device));
            dev->device_id = dev_entry[i].device_id;
            dev->fd = fd;
            
            return dev;
        }
    }
    
    return NULL;
}

int ss_close(struct ss_device *dev)
{
    if (dev) {
        IOS_Close(dev->fd);
        iosFree(_ss_heap_id, dev);
        dev = NULL;
    }
    return 1;
}




static int _ss_dev_id_list_exists(int id)
{
    int i;
    for (i = 0; i < SS_MAX_DEV; ++i) {
        if (_ss_dev_id_list[i] == id) return 1;
    }
    return 0;
}

static int _ss_dev_id_list_add(int id)
{
    int i;
    for (i = 0; i < SS_MAX_DEV; ++i) {
        if (_ss_dev_id_list[i] == 0) {
           _ss_dev_id_list[i] = id;
           return 1; 
        }
    }
    return 0; 
}

