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

int isDuplicateId(int product_ID,Product *inventory,int number_of_products)
{
    for(int index=0;index<number_of_products;index++)
    {
        if(inventory[index].product_ID==product_ID)
        {
            return 1;
        }
    }

    return 0;
}

int getValidProductID()
{
    char product_ID_text[5];
    int product_ID;

    while(1)
    {
        printf("Product ID: ");
        scanf("%s",product_ID_text);

        while(getchar()!='\n');

        if(!validInput(product_ID_text))
        {
            printf("Error: Enter proper Product Id!\n");
            continue;
        }

        product_ID=atoi(product_ID_text);

        if(product_ID<1 || product_ID>10000)
        {
            printf("Error: Product ID must be in the range between 1 to 10000!\n");
            continue;
        }
        return product_ID;
    }
}

void getValidProductName(int index,Product *inventory)
{
    char product_name[60];

    while(1)
    {
        printf("Product Name: ");
        fgets(product_name,sizeof(product_name),stdin);

        int length=strlen(product_name);

        if(length>0 && product_name[length-1]=='\n')
        {
            product_name[length-1]='\0';
            length--;
        }

        int start=0;
        while(isspace((unsigned char)product_name[start])) start++;
        int end=length-1;
        while(end>=start && isspace((unsigned char)product_name[end])) end--;

        int trimmed_length=end-start+1;

        if(trimmed_length>50)
        {
            printf("Error: The length of the Product Name should not exceed 50 characters!\n");
            continue;
        }

        if(trimmed_length==0)
        {
            printf("Error: Product Name should not be empty!\n");
            continue;
        }
    
        strncpy(inventory[index].product_name,product_name+start,trimmed_length);
        inventory[index].product_name[trimmed_length]='\0';
        break;
    }
}

float getValidProductPrice()
{
    char product_price[20];
    float price_value;

    while(1)
    {
        printf("Product Price: ");
        fgets(product_price,sizeof(product_price),stdin);

        int length=strlen(product_price);

        if(length>0 && product_price[length-1]=='\n')
        {
            product_price[length-1]='\0';
            length--;
        }

        int dot_count=0,valid=1;

        for(int i=0;*(product_price+i)!='\0';i++)
        {
            if(*(product_price+i)=='.')
            {
                dot_count++;
            }
            else if(!isdigit(*(product_price+i)))
            {
                valid=0;
            }

            if(dot_count>1)
            {
                valid=0;
            }
        }

        if(!valid)
        {
            printf("Error: Enter a valid numeric price!\n");
            continue;
        }

        price_value=atof(product_price);

        if(price_value<0 || price_value>100000)
        {
            printf("Error: price value should be in the range 0 to 100000!\n");
            continue;
        }

        return price_value;
    }
}
void getValidProductQuantity(int index,Product *inventory)
{
    char product_quantity_text[8];
    int product_quantity;

    while(1)
    {
        printf("Product Quantity: ");
        scanf("%s",product_quantity_text);

        while(getchar()!='\n');

        if(!validInput(product_quantity_text))
        {
            printf("Error: Enter proper Product quantity!\n");
            continue;
        }

        product_quantity=atoi(product_quantity_text);

        if(product_quantity<1 || product_quantity>100000)
        {
            printf("Error: Product quantity must be in the range between 1 to 100000!\n");
            continue;
        }

        inventory[index].product_quantity=product_quantity;
        break;
    }
}

void getProductDetails(Product *inventory,int number_of_products)
{
    for(int index=0;index<number_of_products;index++)
    {
        printf("Enter details for product %d\n",index+1);

        int product_ID=getValidProductID();

        if(isDuplicateId(product_ID,inventory,index))
        {
            printf("Error: Product Id already exists, enter a unique ID.\n");
            index--;
            continue;
        }
        inventory[index].product_ID=product_ID;

        getValidProductName(index,inventory);

        float product_price=getValidProductPrice();
        inventory[index].product_price=product_price;

        getValidProductQuantity(index,inventory);
        printf("\n");
    }
}

