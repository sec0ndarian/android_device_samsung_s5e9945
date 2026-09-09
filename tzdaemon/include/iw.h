extern void iwd_connect(int fd);
extern void iwd_send(int fd, void* buffer, uint64_t size);
extern void iwd_recv(int fd, void* buffer, uint64_t size);
extern int iwshmem_create_region(uint64_t size, int* id);
extern void iwd_listen(int fd);
extern int iwd_accept(int fd);
