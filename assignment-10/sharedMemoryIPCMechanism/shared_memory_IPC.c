#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void sort(int *arr)
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
    int shmid = shmget(IPC_PRIVATE, 5 * sizeof(int), 0666 | IPC_CREAT);
    int *arr = (int *)shmat(shmid, NULL, 0);

    int data[5] = {10, 9, 8, 7, 6};

    printf("Before Sorting: ");
    for (int i = 0; i < 5; i++)
    {
        arr[i] = data[i];
        printf("%d ", arr[i]);
    }
    printf("\n");

    if (fork() == 0)
    {
        sort(arr);
        exit(0);
    }
    wait(NULL);

    printf("After Sorting: ");
    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");

    shmdt(arr);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}