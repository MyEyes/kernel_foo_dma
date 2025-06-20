#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <error.h>
#include <stdlib.h>
#include <linux/udmabuf.h>
#include <linux/dma-heap.h>
#include <sys/ioctl.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "dma_buf_t.h"

static int __dev_fd = -1;
static inline int __get_dev_fd()
{
    if(__dev_fd>=0)
        return __dev_fd;

    __dev_fd = open("/dev/dma_heap/system", O_RDWR);
    if (__dev_fd < 0)
        perror("[-] couldn't open system dma-heap");
    return __dev_fd;
}

dma_buf_t * dma_buf_create(size_t size)
{
    dma_buf_t* new_buf = malloc(sizeof(*new_buf));
    if(!new_buf)
        return NULL;

    struct dma_heap_allocation_data info = {0};
    info.len = size;
    info.fd_flags = O_RDWR;

    int dev_fd = __get_dev_fd();
    
    /* alloc a `page` array of N_PAGES_ALLOC (i.e. 1 page) */
    int dma_fd = ioctl(dev_fd, DMA_HEAP_IOCTL_ALLOC, &info);
    if (dma_fd < 0)
    {
        perror("[-] couldn't create udmabuf");
        free(new_buf);
        return NULL;
    }

    new_buf->buf_fd = info.fd;
    new_buf->size = info.len;
    return new_buf;
}