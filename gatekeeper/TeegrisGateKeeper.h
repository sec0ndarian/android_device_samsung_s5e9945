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

struct __attribute__((packed)) info {
    uint64_t challenge;
    uint32_t auth_token_length;
    bool request_reenroll;
    gatekeeper_error_t error;
    uint32_t retry_timeout;
    uint32_t user_id;
};

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

        message.buffer = &shmem_sz_cmd;
        message.enrolled_password_handle = &shmem_sz_pwd_handle;
        message.provided_password = &shmem_sz_pwd;

        inf = (struct info *)(shmem_sz_cmd.ptr + 0x400);
    }

    ~TeegrisGateKeeper() {
        TEEC_ReleaseSharedMemory(&shmem_sz_cmd);
        TEEC_ReleaseSharedMemory(&shmem_sz_pwd_handle);
        TEEC_ReleaseSharedMemory(&shmem_sz_pwd);
        TEEC_CloseSession(&session);
        TEEC_FinalizeContext(&context);
    }

    void Enroll(const EnrollRequest &request __unused, EnrollResponse *response __unused) {}

    void Verify(const VerifyRequest &request, VerifyResponse *response) {
        uint8_t *auth_token;

        inf->challenge = request.challenge;
        inf->auth_token_length = 0x400;
        inf->user_id = request.user_id;

        message.enrolled_password_handle_length = request.password_handle.size();
        message.provided_password_length = request.provided_password.size();
        memcpy(shmem_sz_pwd_handle.ptr, request.password_handle.Data<uint8_t>(), message.enrolled_password_handle_length);
        memcpy(shmem_sz_pwd.ptr, request.provided_password.Data<uint8_t>(), message.provided_password_length);

        TEEC_InvokeCommand(&session, 0x7e, &message, NULL);
        response->error = inf->error;

        if (response->error == ERROR_NONE) {
            auth_token = (uint8_t *)malloc(inf->auth_token_length);
            memcpy(auth_token, shmem_sz_cmd.ptr, inf->auth_token_length);
            response->SetVerificationToken(SizedBuffer(auth_token, inf->auth_token_length));
            response->request_reenroll = inf->request_reenroll;
        } else if (response->error == ERROR_RETRY) {
            response->retry_timeout = inf->retry_timeout;
        }
    }

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

    struct {
        uint64_t type = 0xddf00000000;
        struct shared_mem *buffer;
        uint32_t cmd = 0x419;
        uint32_t reserved0[3];
        struct shared_mem *enrolled_password_handle;
        uint32_t enrolled_password_handle_length;
        uint32_t reserved1[3];
        struct shared_mem *provided_password;
        uint32_t provided_password_length;
        uint32_t reserved2[3];
    } message;

    struct info *inf;
};

}  // namespace gatekeeper
