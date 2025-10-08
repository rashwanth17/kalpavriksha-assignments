#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define USER_FILE "users.txt"
#define TEMP_FILE "temp.txt"

//structure for the user deatails
typedef struct{
    int id;
    char name[50];
    int age;
}User;

//to find the next ID
int ID()
{

    FILE *file=fopen(USER_FILE,"r");
    if(!file)
    {
        printf("Error: Could not open file\n");
        return 1;
    }

    int maxID=0;
    int currentId;
    int age;
    char name[50];

    while(fscanf(file,"%d,%49[^,],%d\n",&currentId,name,&age)==3)
    {
        if(currentId>maxID) maxID=currentId;
    }

    fclose(file);

    return maxID+1;
}

//creates a new user
void createUser()
{

    User user;
    user.id=ID();

    printf("Enter your name: \n");
    getchar();
    fgets(user.name,sizeof(user.name),stdin);
    user.name[strcspn(user.name,"\n")]=0;

    printf("Enter your age: \n");
    scanf("%d",&user.age);

    while(getchar()!='\n');
    FILE *file=fopen(USER_FILE,"a");
    if(!file)
    {
        printf("Error: Could not open file\n");
        return;
    }

    fprintf(file,"%d,%s,%d\n",user.id,user.name,user.age);
    printf("User created with ID %d \n",user.id);

    fclose(file);
}

//displays all the users from file
void displayUser()
{

    FILE *file=fopen(USER_FILE,"r");
    if(!file)
    {
        printf("User not found");
        return;
    }

    char s[100];
    printf("ID Name Age \n");

    while(fgets(s,sizeof(s),file))
    {
        User user;
        char *token=strtok(s,",");
        user.id=atoi(token);
        token=strtok(NULL,",");
        strcpy(user.name,token);
        token=strtok(NULL,",");
        user.age=atoi(token);
        printf("%d %s %d\n",user.id,user.name,user.age);
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
    FILE *file=fopen(USER_FILE,"r");

    if(!file)
    {
        printf("Error: Could not open file\n");
        return;
    }

    FILE *temp=fopen(TEMP_FILE,"w");
    if(!temp)
    {
        printf("Error: Could not open file\n");
        return;
    }

    char s[100];
    int found=0;

    while(fgets(s,sizeof(s),file))
    {
        User user;
        char *token=strtok(s,",");
        user.id=atoi(token);
        token=strtok(NULL,",");
        strcpy(user.name,token);
        token=strtok(NULL,",");
        user.age=atoi(token);

        if(user.id==id)
        {
            found=1;
            printf("Enter the new name: \n");
            fgets(user.name,sizeof(user.name),stdin);
            user.name[strcspn(user.name,"\n")]=0;
            printf("Enter new age: \n");
            scanf("%d",&user.age);
            while(getchar()!='\n');
        }

        fprintf(temp,"%d,%s,%d\n",user.id,user.name,user.age);
    }

    fclose(file);
    fclose(temp);

    if(found==0)
    {
        printf("User not found");
        return;
    }

    if(remove(USER_FILE)!=0)
    {
        printf("Error deleting original file");
        return;
    }

    if(rename(TEMP_FILE,USER_FILE))
    {
        printf("Error renaming temporary file");
        return;
    }

    printf("User details updated\n");
}

//deletes the user
void deleteUser()
{

    int id;

    printf("Enter ID to delete the user: \n");
    scanf("%d",&id);
    while(getchar()!='\n');

    FILE *file=fopen(USER_FILE,"r");

    if(!file)
    {
        printf("Error: Could not open file\n");
        return;
    }

    FILE *temp=fopen(TEMP_FILE,"w");

    if(!temp)
    {
        printf("Error: Could not open file\n");
        return;
    }

    char s[100];
    int found=0;

    while(fgets(s,sizeof(s),file))
    {
        User user;
        char *token=strtok(s,",");
        user.id=atoi(token);
        token=strtok(NULL,",");
        strcpy(user.name,token);
        token=strtok(NULL,",");
        user.age=atoi(token);

        if(user.id==id)
        {
            found=1;
            continue;
        }

        fprintf(temp,"%d,%s,%d\n",user.id,user.name,user.age);
    }

    fclose(file);
    fclose(temp);

    if(found==0)
    {
        printf("User not found");
        return;
    }

    if(remove(USER_FILE)!=0)
    {
        printf("Error deleting original file");
        return;
    }

    if(rename(TEMP_FILE,USER_FILE))
    {
        printf("Error renaming temporary file");
        return;
    }

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

        int choice;
        if(scanf("%d",&choice)!=1)
        {
            printf("Inavlid input, please enter a number betwen 1-5\n");
            while(getchar()!='\n');
            continue;
        };

        switch(choice)
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
            printf("Please select relevant choice between 1-5\n");
        }
    }
}