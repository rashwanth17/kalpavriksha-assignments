// Dynamic Inventory Management System
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
    int product_ID;
    char product_name[60];
    float product_price;
    int product_quantity;
} Product;

int validInput(char *number_of_products_text)
{
    for(int index = 0;*(number_of_products_text+index)!='\0';index++)
    {
        if(!isdigit(*(number_of_products_text+index)))
        {
            return 0;
        }
    }
    return 1;
}
void getValidProductID(int index,Product *inventory)
{
    char product_ID_text[5];
    scanf("%s",product_ID_text);
    if(!validInput(product_ID_text))
    {
        printf("Error: Enter proper Product Id!");
        exit(0);
    }

    int product_ID=atoi(product_ID_text);
    if(product_ID<1 || product_ID>10000)
    {
        printf("Error: Product ID must be in the range between 1 to 10000!");
        exit(0);
    }
    inventory[index].product_ID=product_ID;
}

void getValidProductName(int index,Product *inventory)
{
    char product_name[60];
    getchar();
    fgets(product_name,sizeof(product_name),stdin);
    int length=strlen(product_name);

    if(length>0 && product_name[length-1]=='\n')
    {
        product_name[length-1]='\0';
        length--;
    }

    if(length>50)
    {
        printf("Error: The length of the Product Name should not exceed 50 characters!");
        exit(0);
    }

    if(length==0)
    {
        printf("Error: Product Name should not be empty!");
        exit(0);
    }
    
    strcpy(inventory[index].product_name,product_name);
}

void getValidProductPrice(int index,Product *inventory)
{
    char product_price[20];
    getchar();
    fgets(product_price,sizeof(product_price),stdin);
    int length=strlen(product_price);
    if(length>0 && product_price[length-1]=='\n')
    {
        product_price[length-1]='\0';
        length--;
    }
    int dot_count=0;
    for(int i=0;*(product_price+i)!='\0';i++)
    {
        if(*(product_price+i)=='.')
        {
            dot_count++;
        }
        else if(!isdigit(*(product_price+i)))
        {
            printf("Error: Enter a valid input for price!");
            exit(0);
        }
        if(dot_count>1)
        {
            printf("Error: Enter a valid input for price!");
            exit(0);
        }
    }
    float price_value=atof(product_price);
    if(price_value<=0 || price_value>100000)
    {
        printf("Error: price value should be in the range 1 to 10000!");
        exit(0);
    }
    inventory[index].product_price=price_value;
}
void getValidProductQuantity(int index,Product *inventory)
{
    char product_quantity_text[8];
    scanf("%s",product_quantity_text);
    if(!validInput(product_quantity_text))
    {
        printf("Error: Enter proper Product Id!");
        exit(0);
    }

    int product_quantity=atoi(product_quantity_text);
    if(product_quantity<1 || product_quantity>100000)
    {
        printf("Error: Product quantity must be in the range between 1 to 100000!");
        exit(0);
    }
    inventory[index].product_quantity=product_quantity;
}

void getProductDetails(Product *inventory,int number_of_products)
{
    // char product_id_text[5];
    for(int index=0;index<number_of_products;index++)
    {
        printf("Enter details for product %d\n",index+1);
        printf("Product ID: ");
        getValidProductID(index,inventory);
        printf("Product Name: ");
        getValidProductName(index,inventory);
        printf("Product Price: ");
        getValidProductPrice(index,inventory);
        printf("Product Quantity: ");
        getValidProductQuantity(index,inventory);
    }
}

int main()
{
    char number_of_products_text[5];
    int number_of_products;
    printf("Enter initial number of products (1-100): ");
    scanf("%s",number_of_products_text);
    if(!validInput(number_of_products_text))
    {
        printf("Error: Enter proper Integer value between 1 to 100.");
        return 0;        
    }
    number_of_products=atoi(number_of_products_text);
    if(number_of_products<1 || number_of_products>100)
    {
        printf("Error: Enter proper Integer value between 1 to 100.");
        return 0;
    }

    Product *inventory=(Product *)calloc(number_of_products,sizeof(Product));

    if(inventory==NULL)
    {
        printf("Memory Allocation Failed!");
        return 0;
    }

    getProductDetails(inventory,number_of_products);

}

