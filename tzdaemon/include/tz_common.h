/*
 * SPDX-FileCopyrightText: Samsung Electronics Co., Ltd
 * SPDX-License-Identifier: GPL-2.0
 */

#define TZ_IOC_MAGIC 'c'
#define TZIO_MEM_REGISTER _IOW(TZ_IOC_MAGIC, 120, struct tzio_mem_register)
#define TZIO_UIWSOCK_CONNECT _IOW(TZ_IOC_MAGIC, 130, int)
#define TZIO_UIWSOCK_WAIT_CONNECTION _IOW(TZ_IOC_MAGIC, 131, int)
#define TZIO_UIWSOCK_SEND _IOW(TZ_IOC_MAGIC, 132, int)
#define TZIO_UIWSOCK_RECV_MSG _IOWR(TZ_IOC_MAGIC, 133, int)
#define TZIO_UIWSOCK_LISTEN _IOWR(TZ_IOC_MAGIC, 134, int)
#define TZIO_UIWSOCK_ACCEPT _IOWR(TZ_IOC_MAGIC, 135, int)
#define TZ_UIWSOCK_MAX_NAME_LENGTH 256

struct tzio_mem_register {
    uint64_t size;  /* Memory region size (in) */
    uint32_t write; /* 1 - rw, 0 - ro */
} __attribute__((__packed__));

struct tz_msghdr {
    char* msgbuf;
    unsigned long msglen;
    char* msg_control;
    unsigned long msg_controllen;
    int msg_flags;
};

struct tz_uiwsock_connection {
    char name[TZ_UIWSOCK_MAX_NAME_LENGTH];
};

struct tz_uiwsock_data {
    uint64_t buffer;
    uint64_t size;
    uint32_t flags;
} __attribute__((__packed__));
