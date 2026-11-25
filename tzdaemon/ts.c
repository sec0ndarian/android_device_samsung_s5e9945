#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iw.h>

char* path[] = {
        (char[]){"/mnt/vendor/efs/tee/srt/00000000-0000-0000-0000-000000000000/0000000000/00000000/"
                 "obj_info"},
        (char[]){"/mnt/vendor/efs/tee/srt/00000000-0000-0000-0000-000000000000/0000000000/00000000/"
                 "data_chunk"},
        (char[]){"/mnt/vendor/efs/tee/srt/00000000-0000-0000-0000-000000000000-0000000000"}};

enum { TS_CMD_WRITE_DATA = 1, TS_CMD_READ_DATA = 3, TS_CMD_READ_OBJ_LIST = 5 };

struct connection {
    int fd;
    struct message* out;
};

struct message {
    int group;
    int cmd;
    int size;
    int flags;
    char buf[];
};

struct header {
    char ta[16];
    int storage;
    char obj[4];
};

struct file {
    int size;
    char data[];
};

void resolve_ids(struct header* head, char* path) {
    char* buf = head->ta;
    char c;

    buf += 10;
    path += 48;
    c = path[12];

    for (int i = 0; i < 6; i++) snprintf(path + 2 * i, 3, "%02x", buf[i]);

    path[12] = c;
    path += 13;

    snprintf(path, 11, "%02u", head->storage);
}

void resolve_obj(char* buf, char* path) {
    const char* gamma = "0123456789abcdef0123456789ABCDEF";

    path[71] = '/';
    path += 72;

    for (int i = 0; i < 4; i++) {
        path[2 * i] = gamma[buf[i] >> 4];
        path[2 * i + 1] = gamma[buf[i] & 0xf];
    }
}

struct message* read_list(struct message* in) {
    int fd;
    int size = sizeof(struct message);
    struct header* head;
    struct message* out;
    struct stat st;

    resolve_ids((struct header*)in->buf, path[2]);
    fd = open(path[2], O_RDONLY);
    fstat(fd, &st);
    size += st.st_size;

    out = malloc(size);
    out->size = size;
    read(fd, out->buf, st.st_size);
    close(fd);
    return out;
}

struct message* read_data(struct message* in) {
    int* chunks;
    int fd[2];
    int size = sizeof(struct message) + sizeof(struct header) + 2 * sizeof(struct file) + 12;
    struct file* file;
    struct header* head;
    struct message* out;
    struct stat st[2];
    void* ret;

    head = (struct header*)in->buf;
    for (int i = 0; i < 2; i++) {
        resolve_ids(head, path[i]);
        resolve_obj(head->obj, path[i]);
        fd[i] = open(path[i], O_RDONLY);
        fstat(fd[i], st + i);
    }

    size += st[0].st_size + st[1].st_size;
    out = malloc(size);
    out->size = size;

    memcpy(out->buf, in->buf, sizeof(struct header));

    file = (struct file*)(out->buf + sizeof(struct header));
    file->size = st[0].st_size;
    read(fd[0], file->data, file->size);

    chunks = (int*)(file->data + file->size + 4);
    *chunks = 1;

    file = (struct file*)(chunks + 2);
    file->size = st[1].st_size;
    read(fd[1], file->data, file->size);

    for (int i = 0; i < 2; i++) close(fd[i]);
    return out;
}

struct message* write_data(struct message* in) {
    int fd[3];
    struct file* file;
    struct header* head;
    struct message* out;

    head = (struct header*)in->buf;
    for (int i = 0; i < 2; i++) {
        resolve_ids(head, path[i]);
        resolve_obj(head->obj, path[i]);
        fd[i] = open(path[i], O_CREAT | O_WRONLY | O_TRUNC, 0600);
    }
    resolve_ids(head, path[2]);
    fd[2] = open(path[2], O_CREAT | O_WRONLY | O_TRUNC, 0600);

    out = malloc(sizeof(struct message));
    out->size = sizeof(struct message);

    file = (struct file*)(in->buf + sizeof(struct header));
    write(fd[0], file->data, file->size);

    file = (struct file*)(file->data + file->size + 12);
    write(fd[1], file->data, file->size);

    file = (struct file*)(file->data + file->size);
    write(fd[2], file, file->size);

    for (int i = 0; i < 3; i++) close(fd[i]);
    return out;
}

void* ts(void* efd) {
    int fd, size, sock, epfd;
    struct connection* con;
    struct epoll_event event;
    struct message *in, *out;

    sock = open("/dev/tziwsock", O_RDWR);
    iwd_listen(sock);

    epfd = epoll_create1(0);
    event.events = EPOLLIN;
    event.data.fd = sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &event);

    event.data.ptr = efd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, *(int*)efd, &event);

    while (true) {
        while (true) {
            epoll_wait(epfd, &event, 1, -1);
            if (event.events == EPOLLIN) {
                if (event.data.fd == sock) {
                    con = calloc(1, sizeof(struct connection));
                    con->fd = iwd_accept(sock);
                    event.data.ptr = con;
                    epoll_ctl(epfd, EPOLL_CTL_ADD, con->fd, &event);
                } else if (event.data.ptr == efd)
                    return NULL;
                else {
                    con = (struct connection*)event.data.ptr;
                    fd = con->fd;
                    out = con->out;

                    if (out != NULL) {
                        free(out);
                    }

                    iwd_recv(fd, &size, 4);
                    in = malloc(size);
                    iwd_recv(fd, in, size);
                    break;
                }
            } else {
                con = (struct connection*)event.data.ptr;
                close(con->fd);
                free(con->out);
                free(con);
            }
        };

        switch (in->cmd) {
            case TS_CMD_WRITE_DATA:
                out = write_data(in);
                break;
            case TS_CMD_READ_DATA:
                out = read_data(in);
                break;
            default:
                out = read_list(in);
        }

        out->group = in->group;
        out->cmd = in->cmd;
        out->flags = 1;
        free(in);

        iwd_send(fd, &out->size, 4);
        iwd_send(fd, out, out->size);
        con->out = out;
    }

    return NULL;
}
