#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void sort(int *numbers)
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
    int sharedMemoryId = shmget(IPC_PRIVATE, 5 * sizeof(int), 0666 | IPC_CREAT);
    int *sharedArray = (int *)shmat(sharedMemoryId, NULL, 0);

    int inputData[5] = {10, 9, 8, 7, 6};

    printf("Before Sorting: ");
    for (int index = 0; index < 5; index++)
    {
        sharedArray[index] = inputData[index];
        printf("%d ", sharedArray[index]);
    }
    printf("\n");

    if (fork() == 0)
    {
        sort(sharedArray);
        exit(0);
    }
    wait(NULL);

    printf("After Sorting: ");
    for (int index = 0; index < 5; index++)
        printf("%d ", sharedArray[index]);
    printf("\n");

    shmdt(sharedArray);
    shmctl(sharedMemoryId, IPC_RMID, NULL);
    return 0;
}
