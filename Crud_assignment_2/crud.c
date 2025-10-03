#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//structure for the user deatails
typedef struct{
    int id;
    char name[50];
    int age;
}user;

//to find the next ID
int ID()
{
    FILE *file=fopen("users.txt","r");
    if(!file) return 1;
    int max=0;
    int curr;
    int age;
    char name[50];
    while(fscanf(file,"%d,%49[^,],%d\n",&curr,name,&age)==3)
    {
        if(curr>max) max=curr;
    }
    fclose(file);
    return max+1;
}

//creates a new user
void createUser()
{
    user u;
    u.id=ID();
    printf("Enter your name: \n");
    getchar();
    fgets(u.name,sizeof(u.name),stdin);
    u.name[strcspn(u.name,"\n")]=0;
    printf("Enter your age: \n");
    scanf("%d",&u.age);
    while(getchar()!='\n');
    FILE *file=fopen("users.txt","a");
    if(!file)
    {
        printf("cannot open file");
        return;
    }
    fprintf(file,"%d,%s,%d\n",u.id,u.name,u.age);
    printf("User created with ID %d \n",u.id);
    fclose(file);
}

//displays all the users from file
void displayUser()
{
    FILE *file=fopen("users.txt","r");
    if(!file)
    {
        printf("user not found");
        return;
    }
    char s[100];
    printf("ID Name Age \n");
    while(fgets(s,sizeof(s),file))
    {
        user u;
        char *sp=strtok(s,",");
        u.id=atoi(sp);
        sp=strtok(NULL,",");
        strcpy(u.name,sp);
        sp=strtok(NULL,",");
        u.age=atoi(sp);
        printf("%d %s %d\n",u.id,u.name,u.age);
    }
    fclose(file);
}

//updates the userdetails
void updateUser()
{
    int id;
    printf("Enter the ID of the user you want to update: \n");
    scanf("%d",&id);
    while(getchar()!='\n');
    FILE *file=fopen("users.txt","r");
    if(!file)
    {
        printf("cannot open file");
        return;
    }
    FILE *f2=fopen("temp.txt","w");
    if(!f2)
    {
        printf("cannot open file");
        return;
    }

    char s[100];
    int x=0;
    while(fgets(s,sizeof(s),file))
    {
        user u;
        char *sp=strtok(s,",");
        u.id=atoi(sp);
        sp=strtok(NULL,",");
        strcpy(u.name,sp);
        sp=strtok(NULL,",");
        u.age=atoi(sp);

        if(u.id==id)
        {
            x=1;
            printf("Enter the new name: \n");
            fgets(u.name,sizeof(u.name),stdin);
            u.name[strcspn(u.name,"\n")]=0;
            printf("Enter new age: \n");
            scanf("%d",&u.age);
            while(getchar()!='\n');
        }
        fprintf(f2,"%d,%s,%d\n",u.id,u.name,u.age);
    }

    fclose(file);
    fclose(f2);
    if(x==0)
    {
        printf("user not found");
        return;
    }

    remove("users.txt");
    rename("temp.txt","users.txt");
    printf("User details updated\n");
}

//deletes the user
void deleteUser()
{
    int id;
    printf("Enter ID to delete the user: \n");
    scanf("%d",&id);
    while(getchar()!='\n');
    FILE *file=fopen("users.txt","r");
    if(!file)
    {
        printf("cannot open file");
        return;
    }
    FILE *f2=fopen("temp.txt","w");
    if(!f2)
    {
        printf("cannot open file");
        return;
    }

    char s[100];
    int x=0;
    while(fgets(s,sizeof(s),file))
    {
        user u;
        char *sp=strtok(s,",");
        u.id=atoi(sp);
        sp=strtok(NULL,",");
        strcpy(u.name,sp);
        sp=strtok(NULL,",");
        u.age=atoi(sp);

        if(u.id==id)
        {
            x=1;
            continue;
        }
        fprintf(f2,"%d,%s,%d\n",u.id,u.name,u.age);
    }

    fclose(file);
    fclose(f2);
    if(x==0)
    {
        printf("user not found");
        return;
    }

    remove("users.txt");
    rename("temp.txt","users.txt");
    printf("User deleted\n");
}
int main()
{
    while(1)
    {
        printf("1. Create User\n");
        printf("2. Display Users\n");
        printf("3. Update user details\n");
        printf("4. Delete User\n");
        printf("5. EXIT\n");
        printf("Enter the choice: ");
        int c;
        scanf("%d",&c);
        switch(c)
        {
            case 1:
            createUser();
            break;

            case 2:
            displayUser();
            break;

            case 3:
            updateUser();
            break;

            case 4:
            deleteUser();
            break;

            case 5:
            printf("BYE");
            exit(0);

            default:
            printf("please select relevant choice");
        }
    }
}