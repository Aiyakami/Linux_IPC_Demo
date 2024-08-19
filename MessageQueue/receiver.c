#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define MAX_TEXT 512

struct message {
    long int msg_type;
    char text[MAX_TEXT];
};

int main() {
    int running = 1;
    int msgid;
    struct message msg_data;

    // 连接到消息队列
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }

    while (running) {
        // 接收消息
        if (msgrcv(msgid, (void *)&msg_data, MAX_TEXT, 0, 0) == -1) {
            perror("msgrcv failed");
            exit(EXIT_FAILURE);
        }

        printf("Received message: %s", msg_data.text);

        if (strncmp(msg_data.text, "end", 3) == 0) {
            running = 0;
        }
    }

    // 删除消息队列
    /*
    msgctl 是一个用于控制和管理消息队列的系统调用函数
    IPC_RMID表示删除消息队列，该功能为消息队列的控制函数参数控制
    */
    if (msgctl(msgid, IPC_RMID, 0) == -1) {
        perror("msgctl(IPC_RMID) failed");
        exit(EXIT_FAILURE);
    }

    return 0;
}
