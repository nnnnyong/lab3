#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define BUFSIZE 16
#define QKEY (key_t)0111

struct msgq_data {
    long type;
    char text[BUFSIZE];
};

int main() {
    int qid, len;
    char buf[BUFSIZE];
    struct msgq_data send_data = {1, ""};
    struct msgq_data rcv_data;

    if ((qid = msgget(QKEY, IPC_CREAT | 0666)) == -1) {
        perror("msgget failed");
        exit(1);
    }

    while(1) {
        printf("send message : ");
        fgets(send_data.text, BUFSIZE, stdin);
        send_data.text[strlen(send_data.text) - 1] = '\0';

        if (strcmp(send_data.text, "exit") == 0) {
            printf("exit program\n");
            exit(0);
        }

        if ((len = msgsnd(qid, &send_data, strlen(send_data.text), 0)) == -1) {
            perror("msgsnd failed");
            msgctl(qid, IPC_RMID, NULL);
            exit(1);
        }

        if ((len = msgrcv(qid, &rcv_data, BUFSIZE, 0, 0)) == -1) {
            perror("msgrcv failed");
            exit(1);
        }

        printf("receive message : %s\n", rcv_data.text);
    }
}