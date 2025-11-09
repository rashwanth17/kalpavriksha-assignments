#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define BLOCK_SIZE 512
#define NUMBER_OF_BLOCKS 1024
#define MAX_NAME 50
#define MAX_LINE 256

typedef struct FreeBlock
{
    int index;
    struct FreeBlock *next;
    struct FreeBlock *prev;
} FreeBlock;

typedef struct FileNode
{
    char name[MAX_NAME];
    int isDirectory;
    struct FileNode *next;
    struct FileNode *prev;
    struct FileNode *parent;
    struct FileNode *child;

    int *blockPointers;
    int dynamicBlocksCount;
    int fileSize;
} FileNode;

static unsigned char virtualDisk[NUMBER_OF_BLOCKS][BLOCK_SIZE];
static FreeBlock *freeBlockListHead = NULL;
static FreeBlock *freeBlockListTail = NULL;
static FileNode *root = NULL;
static int freeBlockCount = 0;
static FileNode *currentDirectory = NULL;

static int is_bad_name(const char *name)
{ 
    if(!name || !(*name))
    {
        return 1; 
    }

    if(!strcmp(name, ".") || !strcmp(name, ".."))
    {
        return 1;
    }

    if(strlen(name) >= MAX_NAME)
    {
        return 1;
    }

    return 0; 
}

void initializeFreeBlockList()
{
    freeBlockListHead = freeBlockListTail = NULL; 
    freeBlockCount = 0;

    for(int index = 0; index < NUMBER_OF_BLOCKS; index++)
    {
        FreeBlock *newBlock = (FreeBlock*)malloc(sizeof(FreeBlock));
        newBlock->index = index;
        newBlock->next = NULL; 
        newBlock->prev = freeBlockListTail;

        if(!freeBlockListHead)
        {
            freeBlockListHead = newBlock;
        } 
        if(freeBlockListTail)
        {
            freeBlockListTail->next = newBlock;
        }

        freeBlockListTail = newBlock; 
        freeBlockCount++;
    }
}

int popFreeBlockHead()
{
    if(freeBlockListHead == NULL)
    {
        return -1;
    }

    int index = freeBlockListHead->index;
    FreeBlock *node = freeBlockListHead;
    freeBlockListHead = freeBlockListHead->next;

    if(freeBlockListHead != NULL)
    {
        freeBlockListHead->prev = NULL;
    }
    else
    {
        freeBlockListTail = NULL;
    }

    free(node);
    freeBlockCount--;
    return index;
}

void pushFreeBlockTail(int index)
{
    FreeBlock *newBlock = (FreeBlock *)malloc(sizeof(FreeBlock));
    newBlock->index = index;
    newBlock->next = NULL;
    newBlock->prev = freeBlockListTail;

    if(!freeBlockListHead)
    {
        freeBlockListHead = newBlock;
    }
    
    if(freeBlockListTail)
    {
        freeBlockListTail->next = newBlock;
    }

    freeBlockListTail = newBlock;
    freeBlockCount++;
}

int countFreeBlocks()
{
    return freeBlockCount;
}

void freeFreeBlockList()
{
    while(freeBlockListHead != NULL)
    {
        FreeBlock *node = freeBlockListHead;
        freeBlockListHead = freeBlockListHead->next;
        free(node);
    }
    freeBlockListTail = NULL;
    freeBlockCount = 0;
}

FileNode* createFileNode(char *name, int isDirectory, FileNode *parent)
{
    FileNode *newNode = (FileNode *)malloc(sizeof(FileNode));
    memset(newNode, 0, sizeof(*newNode));
    strncpy(newNode->name, name ? name : "", MAX_NAME - 1);
    newNode->name[MAX_NAME - 1] = '\0'; 
    newNode->isDirectory = isDirectory;
    newNode->next = NULL;
    newNode->prev = NULL;
    newNode->parent = parent;
    newNode->child = NULL;
    newNode->blockPointers = NULL;
    newNode->dynamicBlocksCount = 0;
    newNode->fileSize = 0;
    return newNode;
}

