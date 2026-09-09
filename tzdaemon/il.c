#include <fcntl.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iw.h>

void load(int* info, char* name) {
    int fd, mfd;
    static char path[49] = "/vendor/tee/";
    struct stat st;
    uint64_t size;
    void* mem;

    memcpy(path + 12, name, 36);
    fd = open(path, O_RDONLY);
    fstat(fd, &st);
    size = st.st_size;

    mfd = iwshmem_create_region(size, info + 1);
    mem = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, mfd, 0);
    close(mfd);
    read(fd, mem, size);
    close(fd);
    info[0] = size;
}

void* il(void* efd) {
    char buf[264] = {[104] = 10};
    int fd, epfd;
    struct epoll_event event;

    fd = open("/dev/tziwsock", O_RDWR);
    iwd_connect(fd);

    iwd_send(fd, buf + 104, 8);
    iwd_recv(fd, buf, 16);

    epfd = epoll_create1(0);
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
    event.data.ptr = efd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, *(int*)efd, &event);

    while (true) {
        epoll_wait(epfd, &event, 1, -1);
        if (event.data.ptr == efd) break;
        iwd_recv(fd, buf, sizeof(buf));
        load((int*)buf + 31, buf + 8);
        buf[104] = 9;
        iwd_send(fd, buf + 104, 160);
    }

    return NULL;
}
