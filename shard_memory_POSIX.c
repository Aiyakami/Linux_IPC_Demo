#include<stdio.h>
#include<stdlib.h>

/*
shmget和shm_open用于不同的共享内存操作：
shmget: 这是一个传统的System V共享内存函数，用于创建或获取System V共享内存段。其参数包括键值、大小和权限标志。System V共享内存具有不同的API和管理机制。
shm_open: 这是POSIX共享内存的函数，用于创建或打开POSIX共享内存对象。它需要传递共享内存对象的名称、打开标志（如O_CREAT）和权限标志。POSIX共享内存基于文件系统接口，提供更现代和灵活的共享内存管理。
*/

#define SHM_NAME "/my_shm"
#define SHM_SIZE 1024

int main(){
    //创建共享内存对象
    /*
    SHM_NAME：共享内存对象的名称，必须以 / 开头，例如 /my_shm。这个名称在系统中唯一标识共享内存对象。
    O_CREAT | O_RDWR：
    O_CREAT：如果共享内存对象不存在，则创建它。
    O_RDWR：以读写模式打开共享内存对象。
    0666：共享内存对象的权限模式（类似于文件权限），指定了文件的访问权限。在这里，它表示所有用户都有读写权限。
    */
    int shm_fd = shm_open(SHM_NAME,O_CREAT | O_RDWR, 0666);//api函数位于fcntl.h
    if(shm_fd==-1){
        perror("Failed to create shard memory object");//位于头文件stdio.h
        exit(EXIT_FAILURE);
    }
    //设置共享内存大小
    /*
    f：代表“file”或“file descriptor”。
    truncate：意为“截断”，即调整文件的长度。
    */
    if(ftruncate(shm_fd,SHM_SIZE)==-1){
        perror("ftruncate");
        closse(shm_fd);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    //将共享内存对象映射到进程地址空间中
    /*
    mmap 用于将文件或对象映射到进程的虚拟地址空间(映射起始地址，映射长度，内存保护表示，映射属性，文件描述符，映射偏移量)
    映射起始地址为0表示让操作系统选择合适的地址来进行映射
    */
    void *shm_ptr=mmap(0,SHM_SIZE,PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr==MAP_FAILED){
        perror("mmap");
        close(shm_fd);
        shm_unlink(SHM_NAME);
        exit(EXIT_FAILURE);
    }
    //写入和读取共享内存数据
    const char *message="Hello, shared memory";
    strncpy((char *)shm_ptr, message, SHM_SIZE);
    printf("Read from shared memory: %s\n", (char *)shm_ptr);
    //清理资源
    // Clean up
    if (munmap(shm_ptr, SHM_SIZE) == -1) {
        perror("munmap");
    }
    close(shm_fd);
    if (shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
    }

    return 0;


}