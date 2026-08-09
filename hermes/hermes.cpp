#include <hidl/HidlSupport.h>
#include <unistd.h>

using android::hardware::hidl_vec;

int hwvault_get_cred(unsigned int, hidl_vec<unsigned char>&);

extern "C" {
long nwd_tz_close();
long nwd_tz_open();
}

void cache_cred() {
    hidl_vec<unsigned char> vec;
    hwvault_get_cred(11, vec);
}

int main() {
    nwd_tz_open();
    cache_cred();
    pause();
    nwd_tz_close();
    return 0;
}