void insertChild(FileNode *parent, FileNode *child)
{
    if(!parent || !child)
    {
        return;
    }

    child->parent = parent;

    if(parent->child == NULL)
    {
        parent->child = child;
        child->next = child;
        child->prev = child;
        return;
    }
    
    FileNode *head = parent->child;
    FileNode *tail = head->prev;
    tail->next = child;
    head->prev = child;
    child->next = head;
    child->prev = tail;
}

FileNode* findChild(FileNode *parent, char *name)
{
    if(!parent || !parent->child)
    {
        return NULL;
    }

    FileNode *current = parent->child;
    do
    {
        if(strcmp(current->name, name) == 0)
        {
            return current;
        }
        current = current->next;
    }
    while(current != parent->child);

    return NULL;
}

int unlinkChildNode(FileNode *parent, FileNode *node)
{
    if(!parent || !node || !parent->child)
    {
        return 0;
    }

    FileNode *current = parent->child;
    do
    {
        if(current == node)
        {
            if(current->next == current)
            {
                parent->child = NULL;
            }
            else
            {
                current->prev->next = current->next;
                current->next->prev = current->prev;
                if(parent->child == current)
                {
                    parent->child = current->next;
                }
            }
            current->next = current->prev = NULL;
            current->parent = NULL;
            return 1;
        }
        current = current->next;
    }
    while(current != parent->child);

    return 0;
}

void freeFileSystem(FileNode *node)
{
    if(node == NULL)
    {
        return;
    }

    while(node->isDirectory && node->child)
    {
        FileNode *c = node->child;
        unlinkChildNode(node, c);
        freeFileSystem(c);
    }

    if(!node->isDirectory && node->blockPointers)
    {
        for(int index = 0; index < node->dynamicBlocksCount; index++)
        {
            pushFreeBlockTail(node->blockPointers[index]);
        }
        free(node->blockPointers);
    }

    free(node);
}

int rightTrim(char *str)
{
    int len = (int)strlen(str);
    int index = len - 1;

    while(index >= 0 && (str[index] == ' ' || str[index] == '\t' || str[index] == '\n'))
    {
        str[index] = '\0';
        index--;
    }
    return index + 1;
}

int leftTrim(char *str)
{
    int len = (int)strlen(str);
    int index = 0;

    while(index < len && (str[index] == ' ' || str[index] == '\t' || str[index] == '\n'))
    {
        index++;
    }

    if(index > 0)
    {
        memmove(str, str + index, len - index + 1);
    }

    return len - index;
}

int freeBlockOfFile(FileNode *file)
{
    if(file == NULL || file->isDirectory || file->blockPointers == NULL)
    {
        return 0;
    }

    int freed = 0;

    for(int index = 0; index < file->dynamicBlocksCount; index++)
    {
        int dataBlockIndex = file->blockPointers[index];
        if(dataBlockIndex >= 0)
        {
            pushFreeBlockTail(dataBlockIndex);
            freed++;
        }
    }

    free(file->blockPointers);
    file->blockPointers = NULL;
    file->dynamicBlocksCount = 0;
    file->fileSize = 0;

    return freed;
}

