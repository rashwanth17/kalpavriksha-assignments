#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#define USER_FILE "users.txt"
#define TEMP_FILE "temp.txt"

//structure for the user deatails
typedef struct{
    int user_id;
    char name[50];
    int age;
}User;

//to find the next ID
int getNextUserId()
{

    FILE *file=fopen(USER_FILE,"r");
    if(!file)
    {
        printf("Error: Could not open file\n");
        return 1;
    }

    int maxUserId=0;
    int currentUserId;
    int tempAge;
    char tempName[50];

    while(fscanf(file,"%d,%49[^,],%d\n",&currentUserId,tempName,&tempAge)==3)
    {
        if(currentUserId>maxUserId) maxUserId=currentUserId;
    }

    fclose(file);

    return maxUserId+1;
}

//creates a new user
void createUser()
{

    User user;
    user.user_id=getNextUserId();

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

    fprintf(file,"%d,%s,%d\n",user.user_id,user.name,user.age);
    printf("User created with ID %d \n",user.user_id);

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

    char line[100];
    printf("ID Name Age \n");

    while(fgets(line,sizeof(line),file))
    {
        User user;
        char *token=strtok(line,",");
        user.user_id=atoi(token);
        token=strtok(NULL,",");
        strcpy(user.name,token);
        token=strtok(NULL,",");
        user.age=atoi(token);
        printf("%d %s %d\n",user.user_id,user.name,user.age);
    }

    fclose(file);
}

//updates the userdetails
void updateUser()
{

    int user_id;
    printf("Enter the ID of the user you want to update: \n");
    scanf("%d",&user_id);

    while(getchar()!='\n');
    FILE *file=fopen(USER_FILE,"r");

    if(!file)
    {
        printf("Error: Could not open file\n");
        return;
    }

    FILE *tempFile=fopen(TEMP_FILE,"w");
    if(!tempFile)
    {
        printf("Error: Could not open file\n");
        return;
    }

    char line[100];
    int isUserFound=0;

    while(fgets(line,sizeof(line),file))
    {
        User user;
        char *token=strtok(line,",");
        user.user_id=atoi(token);
        token=strtok(NULL,",");
        strcpy(user.name,token);
        token=strtok(NULL,",");
        user.age=atoi(token);

        if(user.user_id==user_id)
        {
            isUserFound=1;
            printf("Enter the new name: \n");
            fgets(user.name,sizeof(user.name),stdin);
            user.name[strcspn(user.name,"\n")]=0;
            printf("Enter new age: \n");
            scanf("%d",&user.age);
            while(getchar()!='\n');
        }

        fprintf(tempFile,"%d,%s,%d\n",user.user_id,user.name,user.age);
    }

    fclose(file);
    fclose(tempFile);

    if(isUserFound==0)
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

    int user_id;

    printf("Enter ID to delete the user: \n");
    scanf("%d",&user_id);
    while(getchar()!='\n');

    FILE *file=fopen(USER_FILE,"r");

    if(!file)
    {
        printf("Error: Could not open file\n");
        return;
    }

    FILE *tempFile=fopen(TEMP_FILE,"w");

    if(!tempFile)
    {
        printf("Error: Could not open file\n");
        return;
    }

    char line[100];
    int isUserFound=0;

    while(fgets(line,sizeof(line),file))
    {
        User user;
        char *token=strtok(line,",");
        user.user_id=atoi(token);
        token=strtok(NULL,",");
        strcpy(user.name,token);
        token=strtok(NULL,",");
        user.age=atoi(token);

        if(user.user_id==user_id)
        {
            isUserFound=1;
            continue;
        }

        fprintf(tempFile,"%d,%s,%d\n",user.user_id,user.name,user.age);
    }

    fclose(file);
    fclose(tempFile);

    if(isUserFound==0)
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