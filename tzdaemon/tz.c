#include <pthread.h>
#include <sys/eventfd.h>
#include <unistd.h>

extern void il(void*);
extern void ts(void*);

int main() {
    long efd;
    pthread_t thread;

    efd = eventfd(0, 0);
    pthread_create(&thread, NULL, il, &efd);
    pthread_create(&thread, NULL, ts, &efd);
    pause();
    write(efd, &efd, 8);
    return -1;
}