static void writeAppend(FileNode *file, const char *data)
{
    int addBytes = (int)strlen(data ? data : "");
    if(addBytes == 0)
    {
        printf("Data written successfully (size=0 bytes).\n");
        return;
    }

    int currentSize = file->fileSize;
    int currentBlocks = currentSize ? ((currentSize + BLOCK_SIZE - 1) / BLOCK_SIZE) : 0;
    int off = currentSize % BLOCK_SIZE;
    int totalSize = currentSize + addBytes;
    int needBlocks = (totalSize + BLOCK_SIZE - 1) / BLOCK_SIZE;
    int extra = needBlocks - currentBlocks;

    if(extra < 0)
    {
        extra = 0;
    }

    if(extra > countFreeBlocks())
    { 
        printf("Disk full! Cannot write data.\n");
        return;
    }

    if(file->blockPointers == NULL && needBlocks > 0)
    {
        file->blockPointers = (int*)malloc(sizeof(int) * needBlocks);
        file->dynamicBlocksCount = 0;

        for(int index = 0; index < needBlocks; index++)
        {
            file->blockPointers[index] = -1;
        }
    }
    else if(extra > 0)
    {
        int *np = (int*)realloc(file->blockPointers, sizeof(int) * needBlocks);
        if(!np)
        {
            printf("Memory error.\n");
            return; 
        }

        file->blockPointers = np;

        for(int index = file->dynamicBlocksCount; index < needBlocks; index++)
        {
            file->blockPointers[index] = -1;
        }
    }

    for(int i = currentBlocks; i < needBlocks; i++)
    {
        int index = popFreeBlockHead();
        if(index == -1)
        {
            printf("Disk full! Cannot write data.\n");
            return;
        }
        file->blockPointers[i] = index;
    }

    if(needBlocks > file->dynamicBlocksCount)
    {
        file->dynamicBlocksCount = needBlocks;
    }

    const unsigned char *source = (const unsigned char*)data;
    int remaining = addBytes;

    if(off != 0 && currentBlocks > 0)
    {
        int index = file->blockPointers[currentBlocks - 1];
        int can = BLOCK_SIZE - off;
        int take = remaining < can ? remaining : can;
        memcpy(virtualDisk[index] + off, source, take);
        source += take;
        remaining -= take;
    }

    for(int i = currentBlocks; remaining > 0 && i < needBlocks; i++)
    {
        int index = file->blockPointers[i];
        int take = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        memcpy(virtualDisk[index], source, take);
        if(take < BLOCK_SIZE)
        {
            memset(virtualDisk[index] + take, 0, BLOCK_SIZE - take);
        }
        source += take;
        remaining -= take;
    }

    file->fileSize = totalSize;
    printf("Data written successfully (size=%d bytes).\n", addBytes);
}

void writeCommand(char *filename, char *data)
{
    if(filename == NULL)
    {
        printf("write: missing operand\n");
        return;
    }

    if(is_bad_name(filename))
    {
        printf("write: invalid filename\n");
        return;
    }

    FileNode *file = findChild(currentDirectory, filename);

    if(file == NULL || file->isDirectory)
    {
        printf("write: no such file: %s\n", filename);
        return;
    }

    if(!data)
    {
        data = "";
    }

    writeAppend(file, data);
}

int allocateBlocksForFile(FileNode *file, int blocksNeeded)
{
    if(file == NULL || file->isDirectory || blocksNeeded <= 0)
    {
        return 0;
    }

    file->blockPointers = (int *)malloc(sizeof(int) * blocksNeeded);
    file->dynamicBlocksCount = blocksNeeded;

    for(int i = 0; i < blocksNeeded; i++)
    {
        int blockIndex = popFreeBlockHead();
        if(blockIndex == -1)
        {
            for(int j = 0; j < i; j++)
            {
                pushFreeBlockTail(file->blockPointers[j]);
            }
            free(file->blockPointers);
            file->blockPointers = NULL;
            file->dynamicBlocksCount = 0;
            return 0;
        }
        file->blockPointers[i] = blockIndex;
    }

    return blocksNeeded;
}

void mkdirCommand(char *name)
{
    if(name == NULL)
    {
        printf("mkdir: missing operand\n");
        return;
    }

    if(findChild(currentDirectory, name) != NULL)
    {
        printf("mkdir: cannot create directory '%s': File exists\n", name);
        return;
    }

    FileNode *newDirectory = createFileNode(name, 1, currentDirectory);
    insertChild(currentDirectory, newDirectory);
    printf("Directory '%s' created.\n", name);
}

