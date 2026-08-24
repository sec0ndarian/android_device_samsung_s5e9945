/*
 * Copyright 2015 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

extern "C" {
#include <teecl.h>
}

#define TRUSTLET_PATH  "/vendor/tee/00000000-0000-0000-0000-474154454b45"
#define TRUSTLET_ID    "\0\0\0\0\0\0\0\0\0\0GATEKE"

namespace gatekeeper {

class TeegrisGateKeeper {
  public:
    TeegrisGateKeeper() {
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

        TEEC_AllocateSharedMemory(&context, &shmem_sz_cmd);
        TEEC_AllocateSharedMemory(&context, &shmem_sz_pwd_handle);
        TEEC_AllocateSharedMemory(&context, &shmem_sz_pwd);
    }

    ~TeegrisGateKeeper() {
        TEEC_ReleaseSharedMemory(&shmem_sz_cmd);
        TEEC_ReleaseSharedMemory(&shmem_sz_pwd_handle);
        TEEC_ReleaseSharedMemory(&shmem_sz_pwd);
        TEEC_CloseSession(&session);
        TEEC_FinalizeContext(&context);
    }

    void Enroll(const EnrollRequest &request __unused, EnrollResponse *response __unused) {}
    void Verify(const VerifyRequest &request __unused, VerifyResponse *response __unused) {}

  private:
    void *context, *session;

    struct shared_mem shmem_sz_cmd = {
        .size = 0x438,
        .mode = 3
    };

    struct shared_mem shmem_sz_pwd_handle = {
        .size = 0x40,
        .mode = 1
    };

    struct shared_mem shmem_sz_pwd = {
        .size = 0x80,
        .mode = 1
    };
};

}  // namespace gatekeeper
