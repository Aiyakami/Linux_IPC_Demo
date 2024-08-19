#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#define SHM_SIZE 1024  // 共享内存大小

int main() {
    key_t key = 1234;  // 共享内存的键值
    int shm_id;
    char *shm_ptr;

    // 创建共享内存
    shm_id = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        exit(1);
    }

    // 将共享内存附加到进程地址空间
    shm_ptr = (char *) shmat(shm_id, NULL, 0);
    if (shm_ptr == (char *) -1) {
        perror("shmat failed");
        exit(1);
    }

    // 写数据到共享内存
    strncpy(shm_ptr, "Hello, Shared Memory!", SHM_SIZE);

    // 读取数据
    printf("Data in shared memory: %s\n", shm_ptr);

    // 分离共享内存
    if (shmdt(shm_ptr) == -1) {
        perror("shmdt failed");
        exit(1);
    }

    // 删除共享内存
    if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
        perror("shmctl failed");
        exit(1);
    }

    return 0;
}
