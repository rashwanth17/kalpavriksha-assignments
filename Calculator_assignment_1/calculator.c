#include <stdio.h>
#include<ctype.h>
int main()
{
    char exp[100];
    printf("Enter the Mathematical Expression: ");
    fgets(exp,sizeof(exp),stdin);
    for(int i=0;exp[i]!='\0';i++)
    {
        if(isspace(exp[i])) continue;
        printf("%c",exp[i]);
    }
    return 0;
}