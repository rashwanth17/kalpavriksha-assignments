#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

void sort(int numbers[], int size)
{
    for (int outerIndex = 0; outerIndex < size - 1; outerIndex++)
        for (int innerIndex = outerIndex + 1; innerIndex < size; innerIndex++)
            if (numbers[outerIndex] > numbers[innerIndex])
            {
                int temp = numbers[outerIndex];
                numbers[outerIndex] = numbers[innerIndex];
                numbers[innerIndex] = temp;
            }
}

int main()
{
    int pipeFd[2];
    pipe(pipeFd);

    int numbers[5] = {9, 7, 5, 3, 1};

    printf("Before Sorting: ");
    for (int index = 0; index < 5; index++)
        printf("%d ", numbers[index]);
    printf("\n");

    if (fork() == 0)
    {
        read(pipeFd[0], numbers, sizeof(numbers));
        sort(numbers, 5);
        write(pipeFd[1], numbers, sizeof(numbers));
        exit(0);
    }
    write(pipeFd[1], numbers, sizeof(numbers));
    wait(NULL);
    read(pipeFd[0], numbers, sizeof(numbers));

    printf("After Sorting: ");
    for (int index = 0; index < 5; index++)
        printf("%d ", numbers[index]);
    printf("\n");

    return 0;
}
