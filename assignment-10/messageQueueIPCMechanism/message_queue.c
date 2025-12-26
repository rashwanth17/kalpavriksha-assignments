#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

struct message
{
    long messageType;
    int numbers[5];
};

void sort(int numbers[])
{
    for (int outerIndex = 0; outerIndex < 4; outerIndex++)
        for (int innerIndex = outerIndex + 1; innerIndex < 5; innerIndex++)
            if (numbers[outerIndex] > numbers[innerIndex])
            {
                int temp = numbers[outerIndex];
                numbers[outerIndex] = numbers[innerIndex];
                numbers[innerIndex] = temp;
            }
}

int main()
{
    int messageQueueId = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    struct message messageBuffer;

    int numbers[5] = {8, 6, 4, 2, 0};

    printf("Before Sorting: ");
    for (int index = 0; index < 5; index++)
        printf("%d ", numbers[index]);
    printf("\n");

    if (fork() == 0)
    {
        msgrcv(messageQueueId, &messageBuffer, sizeof(messageBuffer.numbers), 1, 0);
        sort(messageBuffer.numbers);
        messageBuffer.messageType = 2;
        msgsnd(messageQueueId, &messageBuffer, sizeof(messageBuffer.numbers), 0);
        exit(0);
    }

    messageBuffer.messageType = 1;
    for (int index = 0; index < 5; index++)
        messageBuffer.numbers[index] = numbers[index];
    msgsnd(messageQueueId, &messageBuffer, sizeof(messageBuffer.numbers), 0);

    wait(NULL);

    msgrcv(messageQueueId, &messageBuffer, sizeof(messageBuffer.numbers), 2, 0);

    printf("After Sorting: ");
    for (int index = 0; index < 5; index++)
        printf("%d ", messageBuffer.numbers[index]);
    printf("\n");

    msgctl(messageQueueId, IPC_RMID, NULL);
    return 0;
}