void lsCommand()
{
    if(currentDirectory->child == NULL)
    {
        printf("Directory is empty.\n");
        return;
    }

    FileNode *current = currentDirectory->child;
    do
    {
        if(current->isDirectory)
        {
            printf("[DIR] %s\n", current->name);
        }
        else
        {
            printf("[FILE] %s (Size: %d bytes)\n", current->name, current->fileSize);
        }
        current = current->next;
    }
    while(current != currentDirectory->child);
}

void cdCommand(char *name)
{
    if(name == NULL)
    {
        printf("cd: missing operand\n");
        return;
    }

    if(strcmp(name, "..") == 0)
    {
        if(currentDirectory->parent != NULL)
        {
            currentDirectory = currentDirectory->parent;
        }
        return;
    }

    FileNode *targetDirectory = findChild(currentDirectory, name);
    if(targetDirectory == NULL || !targetDirectory->isDirectory)
    {
        printf("cd: no such directory: %s\n", name);
        return;
    }

    currentDirectory = targetDirectory;
}

void createCommand(char *name)
{
    if(name == NULL)
    {
        printf("create: missing operand\n");
        return;
    }

    if(findChild(currentDirectory, name) != NULL)
    {
        printf("create: cannot create file '%s': File exists\n", name);
        return;
    }

    FileNode *newFile = createFileNode(name, 0, currentDirectory);
    insertChild(currentDirectory, newFile);
    printf("File '%s' created.\n", name);
}

FileNode* findFileInCurrentDirectory(char *name)
{
    return findChild(currentDirectory, name);
}

void readCommand(char *name)
{
    if(name == NULL)
    {
        printf("read: missing operand\n");
        return;
    }

    FileNode *file = findFileInCurrentDirectory(name);
    if(file == NULL || file->isDirectory)
    {
        printf("read: no such file: %s\n", name);
        return;
    }

    if(file->fileSize == 0)
    {
        printf("File '%s' is empty.\n", name);
        return;
    }

    printf("Contents of file '%s':\n", name);
    int bytesRead = 0;

    for(int index = 0; index < file->dynamicBlocksCount; index++)
    {
        int blockIndex = file->blockPointers[index];
        int bytesToRead = (file->fileSize - bytesRead < BLOCK_SIZE) ? (file->fileSize - bytesRead) : BLOCK_SIZE;
        fwrite(virtualDisk[blockIndex], 1, bytesToRead, stdout);
        bytesRead += bytesToRead;

        if(bytesRead >= file->fileSize)
        {
            break;
        }
    }

    printf("\n");
}

void deleteCommand(char *name)
{
    if(name == NULL)
    {
        printf("delete: missing operand\n");
        return;
    }

    FileNode *file = findFileInCurrentDirectory(name);
    if(file == NULL || file->isDirectory)
    {
        printf("delete: no such file: %s\n", name);
        return;
    }

    int freedBlocks = freeBlockOfFile(file);
    unlinkChildNode(currentDirectory, file);
    free(file);
    printf("File '%s' deleted, freed %d blocks.\n", name, freedBlocks);
}

void rmdirCommand(char *name)
{
    if(name == NULL)
    {
        printf("rmdir: missing operand\n");
        return;
    }

    FileNode *directory = findChild(currentDirectory, name);
    if(directory == NULL || !directory->isDirectory)
    {
        printf("rmdir: no such directory: %s\n", name);
        return;
    }

    if(directory->child != NULL)
    {
        printf("rmdir: directory '%s' is not empty\n", name);
        return;
    }

    unlinkChildNode(currentDirectory, directory);
    free(directory);
    printf("Directory '%s' removed.\n", name);
}