void addNewProduct(int *number_of_products,Product **inventory)
{
    (*number_of_products)++;
    Product *temporary_pointer=(Product *)realloc(*inventory,(*number_of_products)*sizeof(Product));

    if(temporary_pointer==NULL)
    {
        printf("Error: Memory Allocation failed\n");
        return;
    }

    *inventory=temporary_pointer;
    int index=*number_of_products-1;

    printf("Enter new product details:\n");

    int product_ID=getValidProductID();
    if(isDuplicateId(product_ID,*inventory,*number_of_products-1))
    {
        printf("Error: Product Id already exists, enter a unique ID.\n");
        (*number_of_products)--;
        return;
    }
    (*inventory)[index].product_ID=product_ID;

    getValidProductName(index,*inventory);

    float product_price=getValidProductPrice();
    (*inventory)[index].product_price=product_price;

    getValidProductQuantity(index,*inventory);

    printf("Product added successfully!\n");
    
}

void viewProducts(int number_of_products,Product *inventory)
{
    printf("\n\n========= PRODUCT LIST =========\n");

    for (int index=0;index<number_of_products;index++)
    {
        printf("Product ID: %d | ", inventory[index].product_ID);
        printf("Name: %s | ", inventory[index].product_name);
        printf("Price: %.2f | ", inventory[index].product_price);
        printf("Quantity: %d\n", inventory[index].product_quantity);
    }

    if(number_of_products==0)
    {
        printf("No products available.\n");
    }
}

int findProductByID(int product_ID,Product *inventory,int number_of_products)
{
    for(int index=0;index<number_of_products;index++)
    {
        if(product_ID==inventory[index].product_ID)
        {
            return index;
        }
    }

    return -1;
}

void updateQuantity(Product *inventory,int number_of_products)
{
    printf("Enter Product ID to update quantity: ");
    int product_ID=getValidProductID();
    int index=findProductByID(product_ID,inventory,number_of_products);

    if(index==-1)
    {
        printf("Error: Product Not Found!\n");
        return; 
    }

    printf("Enter new Quantity: ");
    getValidProductQuantity(index,inventory);
    
}

void searchProductByID(Product *inventory,int number_of_products)
{
    int product_ID=getValidProductID();
    int index=findProductByID(product_ID,inventory,number_of_products);

    if(index==-1)
    {
        printf("Error: Product Not Found!\n");
        return;
    }

    printf("Product ID: %d | ", inventory[index].product_ID);
    printf("Name: %s | ", inventory[index].product_name);
    printf("Price: %.2f | ", inventory[index].product_price);
    printf("Quantity: %d\n", inventory[index].product_quantity);
}

void searchProductByName(Product *inventory,int number_of_products)
{
    char search_term[60];
    printf("Enter name to search the product (partial allowed): ");
    fgets(search_term,sizeof(search_term),stdin);

    int length=strlen(search_term);

    if(length>0 && search_term[length-1]=='\n')
    {
        search_term[length-1]='\0';
        length--;
    }

    for(int index=0;index<length;index++)
    {
        search_term[index]=tolower(search_term[index]);
    }

    int found=0;
    printf("Product Found:\n");

    for(int index=0;index<number_of_products;index++)
    {
        char name_copy[60];
        strcpy(name_copy,inventory[index].product_name);

        for(int j=0;name_copy[j];j++)
        {
            name_copy[j]=tolower(name_copy[j]);
        }

        if(strstr(name_copy,search_term)!=NULL)
        {
            found=1;

            printf("Product ID: %d | ", inventory[index].product_ID);
            printf("Name: %s | ", inventory[index].product_name);
            printf("Price: %.2f | ", inventory[index].product_price);
            printf("Quantity: %d\n", inventory[index].product_quantity);
        }
    }

    if(found==0)
    {
        printf("No products found for \"%s\" .\n",search_term);
    }
}

