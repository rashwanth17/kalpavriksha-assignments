#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void sort(int numbers[], int count)
{
    for (int index1 = 0; index1 < count - 1; index1++)
        for (int index2 = index1 + 1; index2 < count; index2++)
            if (numbers[index1] > numbers[index2])
            {
                int temp = numbers[index1];
                numbers[index1] = numbers[index2];
                numbers[index2] = temp;
            }
}

int main()
{
    int numbers[5] = {5, 3, 1, 4, 2};
    FILE *file;

    printf("Before Sorting: ");
    for (int index = 0; index < 5; index++)
        printf("%d ", numbers[index]);
    printf("\n");

    file = fopen("data.txt", "w");
    for (int index = 0; index < 5; index++)
        fprintf(file, "%d ", numbers[index]);
    fclose(file);

    if (fork() == 0)
    {
        file = fopen("data.txt", "r");
        for (int index = 0; index < 5; index++)
            fscanf(file, "%d", &numbers[index]);
        fclose(file);

        sort(numbers, 5);

        file = fopen("data.txt", "w");
        for (int index = 0; index < 5; index++)
            fprintf(file, "%d ", numbers[index]);
        fclose(file);
        exit(0);
    }
    wait(NULL);

    file = fopen("data.txt", "r");
    for (int index = 0; index < 5; index++)
        fscanf(file, "%d", &numbers[index]);
    fclose(file);

    printf("After Sorting: ");
    for (int index = 0; index < 5; index++)
        printf("%d ", numbers[index]);
    printf("\n");

    return 0;
}