void pwdCommand()
{
    if(currentDirectory == root)
    {
        printf("Current Directory: /\n");
        return;
    }

    FileNode *temproraryDirectory = currentDirectory;
    char path[MAX_LINE] = "";

    while(temproraryDirectory != NULL && temproraryDirectory != root)
    {
        char temproraryPath[MAX_LINE];
        snprintf(temproraryPath, sizeof(temproraryPath), "/%s%s", temproraryDirectory->name, path);
        strncpy(path, temproraryPath, sizeof(path) - 1);
        path[sizeof(path) - 1] = '\0';
        temproraryDirectory = temproraryDirectory->parent;
    }

    printf("Current Directory: %s\n", path[0] ? path : "/");
}

void dfCommand()
{
    int freeBlocks = countFreeBlocks();
    int usedBlocks = NUMBER_OF_BLOCKS - freeBlocks;
    float usagePercent = ((float)usedBlocks / NUMBER_OF_BLOCKS) * 100.0;

    printf("Disk Usage:\n");
    printf("Total Blocks: %d\n", NUMBER_OF_BLOCKS);
    printf("Used Blocks: %d\n", usedBlocks);
    printf("Free Blocks: %d\n", freeBlocks);
    printf("Usage: %.2f%%\n", usagePercent);
}

void exitCommand()
{
    freeFileSystem(root);
    freeFreeBlockList();
    printf("Exiting Virtual File System. Goodbye!\n");
    exit(0);
}

void processCommand(char *line)
{
    rightTrim(line);

    char *command = strtok(line, " ");
    if(command == NULL)
    {
        return;
    }

    if(strcmp(command, "mkdir") == 0)
    {
        char *argument = strtok(NULL, " \t\n");
        mkdirCommand(argument);
    }
    else if(strcmp(command, "ls") == 0)
    {
        lsCommand();
    }
    else if(strcmp(command, "cd") == 0)
    {
        char *argument = strtok(NULL, " \t\n");
        if(argument != NULL)
        {
            cdCommand(argument);
        }
        else
        {
            printf("cd: missing operand\n");
        }
    }
    else if(strcmp(command, "create") == 0)
    {
        char *argument = strtok(NULL, " \t\n");
        createCommand(argument);
    }
    else if(strcmp(command, "write") == 0)
    {
        char *filename = strtok(NULL, " \t\n");
        char *data = strtok(NULL, "\n");        
        if(filename != NULL && data != NULL)
        {
            writeCommand(filename, data);
        }
        else
        {
            printf("write: missing operand\n");
        }
    }
    else if(strcmp(command, "read") == 0)
    {
        char *argument = strtok(NULL, " \t\n");
        readCommand(argument);
    }
    else if(strcmp(command, "delete") == 0)
    {
        char *argument = strtok(NULL, " \t\n");
        deleteCommand(argument);
    }
    else if(strcmp(command, "rmdir") == 0)
    {
        char *argument = strtok(NULL, " \t\n");
        rmdirCommand(argument);
    }
    else if(strcmp(command, "pwd") == 0)
    {
        pwdCommand();
    }
    else if(strcmp(command, "df") == 0)
    {
        dfCommand();
    }
    else if(strcmp(command, "exit") == 0)
    {
        exitCommand();
    }
    else
    {
        printf("Invalid command!\n");
    }
}

void initializeRootDirectory()
{
    root = createFileNode("", 1, NULL);
    root->next = root->prev = root;
    root->child = NULL;
    currentDirectory = root;
}

void initializeFileSystem()
{
    initializeFreeBlockList();
    initializeRootDirectory();
    printf("Virtual File System Initialized. type 'exit' to quit.\n");
}

int main()
{
    initializeFileSystem();
    char line[MAX_LINE];

    while(1)
    {
        if(currentDirectory == root)
        {
            printf("/ > ");
        }
        else
        {
            printf("/%s > ", currentDirectory->name);
        }

        if(fgets(line, sizeof(line), stdin) == NULL)
        {
            printf("\n");
            exitCommand();
        }

        processCommand(line);
    }

    return 0;
}