void searchProductByPriceRange(Product *inventory,int number_of_products)
{
    printf("Enter minimum price: ");
    float minimum_product_price=getValidProductPrice();

    printf("Enter maximum price: ");
    float maximum_product_price=getValidProductPrice();

    int found=0;

    for(int index=0;index<number_of_products;index++)
    {
        if(inventory[index].product_price>=minimum_product_price && inventory[index].product_price<=maximum_product_price)
        {
            found=1;
            printf("Product ID: %d | ", inventory[index].product_ID);
            printf("Name: %s | ", inventory[index].product_name);
            printf("Price: %.2f | ", inventory[index].product_price);
            printf("Quantity: %d\n", inventory[index].product_quantity);
        }
    }

    if(found==0)
    {
        printf("No products found within the price range %.2f to %.2f\n",minimum_product_price,maximum_product_price);
    }

}

void deleteProduct(Product **inventory,int *number_of_products)
{
    int product_ID=getValidProductID();
    int index=findProductByID(product_ID,*inventory,*number_of_products);

    if(index==-1)
    {
        printf("Error: Product Not Found!\n");
        return; 
    }

    for(int i=index;i<*number_of_products-1;i++)
    {
        (*inventory)[i]=(*inventory)[i+1];
    }

    (*number_of_products)--;

    if(*number_of_products==0)
    {
        free(*inventory);
        *inventory=NULL;
        printf("All products deleted.\n");
        return;
    }

    Product *temporary_pointer=(Product *)realloc(*inventory,(*number_of_products)*sizeof(Product));

    if(temporary_pointer==NULL && *number_of_products>0)
    {
        printf("Error: Memory Allocation failed\n");
        return;
    }

    *inventory=temporary_pointer;
    
    printf("Product deleted successfully!!\n");
}


int main()
{
    char number_of_products_text[5];
    int number_of_products;

    while(1)
    {
        printf("Enter initial number of products (1-100): ");
        scanf("%s",number_of_products_text);

        while(getchar()!='\n');

        if(!validInput(number_of_products_text))
        {
            printf("Error: Enter proper Integer value between 1 to 100.\n");
            continue;        
        }

        number_of_products=atoi(number_of_products_text);

        if(number_of_products<1 || number_of_products>100)
        {
            printf("Error: Enter proper Integer value between 1 to 100.\n");
            continue;
        }
        break;
    }

    Product *inventory=(Product *)calloc(number_of_products,sizeof(Product));

    if(inventory==NULL)
    {
        printf("Memory Allocation Failed!\n");
        return 0;
    }

    getProductDetails(inventory,number_of_products);

    char choice_text[2];
    int choice;

    while(1)
    {
        printf("\n\n============INVENTORY MENU============\n");
        printf("1. Add New Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search Product by ID\n");
        printf("5. Search Product by Name\n");
        printf("6. Search Product by Price Range\n");
        printf("7. Delete Product\n");
        printf("8. Exit\n");
        

        while(1)
        {
            printf("Enter your choice: \n");
            scanf("%s",choice_text);

            while(getchar()!='\n');

            if(!validInput(choice_text))
            {
                printf("Error: Enter a valid number between 1 to 8!\n");
                continue;
            }

            choice=atoi(choice_text);
            if(choice<1 || choice>8)
            {
                printf("Error: Enter a valid number between 1 to 8\n");
                continue;
            }
            break;
        }
     
        switch (choice)
        {
            case 1:
                addNewProduct(&number_of_products,&inventory);
                break;

            case 2:
                viewProducts(number_of_products,inventory);
                break;

            case 3:
                updateQuantity(inventory,number_of_products);
                break;

            case 4:
                searchProductByID(inventory,number_of_products);
                break;

            case 5:
                searchProductByName(inventory,number_of_products);
                break;

            case 6:
                searchProductByPriceRange(inventory,number_of_products);
                break;

            case 7:
                deleteProduct(&inventory,&number_of_products);
                break;

            case 8:
                printf("Memory released successfully. Exiting program...!\n");
                free(inventory);
                inventory=NULL;
                return 0;
        }

    }

}
