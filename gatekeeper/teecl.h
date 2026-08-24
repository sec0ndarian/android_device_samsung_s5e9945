struct shared_mem {
    uint8_t *ptr;
    size_t size;
    uint32_t mode;
    uint64_t reserved;
};

void TEEC_AllocateSharedMemory(void **, struct shared_mem *);
void TEEC_CloseSession(void **);
void TEEC_FinalizeContext(void **);
void TEEC_InitializeContext(void*, void**);
void TEEC_InvokeCommand(void **, int, void *, int *);
void TEEC_ReleaseSharedMemory(struct shared_mem *);
void TEECS_OpenSession(void **, void **, const char[], void *, size_t, int, void *, void *, int *);
