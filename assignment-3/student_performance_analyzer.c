//student performance analyzer


#include <stdio.h>

typedef struct
{
    int roll_number;
    char name[50];
    double mark1;
    double mark2;
    double mark3;
    int total_marks;
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

 void sort_by_roll_number(Student student[],int number_of_students)
 {
    for(int i=0;i<number_of_students-1;i++)
    {
        for(int j=0;j<number_of_students-i-1;j++)
        {
            if(student[j].roll_number>student[j+1].roll_number)
            {
                Student swapStudent=student[j];
                student[j]=student[j+1];
                student[j+1]=swapStudent;
            }
        }
    }
 }


int calculate_total_marks(double mark1,double mark2, double mark3)
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
    for(int i=0;i<number_of_students;i++)
    {

        printf("Roll : %d\n",student[i].roll_number);
        printf("Name : %s\n",student[i].name);
        printf("Total : %d\n",student[i].total_marks);
        printf("Average : %.2lf\n",student[i].average_mark);
        printf("Grade : %c\n",student[i].grade);

        if(student[i].average_mark<35)
        {
                printf("\n");
                continue;
        }
        else
        {
            display_student_performance(student[i].grade);
        }

    }
}

int main()
{
    int number_of_students;
    printf("Enter the Number of Students: ");
    scanf("%d",&number_of_students);
    Student student[100];

    if(number_of_students<=0)
    {
        printf("Invalid number of students \n");
        return 0;
    }

    for(int i=0;i<number_of_students;i++)
    {
        printf("Enter Roll number , Name , Mark 1 , Mark 2 , Mark 3 :");
        scanf("%d %50s %lf %lf %lf",&student[i].roll_number,student[i].name,&student[i].mark1,&student[i].mark2,&student[i].mark3);
        
        student[i].total_marks=calculate_total_marks(student[i].mark1,student[i].mark2,student[i].mark3);
        student[i].average_mark=calculate_average_marks(student[i].total_marks);
        student[i].grade=calculate_grade(student[i].average_mark);

    }
        sort_by_roll_number(student,number_of_students);
        display_student_details(student,number_of_students);
        printf("\nList of Roll Numbers (via recursion): ");
        print_roll_numbers(student,0,number_of_students);
        printf("\n");

}