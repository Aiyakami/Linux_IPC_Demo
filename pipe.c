#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100


/*
共享内存：允许多个进程访问同一块内存区域，效率高，但需要额外的同步机制（如互斥锁）来避免数据竞争和一致性问题。
管道：提供单向的数据流，一端写入数据，另一端读取数据，简单易用，但每次传输的数据量有限，需要系统调用进行读写操作。

共享内存：操作系统为进程分配一块物理内存区域，这块内存区域对所有关联的进程可见。进程直接读写这块内存，无需通过系统调用传输数据，访问速度快，但需要额外的同步机制来协调对共享内存的访问。
管道：通过内核提供的数据缓冲区来实现进程间的通信。写入数据存入内核缓冲区，读取数据从缓冲区中取出。每次读写操作都涉及系统调用，且数据是通过内核处理的，效率相对较低，但设计上自带了一些同步机制。

管道（pipe）本身并不限制单一进程读写的关系；实际上，管道可以同时支持多个进程对其进行读写操作。然而，在实际应用中，管道通常用于一个进程写入数据，另一个进程读取数据。

*/
int main() {
    int pipefd[2];  // 文件描述符数组: pipefd[0]是读端，pipefd[1]是写端
    pid_t pid;
    char write_msg[] = "Hello from parent!";
    char read_msg[BUFFER_SIZE];

    // 创建管道
    if(pipe(pipefd)==-1){
        //perror 自动从 errno 获取错误描述，而 printf 需要你手动提供错误信息。这样可以减少错误消息构造的出错概率。
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // 创建子进程
    pid=fork();
    if(pid==-1){
        perror("fork");
        exit(EXIT_FAILURE);
    }
    //此次有待深入认证
    //fork() 的执行是异步的，所以无法确定父进程和子进程的执行顺序，它们的执行次序由操作系统调度器决定。
    if (pid == 0) {  // 子进程
        // 关闭写端
        close(pipefd[1]);

        // 从管道读取数据
        read(pipefd[0], read_msg, sizeof(read_msg));
        printf("Child process received message: %s\n", read_msg);

        // 关闭读端
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    } else {  // 父进程
        // 关闭读端
        close(pipefd[0]);

        // 向管道写数据
        write(pipefd[1], write_msg, strlen(write_msg) + 1);

        // 关闭写端
        close(pipefd[1]);

        // 等待子进程结束
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}
