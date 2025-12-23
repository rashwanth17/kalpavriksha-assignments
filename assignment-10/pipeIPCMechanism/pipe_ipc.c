#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void sort(int arr[], int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (arr[i] > arr[j])
            {
                int t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
}

int main()
{
    int fd[2];
    pipe(fd);

    int arr[5] = {9, 7, 5, 3, 1};

    printf("Before Sorting: ");
    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");

    if (fork() == 0)
    {
        read(fd[0], arr, sizeof(arr));
        sort(arr, 5);
        write(fd[1], arr, sizeof(arr));
        exit(0);
    }
    write(fd[1], arr, sizeof(arr));
    wait(NULL);
    read(fd[0], arr, sizeof(arr));

    printf("After Sorting: ");
    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}