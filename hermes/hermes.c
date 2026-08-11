#include <unistd.h>

void hwvault_get_cred(unsigned int);
void nwd_tz_close();
void nwd_tz_open();

int main() {
    nwd_tz_open();
    hwvault_get_cred(11);
    pause();
    nwd_tz_close();
    return 0;
}
