#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
    int arr[5] = {5, 3, 1, 4, 2};
    FILE *fp;

    printf("Before Sorting: ");
    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");

    fp = fopen("data.txt", "w");
    for (int i = 0; i < 5; i++)
        fprintf(fp, "%d ", arr[i]);
    fclose(fp);

    if (fork() == 0)
    {
        fp = fopen("data.txt", "r");
        for (int i = 0; i < 5; i++)
            fscanf(fp, "%d", &arr[i]);
        fclose(fp);

        sort(arr, 5);

        fp = fopen("data.txt", "w");
        for (int i = 0; i < 5; i++)
            fprintf(fp, "%d ", arr[i]);
        fclose(fp);
        exit(0);
    }
    wait(NULL);

    fp = fopen("data.txt", "r");
    for (int i = 0; i < 5; i++)
        fscanf(fp, "%d", &arr[i]);
    fclose(fp);

    printf("After Sorting: ");
    for (int i = 0; i < 5; i++)
        printf("%d ", arr[i]);
    printf("\n");

    return 0;
}