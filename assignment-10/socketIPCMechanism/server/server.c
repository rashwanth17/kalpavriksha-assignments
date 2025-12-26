#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <netinet/in.h>

pthread_mutex_t mutexLock;

void *handleClient(void *arg)
{
    int clientSocket = *(int *)arg;
    int operationChoice, accountBalance;
    FILE *filePointer;

    pthread_mutex_lock(&mutexLock);
    filePointer = fopen("accountDB.txt", "r");
    fscanf(filePointer, "%d", &accountBalance);
    fclose(filePointer);
    pthread_mutex_unlock(&mutexLock);

    read(clientSocket, &operationChoice, sizeof(operationChoice));

    pthread_mutex_lock(&mutexLock);
    filePointer = fopen("accountDB.txt", "r");
    fscanf(filePointer, "%d", &accountBalance);
    fclose(filePointer);

    int updatedBalance = accountBalance;

    if (operationChoice == 1)
    {
        int withdrawAmount;
        read(clientSocket, &withdrawAmount, sizeof(withdrawAmount));
        if (withdrawAmount <= accountBalance)
            updatedBalance = accountBalance - withdrawAmount;
    }
    else if (operationChoice == 2)
    {
        int depositAmount;
        read(clientSocket, &depositAmount, sizeof(depositAmount));
        updatedBalance = accountBalance + depositAmount;
    }

    filePointer = fopen("accountDB.txt", "w");
    fprintf(filePointer, "%d", updatedBalance);
    fclose(filePointer);
    pthread_mutex_unlock(&mutexLock);

    write(clientSocket, &updatedBalance, sizeof(updatedBalance));
    close(clientSocket);
    return NULL;
}

int main()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddress = {AF_INET, htons(8080), INADDR_ANY};

    bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    listen(serverSocket, 5);

    pthread_mutex_init(&mutexLock, NULL);

    while (1)
    {
        int clientSocket = accept(serverSocket, NULL, NULL);
        pthread_t clientThread;
        pthread_create(&clientThread, NULL, handleClient, &clientSocket);
        pthread_detach(clientThread);
    }
}
