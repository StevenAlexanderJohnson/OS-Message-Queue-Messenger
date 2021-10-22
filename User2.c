#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MESSAGESIZE 128

struct message_s {
    long type;
    char content[MESSAGESIZE];
};

int main(void) {
    printf("Welcome to Fluff3rChat\n");

    pid_t reader = fork();

    if (reader < 0)
    {
        perror("fork failure");
        exit(EXIT_FAILURE);
    }
    else if (reader == 0) // child process
    {
        struct message_s message;
        char *readQueue = "client1.c";
        int readQueueID;
        key_t readKey;
        if((readKey = ftok(readQueue, 1)) == 1)
        {
            printf("Error generating readQueue key.\n");
            perror("ftok");
            exit(1);
        }

        if((readQueueID = msgget(readKey, 0644 | IPC_CREAT)) == -1)
        {
            if(EEXIST == errno)
            {
                if((readQueueID = msgget(readKey, 0644)) == -1)
                {
                    printf("Error opening the readQueue.\n");
                    perror("msgget");
                    exit(1);
                }
            }
            else
            {
                perror("msgget read");
            }
        }
        for(;;)
        {
            if (msgrcv(readQueueID, &message, sizeof message.content, 0, 0) == -1)
            {
                if(EIDRM == errno)
                {
                    printf("Client 1 has left the chat.\n");
                    break;
                }
                else
                {
                    perror("msgrcv");
                    exit(1);
                }
            }
            printf("\r\nClient2: %s\n", message.content);
        }
    }
    else
    {
        struct message_s message;
        int writeQueueID;
        key_t writeKey;

        char *writeQueue = "client2.c";

        if((writeKey = ftok(writeQueue, 1)) == 1)
        {
            printf("Error generating writeQueue key.\n");
            perror("ftok");
            exit(1);
        }

        if((writeQueueID = msgget(writeKey, 0644 | IPC_CREAT)) == -1)
        {
            if(EEXIST == errno)
            {
                if((writeQueueID = msgget(writeKey, 0644)) == -1)
                {
                    printf("Error opening the writeQueue.\n");
                    perror("msgget");
                    exit(1);
                }
            }
            else
            {
                perror("msgget write");
                exit(1);
            }
        }
        strcpy(message.content, "Client 1 has entered the chat.\n\r");
        message.type = 1;
        if(msgsnd(writeQueueID, &message, MESSAGESIZE, 0) == -1)
        {
            perror("msgsnd");
        }
        printf("Type out your message and hit enter. Use ^D to exit.\n");
        while(fgets(message.content, sizeof message.content, stdin) != NULL) 
        {
            int stringLength = strlen(message.content);
            message.type = 1;
            if(message.content[stringLength - 1] == '\n')
            {
                message.content[stringLength - 1] = '\0';
            }
            if(msgsnd(writeQueueID, &message, MESSAGESIZE, 0) == -1)
            {
                perror("msgsnd");
            }
        }

        if(msgctl(writeQueueID, IPC_RMID, NULL) == -1)
        {
            perror("msgctl");
            exit(1);
        }
        wait(NULL);
    }
}