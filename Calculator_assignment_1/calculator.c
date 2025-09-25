#include <stdio.h>
#include<ctype.h>
#define MAX 1000
int stack[MAX];
int top=-1;
int main()
{
    char exp[100];
    printf("Enter the Mathematical Expression: ");
    fgets(exp,sizeof(exp),stdin);
    for(int i=0;exp[i]!='\0';i++)
    {
        if(isspace(exp[i])) continue;
        int num=0;
        int j=i;
        while(isdigit(exp[j]))
        {
            num=num*10+(exp[j]-'0');
            j++;
        }
        if(j!=i) i=j-1;

        if(num!=0 && top!=MAX-1)
        {
            top++;
            stack[top]=num;
        }
        // printf("%c",exp[i]);
    }

    for(int i=0;i<=top;i++)
    {
        printf("%d ",stack[i]);
    }
    return 0;
}