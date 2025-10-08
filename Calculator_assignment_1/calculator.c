#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_SIZE 1000

int numStack[MAX_SIZE];
int numTop=-1;
char opStack[MAX_SIZE];
int opTop=-1;

int performOperation(int operand1,int operand2,char operator) //performs arithmetic operations
{

    switch (operator) 
    {
        case '+': return operand1 + operand2;
        case '-': return operand1 - operand2;
        case '*': return operand1 * operand2;
        case '/':
            if (operand2 == 0) {
                printf("Error: Division by zero is not allowed.\n");
                exit(1);
            }
            return operand1 / operand2;
        default:
            printf("Error: Unknown operator '%c'\n", operator);
            exit(1);
    }
}

int getPrecedence(char operator) //precedence logic
{

    if(operator=='+' || operator=='-') return 1;
    if(operator=='*' || operator=='/') return 2;
    return 0;
}

void processStacks() //handles the stacks
{

    char operator=opStack[opTop--];
    int operand2=numStack[numTop--];
    int operand1=numStack[numTop--];

    int result=performOperation(operand1,operand2,operator);
    numStack[++numTop]=result;

}

// Validates if two consecutive operators exist
int hasConsecutiveOperators(const char *expression, int index) 
{

    char current=expression[index];
    char previous=expression[index-1];

    return ((previous=='+' || previous=='-'|| previous=='*' || previous=='/') &&
            (current=='+' || current=='-' || current=='*' || current=='/'));
}

int main()
{

    char expression[100];

    printf("Enter the Mathematical Expression: ");
    if(fgets(expression,sizeof(expression),stdin)==NULL)
    {
       printf("Error: Failed to read input.\n");
        return 1;
    }

    for(int i=0;expression[i]!='\0';i++)
    {
        if(isspace(expression[i])) continue; //checks for empty spaces and skips them

        if(i!=0 && hasConsecutiveOperators(expression, i)) //checks whether 2 opeartors in expression comes together
        {
            printf("Error: Invalid expression — consecutive operators found.\n");
            return 1;
        }

        if (isdigit(expression[i]) ||
            (expression[i]=='-' && (i== 0 || expression[i-1]=='+' ||
            expression[i-1]=='-' || expression[i-1]=='*' || expression[i-1]=='/')) ||
            (expression[i]=='+' && (i==0 || expression[i-1]=='+' ||
            expression[i-1]=='-' || expression[i-1]=='*' || expression[i-1]=='/'))) //handles expression if number starts with - symbol
        {
            int sign=1;
            if(expression[i]=='-')
            {
                sign=-1;
                i++;
            }

            else if (expression[i]=='+')
            {
                i++;
            }
            
            int number=0;
            while(isdigit(expression[i]))
            {
                number=number*10+(expression[i]-'0');
                i++;
            }
            i--;
            
            if(numTop!=MAX_SIZE-1)
            {
                numStack[++numTop]=sign*number;
            }

            else
            {
                printf("Error: Number stack overflow.\n");
                return 1;
            }
        }

        else if(expression[i]=='+' || expression[i]=='-' || expression[i]=='*' || expression[i]=='/')
        {
            while(opTop!=-1 && getPrecedence(opStack[opTop])>=getPrecedence(expression[i]))
            {
                processStacks();
            }
            opStack[++opTop]=expression[i];
        }

        else
        {
            printf("Error: Invalid expression.\n");

            return 1;
        }
    }

    while(opTop!=-1)
    {
        processStacks();
    }

    if(numTop!=0)
    {
        printf("Error: Invalid expression unmatched numbers and operators.\n");
        return 1;
    }

    printf("RESULT: %d\n",numStack[0]);

    return 0;
}