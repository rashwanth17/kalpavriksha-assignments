//student performance analyzer


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct
{
    int roll_number;
    char name[50];
    double mark1;
    double mark2;
    double mark3;
    double total_marks;
    double average_mark;
    char grade;
} Student;

 void print_roll_numbers(Student student[],int index,int number_of_students)
 {
    if(index==number_of_students)
    {
        return;
    }
    printf("%d ",student[index].roll_number);

    print_roll_numbers(student,index+1,number_of_students);
 }

 int is_valid_input(char *input)
 {
    if(input[0]=='\0')
    {
        return 0;
    }
    
    for(int index=0; input[index]!='\0';index++)
    {
        if(!isdigit(input[index]))
        {
            return 0;
        }
    }

    return 1;
 }

 void sort_by_roll_number(Student student[],int number_of_students)
 {
    for(int index=0;index<number_of_students-1;index++)
    {
        for(int next_index=0;next_index<number_of_students-index-1;next_index++)
        {
            if(student[next_index].roll_number>student[next_index+1].roll_number)
            {
                Student swapStudent=student[next_index];
                student[next_index]=student[next_index+1];
                student[next_index+1]=swapStudent;
            }
        }
    }
 }


double calculate_total_marks(double mark1,double mark2, double mark3)
{
    return mark1+mark2+mark3;
}

double calculate_average_marks(double total_marks)
{
    return total_marks/3.0;
}

char calculate_grade(double average_mark)
{
    if(average_mark>=85 && average_mark<=100)
    {
        return 'A';
    }
    else if(average_mark>=70 && average_mark<85)
    {
        return 'B';
    }
    else if(average_mark>=50 && average_mark<70)
    {
        return 'C';
    }
    else if(average_mark>=35 && average_mark<50)
    {
        return 'D';
    }
    else
    {
        return 'F';
    }
}

void display_student_performance(char grade)
{
    if(grade=='A')
    {
        printf("Performance : *****\n");
    }
    else if(grade=='B')
    {
        printf("Performance : ****\n");
    }
    else if(grade=='C')
    {
        printf("Performance : ***\n");
    }
    else if(grade=='D')
    {
        printf("Performance : **\n");
    }

    printf("\n");
    
}


void display_student_details(Student student[], int number_of_students)
{
    for(int index=0;index<number_of_students;index++)
    {

        printf("Roll : %d\n",student[index].roll_number);
        printf("Name : %s\n",student[index].name);
        printf("Total : %.2lf\n",student[index].total_marks);
        printf("Average : %.2lf\n",student[index].average_mark);
        printf("Grade : %c\n",student[index].grade);

        if(student[index].average_mark<35)
        {
                printf("\n");
                continue;
        }
        else
        {
            display_student_performance(student[index].grade);
        }

    }
}

int main()
{
    char input[100];
    int number_of_students;
    printf("Enter the Number of Students: ");
    scanf("%s",input);
    if (!is_valid_input(input))
    {
        printf("Error: Invalid input. Please enter an integer for number of students.\n");
        return 1;
    }

    number_of_students=atoi(input);
    
    if(number_of_students<1 || number_of_students>100)
    {
        printf("Error: Number of students must be between 1 and 100.\n");
        return 0;
    }
    
    Student student[100];
    char roll_input[50];

    for(int index=0;index<number_of_students;index++)
    {
        printf("Enter Roll number , Name , Mark 1 , Mark 2 , Mark 3 :");
        if (scanf("%s %49s %lf %lf %lf",roll_input,student[index].name,&student[index].mark1,&student[index].mark2,&student[index].mark3) != 5) 
        {
            printf("Error: Invalid input format. Please enter roll number, name and three marks correctly.\n");
            return 1;
        }

        if(!is_valid_input(roll_input))
        {
             printf("Error: Roll number must be a positive integer.\n");
             return 1;
        }

        student[index].roll_number=atoi(roll_input);

        if (student[index].roll_number <= 0) 
        {
            printf("Error: Roll number must be a positive integer.\n");
            return 1;
        }



         if (student[index].mark1<0 ||student[index].mark1>100 ||
            student[index].mark2<0 ||student[index].mark2>100 ||
            student[index].mark3<0 ||student[index].mark3>100)
        {
            printf("Error: Marks should be between 0 and 100.\n");
            return 1;
        }
        
        student[index].total_marks=calculate_total_marks(student[index].mark1,student[index].mark2,student[index].mark3);
        student[index].average_mark=calculate_average_marks(student[index].total_marks);
        student[index].grade=calculate_grade(student[index].average_mark);

    }
        sort_by_roll_number(student,number_of_students);
        display_student_details(student,number_of_students);
        
        printf("\nList of Roll Numbers (via recursion): ");
        print_roll_numbers(student,0,number_of_students);
        printf("\n");

}