#include <stdio.h>
#include<ctype.h>
#include<stdlib.h>
#define MAX 1000
int stack[MAX];
int top=-1;
char op[MAX];
int ot=-1;



int operation(int x,int y,char ch)
{
    if(ch=='+') return x+y;
    else if(ch=='-') return x-y;
    else if(ch=='*') return x*y;
    else{
        if(y==0)
        {
            printf("cannot divide by 0");
            exit(1);
        }
        return x/y;
    }
}

int precedence(char ch)
{
    if(ch=='+' || ch=='-') return 1;
    if(ch=='*' || ch=='/') return 2;
    return 0;
}

void process()
{
    char c=op[ot--];
    int x=stack[top--];
    int y=stack[top--];
    int ans=operation(y,x,c);
    stack[++top]=ans;

}

int main()
{
    char exp[100];
    printf("Enter the Mathematical Expression: ");
    fgets(exp,sizeof(exp),stdin);
    for(int i=0;exp[i]!='\0';i++)
    {
        if(isspace(exp[i])) continue;
        if(i!=0 && (exp[i-1]=='+' || exp[i-1]=='-' || exp[i-1]=='*' || exp[i-1]=='/')&&(exp[i]=='+' || exp[i]=='-' || exp[i]=='*' || exp[i]=='/'))
        {
            printf("invalid expression");
            return 1;
        }
        if(isdigit(exp[i]))
        {

            int num=0;
            // int j=i;
            while(isdigit(exp[i]))
            {
                num=num*10+(exp[i]-'0');
                i++;
            }
            // if(j!=i) i=j-1;
            i--;
            
            if(top!=MAX-1)
            {
                top++;
                stack[top]=num;
            }
        }
        else if(exp[i]=='+' || exp[i]=='-' || exp[i]=='*' || exp[i]=='/')
        {
            while(ot!=-1 && precedence(op[ot])>=precedence(exp[i]))
            {
                process();
            }
            op[++ot]=exp[i];
        }
        else
        {
            printf("invalid expression");
            return 1;
        }
        // printf("%c",exp[i]);
    }
    while(ot!=-1)
    {
        // char c=op[ot--];
        // int x=stack[top--];
        // int y=stack[top--];
        // int ans=operation(y,x,c);
        // stack[++top]=ans;
        process();
    }

    // for(int i=0;i<=top;i++)
    // {
    //     printf("%d ",stack[i]);
    // }
    printf("%d",stack[0]);
    return 0;
}