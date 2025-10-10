//student performance analyzer


#include <stdio.h>

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


double calculate_total_marks(double mark1,double mark2, double mark3)
{
    return mark1+mark2+mark3;
}

double calculate_average_mark(double total_marks)
{
    return total_marks/3.0;
}

int main()
{
    int number_of_students;
    printf("Enter the Number of Students: ");
    scanf("%d",&number_of_students);
    Student student[100];

    for(int i=0;i<number_of_students;i++)
    {
        printf("Enter Roll number , Name , Mark 1 , Mark 2 , Mark 3 :");
        scanf("%d %50s %lf %lf %lf",&student[i].roll_number,student[i].name,&student[i].mark1,&student[i].mark2,&student[i].mark3);
        student[i].total_marks=calculate_total_marks(student[i].mark1,student[i].mark2,student[i].mark3);
        student[i].average_mark=calculate_average_marks(student[i].total_marks);
        student[i].grade=calculate_grade(student[i].average_mark);
    }

}