#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {AF_INET, htons(8080), INADDR_ANY};

    connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    int choice, amount, result;

    printf("1.Withdraw\n2.Deposit\n3.Display\nEnter choice: ");
    scanf("%d", &choice);
    write(sock, &choice, sizeof(choice));

    if (choice == 1 || choice == 2)
    {
        printf("Enter amount: ");
        scanf("%d", &amount);
        write(sock, &amount, sizeof(amount));
    }

    read(sock, &result, sizeof(result));
    printf("Current Balance: %d\n", result);

    close(sock);
    return 0;
}