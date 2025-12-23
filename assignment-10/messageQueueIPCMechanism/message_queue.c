#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>

struct msg
{
    long type;
    int arr[5];
};

void sort(int arr[])
{
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 5; j++)
            if (arr[i] > arr[j])
            {
                int t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
}

int main()
{
    int msgid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    struct msg m;

    int arr[5] = {8, 6, 4, 2, 0};

    printf("Before Sorting: ");
    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");

    if (fork() == 0)
    {
        msgrcv(msgid, &m, sizeof(m.arr), 1, 0);
        sort(m.arr);
        m.type = 2;
        msgsnd(msgid, &m, sizeof(m.arr), 0);
        exit(0);
    }

    m.type = 1;
    for (int i = 0; i < 5; i++)
        m.arr[i] = arr[i];
    msgsnd(msgid, &m, sizeof(m.arr), 0);

    wait(NULL);

    msgrcv(msgid, &m, sizeof(m.arr), 2, 0);

    printf("After Sorting: ");
    for (int i = 0; i < 5; i++)
        printf("%d ", m.arr[i]);
    printf("\n");

    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}