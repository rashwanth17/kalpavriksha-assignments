#include <stdio.h>
#include<ctype.h>
int main()
{
    char exp[100];
    printf("Enter the Mathematical Expression: ");
    fgets(exp,sizeof(exp),stdin);
    printf("%s",exp);
    return 0;
}