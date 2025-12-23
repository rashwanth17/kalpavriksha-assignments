#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <netinet/in.h>

pthread_mutex_t lock;

void *handleClient(void *arg)
{
    int sock = *(int *)arg;
    int choice, amount;
    FILE *fp;

    pthread_mutex_lock(&lock);
    fp = fopen("accountDB.txt", "r");
    fscanf(fp, "%d", &amount);
    fclose(fp);
    pthread_mutex_unlock(&lock);

    read(sock, &choice, sizeof(choice));

    pthread_mutex_lock(&lock);
    fp = fopen("accountDB.txt", "r");
    fscanf(fp, "%d", &amount);
    fclose(fp);

    int result = amount;

    if (choice == 1)
    {
        int w;
        read(sock, &w, sizeof(w));
        if (w <= amount)
            result = amount - w;
    }
    else if (choice == 2)
    {
        int d;
        read(sock, &d, sizeof(d));
        result = amount + d;
    }

    fp = fopen("accountDB.txt", "w");
    fprintf(fp, "%d", result);
    fclose(fp);
    pthread_mutex_unlock(&lock);

    write(sock, &result, sizeof(result));
    close(sock);
    return NULL;
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 5);

    pthread_mutex_init(&lock, NULL);

    while (1)
    {
        int client = accept(server_fd, NULL, NULL);
        pthread_t t;
        pthread_create(&t, NULL, handleClient, &client);
        pthread_detach(t);
    }
}