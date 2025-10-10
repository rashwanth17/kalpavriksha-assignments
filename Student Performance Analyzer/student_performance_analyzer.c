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

void display_student_details(Student student)
{
    printf("Roll : %d\n",student.roll_number);
    printf("Name : %s\n",student.name);
    printf("Total : %d\n",student.total_marks);
    printf("Average : %lf\n",student.average_mark);
    printf("Grade : %c\n",student.grade);
    printf("\n");
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

        display_student_details(student[i]);
    }

}