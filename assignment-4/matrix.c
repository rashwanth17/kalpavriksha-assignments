#include<stdio.h>
#include<time.h>
#include<stdlib.h>

void display_matrix(int matrix_size,int *matrix_ptr)
{
    for(int row=0;row<matrix_size;row++)
    {
        for(int column=0;column<matrix_size;column++)
        {
            printf("%d\t",*(matrix_ptr+row*matrix_size+column));
        }
        printf("\n");
    }
}

void generate_random_matrix(int matrix_size,int *matrix_ptr)
{
    srand(time(0));
    for(int row=0;row<matrix_size;row++)
    {
        for(int column=0;column<matrix_size;column++)
        {
            *(matrix_ptr+row*matrix_size+column)=(rand()%255)+1;
        }
    }
}

void smooth_matrix(int matrix_size,int *matrix_ptr)
{
    int *temp_matrix=(int*)malloc(matrix_size*matrix_size*sizeof(int));
    if(temp_matrix==NULL)
    {
        printf("Memory allocation failed for temp matrix.\n");
        return;
    }

    int total_elements=matrix_size*matrix_size;
    for(int index=0;index<total_elements;index++)
    {
        *(temp_matrix+index)=*(matrix_ptr+index);
    }

    for(int row=0;row<matrix_size;row++)
    {
        for(int column=0;column<matrix_size;column++)
        {
            int sum=0,count=0;
            for(int i=row-1;i<=row+1;i++)
            {
                for(int j=column-1;j<=column+1;j++)
                {
                    if(i>=0 && i<matrix_size && j>=0 && j<matrix_size)
                    {
                        sum+=*(temp_matrix+i*matrix_size+j);
                        count++;
                    }
                }
            }

            int average=sum/count;
            *(matrix_ptr+row*matrix_size+column)=average;
        }
    }

    free(temp_matrix);

    printf("Smoothed Matrix:\n");
    display_matrix(matrix_size,matrix_ptr);
}


void rotate_matrix(int matrix_size,int *matrix_ptr)
{
    for(int row=0;row<matrix_size;row++)
    {
        for(int column=row;column<matrix_size;column++)
        {
            int *element1=matrix_ptr+row*matrix_size+column;
            int *element2=matrix_ptr+column*matrix_size+row;
            int temp=*element1;
            *element1=*element2;
            *element2=temp;
        }
    }

    for(int row=0;row<matrix_size;row++)
    {
        for(int column=0;column<matrix_size/2;column++)
        {
            int *left_element=matrix_ptr+row*matrix_size+column;
            int *right_element=matrix_ptr+row*matrix_size+(matrix_size-1-column);
            int temp=*left_element;
            *left_element=*right_element;
            *right_element=temp;
        }
    }
    printf("Rotated Matrix:\n");
    display_matrix(matrix_size,matrix_ptr);
}

int main()
{
    int matrix_size;
    printf("Enter the size of the matrix (2-10): ");
    scanf("%d",&matrix_size);

    if(matrix_size<2||matrix_size>10)
    {
        printf("Invalid input. Size must be between 2 and 10.\n");
        return 1;
    }

    int *matrix_ptr=(int*)malloc(matrix_size*matrix_size*sizeof(int));

    if(matrix_ptr==NULL)
    {
        printf("Memory allocation failed.\n");
        return 1;
    }
    generate_random_matrix(matrix_size,matrix_ptr);

    printf("Randomly Generated Matrix:\n");
    display_matrix(matrix_size,matrix_ptr);

    rotate_matrix(matrix_size,matrix_ptr);
    smooth_matrix(matrix_size,matrix_ptr);

    free(matrix_ptr);

    return 0;
}
