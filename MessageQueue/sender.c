#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>

#define MAX_TEXT 512

/*
消息队列提供更复杂的消息处理机制（如消息优先级），而管道提供简单的流式数据传输。
消息队列机制应该算系统间进程默认通信，跟windows一样，管道比消息队列更适合传输
*/

struct message{
    long int msg_type;
    char text[MAX_TEXT];
}

int main(){
    int running=1;
    struct message msg_data;
    int msgid;
    char buffer[MAX_TEXT];
    //创建消息队列，返回消息队列的ID
    /*
    msgget 是一个用于创建或访问消息队列的系统调用函数 原型：int msgget(key_t key, int msgflg);
    key：唯一标识，msgflg：创建和访问权限
    */
    msgid=msgget((key_t)1234,0666|IPC_CREAT);//没有这个消息队列就创建
    if (msgid == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }
    while(running){
        printf("Enter some text: ");
        //char *fgets(char *str, int n, FILE *stream); stream: FILE* 类型的文件指针 stdin：标准输入流，通常是键盘输入。文件流：使用 fopen 打开的文件指针。
        fgets(buffer,MAX_TEXT,stdin);
        msg_data.msg_type=1;
        //strncpy这里的n指定复制长度
        strncpy(msg_data.text,buffer,MAX_TEXT);
        //发送到消息队列
        /*
        msgsnd向消息队列发送消息的系统调用函数 int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
        msgflg控制函数的行为 0: 表示使用默认行为。如果消息队列已满，进程会阻塞等待空间可用。
        */
        if(msgsnd(msgid,(void*)&msg_data,MAX_TEXT,0)==-1){
            perror("msgsnd faild");
            exit(EXIT_FAILURE);
        }
        //如果输入end则推出程序
        if(strncmp(buffer,"end",3)==0){
            running=0;
        }
        
    }
    return 0;
}

