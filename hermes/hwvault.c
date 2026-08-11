#include <fcntl.h>
#include <shctrl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <teecl.h>
#include <unistd.h>

#define HV_TZ_CMD_GET_CRED   0x271f
#define HWVAULT_CRED_SLOT    11
#define SHARED_MEM_SIZE      140
#define SHARED_MEM_SIZE_IN   16
#define SHARED_MEM_SIZE_OUT  124
#define TRUSTLET_PATH        "/vendor/tee/00000000-0000-0000-0000-487641557457"
#define TRUSTLET_ID          "\0\0\0\0\0\0\0\0\0\0HvAUtW"

void *context, *session;

struct shared_mem in = {
    .size = SHARED_MEM_SIZE_IN,
    .mode = 1
};

struct shared_mem out = {
    .size = SHARED_MEM_SIZE_OUT,
    .mode = 2
};

void nwd_tz_open() {
    int fd;
    size_t size;
    struct stat st;
    void* data;

    TEEC_InitializeContext(NULL, &context);

    fd = open(TRUSTLET_PATH, O_RDONLY);
    fstat(fd, &st);
    size = st.st_size;
    data = malloc(size);
    read(fd, data, size);
    close(fd);

    TEECS_OpenSession(&context, &session, TRUSTLET_ID, data, size, 0, NULL, NULL, NULL);
    free(data);
}

void nwd_tz_close() {
    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);
}

void nwd_tz_run_cmd() {
    struct message msg = {
        .type = 0xcc00000000,
        .in = &in,
        .out = &out
    };

    TEEC_InvokeCommand(&session, 0, &msg, NULL);
}

void hv_run_cmd() {
    hwvault_ssp_init();
    nwd_tz_run_cmd();
    hwvault_ssp_exit();
}

void hwvault_get_cred() {
    in.ptr = malloc(SHARED_MEM_SIZE);
    TEEC_RegisterSharedMemory(&context, &in);
    out.ptr = in.ptr + in.size;
    TEEC_RegisterSharedMemory(&context, &out);

    in.ptr[0] = HV_TZ_CMD_GET_CRED;
    in.ptr[1] = 8;
    in.ptr[2] = 0x1000005;
    in.ptr[3] = HWVAULT_CRED_SLOT;
    hv_run_cmd();

    TEEC_ReleaseSharedMemory(&in);
    TEEC_ReleaseSharedMemory(&out);
    free(in.ptr);
}
