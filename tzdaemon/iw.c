#include <fcntl.h>
#include <sys/ioctl.h>

#include <tz_common.h>

void iwd_connect(int fd) {
    const struct tz_uiwsock_connection con = {.name = "root_task"};

    ioctl(fd, TZIO_UIWSOCK_CONNECT, con);
    ioctl(fd, TZIO_UIWSOCK_WAIT_CONNECTION);
}

void iwd_send(int fd, void* buffer, uint64_t size) {
    struct tz_uiwsock_data data;

    data.buffer = (uint64_t)buffer;
    data.size = size;
    ioctl(fd, TZIO_UIWSOCK_SEND, &data);
}

void iwd_recv(int fd, void* buffer, uint64_t size) {
    struct tz_msghdr msg;

    msg.msgbuf = buffer;
    msg.msglen = size;
    ioctl(fd, TZIO_UIWSOCK_RECV_MSG, &msg);
}

int iwshmem_create_region(uint64_t size, int* id) {
    int fd;
    struct tzio_mem_register mem;

    fd = open("/dev/tziwshmem", O_RDWR);
    mem.size = size;
    *id = ioctl(fd, TZIO_MEM_REGISTER, &mem);
    return fd;
}

void iwd_listen(int fd) {
    const struct tz_uiwsock_connection con = {.name = "iwsock_ts"};

    ioctl(fd, TZIO_UIWSOCK_LISTEN, con);
}

int iwd_accept(int fd) {
    return ioctl(fd, TZIO_UIWSOCK_ACCEPT);
}
