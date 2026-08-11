#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

struct shared_mem {
    int *ptr;
    size_t size;
    int mode;
    char reserved[12];
};

struct message {
    long type;
    struct shared_mem *in;
    char reserved[16];
    struct shared_mem *out;
};

void hwvault_ssp_exit();
void hwvault_ssp_init();
void TEEC_CloseSession(void **);
void TEEC_FinalizeContext(void **);
void TEEC_InitializeContext(void*, void**);
void TEEC_InvokeCommand(void **, int, struct message *, int *);
void TEEC_RegisterSharedMemory(void **, struct shared_mem *);
void TEEC_ReleaseSharedMemory(struct shared_mem *);
void TEECS_OpenSession(void **, void **, char[], void *, size_t, int, void *, void *, int *);
void TEECS_SetCryptoClk(void **, int);

#define HV_TZ_CMD_GET_CRED  0x271f
#define SHARED_MEM_SIZE     81924
#define TRUSTLET_PATH       "/vendor/tee/00000000-0000-0000-0000-487641557457"
#define TRUSTLET_ID         "\0\0\0\0\0\0\0\0\0\0HvAUtW"

int shared_mem_in[SHARED_MEM_SIZE / 4];
int shared_mem_out[SHARED_MEM_SIZE / 4];
void *context, *session;

struct shared_mem in = {
    .ptr = shared_mem_in,
    .size = SHARED_MEM_SIZE,
    .mode = 1
};

struct shared_mem out = {
    .ptr = shared_mem_out,
    .size = SHARED_MEM_SIZE,
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

    TEEC_RegisterSharedMemory(&context, &in);
    TEEC_RegisterSharedMemory(&context, &out);
}

void nwd_tz_close() {
    TEEC_ReleaseSharedMemory(&in);
    TEEC_ReleaseSharedMemory(&out);
    TEEC_CloseSession(&session);
    TEEC_FinalizeContext(&context);
}

void nwd_tz_run_cmd() {
    struct message msg = {
        .type = 0xcc00000000,
        .in = &in,
        .out = &out
    };

    TEECS_SetCryptoClk(&context, 1);
    TEEC_InvokeCommand(&session, 0, &msg, NULL);
    TEECS_SetCryptoClk(&context, 0);
}

void hv_run_cmd() {
    hwvault_ssp_init();
    nwd_tz_run_cmd();
    hwvault_ssp_exit();
}

void hwvault_get_cred(unsigned int slot) {
    shared_mem_in[0] = HV_TZ_CMD_GET_CRED;
    shared_mem_in[1] = 8;
    shared_mem_in[2] = 0x1000005;
    shared_mem_in[3] = slot;
    hv_run_cmd();
}
