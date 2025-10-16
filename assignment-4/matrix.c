#include<stdio.h>
#include<time.h>
#include<stdlib.h>

void display_matrix(int matrix_size,int *p)
{
    for(int row=0;row<matrix_size;row++)
    {
        for(int column=0;column<matrix_size;column++)
        {
            printf("%d\t",*(p+row*matrix_size+column));
        }
        printf("\n");
    }
}

void generate_random_matrix(int matrix_size,int *p)
{
    srand(time(0));
    for(int row=0;row<matrix_size;row++)
    {
        for(int column=0;column<matrix_size;column++)
        {
            *(p+row*matrix_size+column)=(rand()%255)+1;
        }
    }
}

void rotate_matrix(int matrix_size,int *p)
{
    for(int row=0;row<matrix_size;row++)
    {
        for(int column=row;column<matrix_size;column++)
        {
            int *a=p+row*matrix_size+column;
            int *b=p+column*matrix_size+row;
            int temp=*a;
            *a=*b;
            *b=temp;
        }
    }
    for(int row=0;row<matrix_size;row++)
    {
        for(int column=0;column<matrix_size/2;column++)
        {
            int *a=p+row*matrix_size+column;
            int *b=p+row*matrix_size+(matrix_size-1-column);
            int temp=*a;
            *a=*b;
            *b=temp;
        }
    }
    display_matrix(matrix_size,p);
}

int main()
{
    int matrix_size;
    printf("Enter the size of the matrix: ");
    scanf("%d",&matrix_size);
    int matrix[matrix_size][matrix_size];
    int *p=&matrix[0][0];
    generate_random_matrix(matrix_size,p);
    printf("Randomly Generated Matrix:\n");
    display_matrix(matrix_size,p);
    printf("Rotated matrix by 90 degrees:\n");
    rotate_matrix(matrix_size,p);
}
