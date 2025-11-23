#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define HASH_MAP_SIZE 1009
#define MAX_KEY_LENGTH 20
#define MAX_VALUE_LENGTH 100
#define MAX_CAPACITY 1000
#define MIN_CAPACITY 1

typedef struct CacheNode 
{
    int key;
    char* value;
    struct CacheNode* previousNode;
    struct CacheNode* nextNode;
} CacheNode;

typedef struct 
{
    CacheNode* head;
    CacheNode* tail;
    CacheNode** hashMap;
    int capacity;
    int size;
} LRUCache;

static CacheNode* createNode(int key, const char* value)
{
    CacheNode* newNode = (CacheNode*)malloc(sizeof(CacheNode));
    if (!newNode) return NULL;
    
    newNode->key = key;
    newNode->value = strdup(value);
    if (!newNode->value) 
    {
        free(newNode);
        return NULL;
    }
    newNode->previousNode = NULL;
    newNode->nextNode = NULL;
    return newNode;
}

static void removeNode(LRUCache* cache, CacheNode* node) 
{
    if (node->previousNode) 
    {
        node->previousNode->nextNode = node->nextNode;
    } 
    else 
    {
        cache->head = node->nextNode;
    }
    
    if (node->nextNode) 
    {
        node->nextNode->previousNode = node->previousNode;
    } 
    else 
    {
        cache->tail = node->previousNode;
    }
    
    cache->size--;
}

static void addToFront(LRUCache* cache, CacheNode* node) 
{
    node->nextNode = cache->head;
    node->previousNode = NULL;
    
    if (cache->head) 
    {
        cache->head->previousNode = node;
    }
    cache->head = node;
    
    if (!cache->tail) 
    {
        cache->tail = node;
    }
    
    cache->size++;
}

static void moveToFront(LRUCache* cache, CacheNode* node) 
{
    if (cache->head == node) return;
    
    removeNode(cache, node);
    addToFront(cache, node);
}

LRUCache* createCache(int capacity) 
{
    if (capacity < MIN_CAPACITY || capacity > MAX_CAPACITY) 
    {
        return NULL;
    }

    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    if (!cache) return NULL;
    
    cache->hashMap = (CacheNode**)calloc(HASH_MAP_SIZE, sizeof(CacheNode*));
    if (!cache->hashMap) 
    {
        free(cache);
        return NULL;
    }
    
    cache->head = NULL;
    cache->tail = NULL;
    cache->capacity = capacity;
    cache->size = 0;
    
    return cache;
}

char* get(LRUCache* cache, int key) 
{
    if (!cache || key < 0) return NULL;
    
    int hashIndex = key % HASH_MAP_SIZE;
    CacheNode* currentNode = cache->hashMap[hashIndex];
    
    while (currentNode) 
    {
        if (currentNode->key == key) 
        {
            moveToFront(cache, currentNode);
            return currentNode->value;
        }
        currentNode = currentNode->nextNode;
    }
    
    return NULL;
}

void put(LRUCache* cache, int key, const char* value) 
{
    if (!cache || key < 0 || !value) return;
    
    int hashIndex = key % HASH_MAP_SIZE;
    CacheNode* currentNode = cache->hashMap[hashIndex];
    CacheNode* previousNode = NULL;
    
    while (currentNode) 
    {
        if (currentNode->key == key) 
        {
            free(currentNode->value);
            currentNode->value = strdup(value);
            moveToFront(cache, currentNode);
            return;
        }
        previousNode = currentNode;
        currentNode = currentNode->nextNode;
    }
    
    CacheNode* newNode = createNode(key, value);
    if (!newNode) return;
    
    if (cache->size >= cache->capacity) 
    {
        CacheNode* lruNode = cache->tail;
        if (lruNode) 
        {
            int lruHashIndex = lruNode->key % HASH_MAP_SIZE;
            CacheNode* current = cache->hashMap[lruHashIndex];
            CacheNode* prev = NULL;
            
            while (current)
            {
                if (current == lruNode)
                {
                    if (prev)
                    {
                        prev->nextNode = current->nextNode;
                    } 
                    else 
                    {
                        cache->hashMap[lruHashIndex] = current->nextNode;
                    }
                    break;
                }
                prev = current;
                current = current->nextNode;
            }
            
            removeNode(cache, lruNode);
            free(lruNode->value);
            free(lruNode);
        }
    }
    
    newNode->nextNode = cache->hashMap[hashIndex];
    cache->hashMap[hashIndex] = newNode;
    
    addToFront(cache, newNode);
}

void freeCache(LRUCache* cache) 
{
    if (!cache) return;
    
    CacheNode* currentNode = cache->head;
    while (currentNode) 
    {
        CacheNode* nextNode = currentNode->nextNode;
        free(currentNode->value);
        free(currentNode);
        currentNode = nextNode;
    }
    
    free(cache->hashMap);
    free(cache);
}

bool isNumber(const char* str) 
{
    if (!str || *str == '\0') return false;
    for (int i = 0; str[i] != '\0'; i++) 
    {
        if (!isdigit(str[i])) return false;
    }
    return true;
}

int main() 
{
    printf("LRU Cache Implementation\n");
    printf("Available commands:\n");
    printf("1. createCache <capacity>\n");
    printf("2. put <key> <value>\n");
    printf("3. get <key>\n");
    printf("4. exit\n\n");
    
    LRUCache* cache = NULL;
    char inputBuffer[256];
    char command[20];
    char keyStr[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
    int key, capacity;
    
    while (1) 
    {
        printf("Command> ");
        if (fgets(inputBuffer, sizeof(inputBuffer), stdin) == NULL) 
        {
            printf("Error reading input\n");
            continue;
        }
        
        inputBuffer[strcspn(inputBuffer, "\n")] = '\0';
        
        if (sscanf(inputBuffer, "%19s", command) != 1) 
        {
            printf("Invalid command\n");
            continue;
        }
        
        if (strcmp(command, "createCache") == 0) 
        {
            if (sscanf(inputBuffer + strlen(command), "%19s", keyStr) != 1) 
            {
                printf("Error: Missing capacity\n");
                continue;
            }
            
            if (!isNumber(keyStr)) 
            {
                printf("Error: Capacity must be a positive number\n");
                continue;
            }
            
            capacity = atoi(keyStr);
            if (capacity < MIN_CAPACITY || capacity > MAX_CAPACITY) 
            {
                printf("Error: Capacity must be between %d and %d\n", MIN_CAPACITY, MAX_CAPACITY);
                continue;
            }
            
            if (cache) 
            {
                freeCache(cache);
            }
            
            cache = createCache(capacity);
            if (cache) 
            {
                printf("Cache created with capacity %d\n", capacity);
            } 
            else 
            {
                printf("Failed to create cache\n");
            }
        }
        else if (strcmp(command, "put") == 0) 
        {
            if (sscanf(inputBuffer + strlen(command), "%19s %99[^\n]", keyStr, value) != 2) 
            {
                printf("Error: Invalid arguments for put command\n");
                continue;
            }
            
            if (!isNumber(keyStr)) 
            {
                printf("Error: Key must be a number\n");
                continue;
            }
            
            key = atoi(keyStr);
            if (key < 0) 
            {
                printf("Error: Key must be non-negative\n");
                continue;
            }
            
            if (cache) 
            {
                put(cache, key, value);
                printf("Inserted\n");
            } 
            else 
            {
                printf("Error: Cache not initialized. Use 'createCache' first.\n");
            }
        }
        else if (strcmp(command, "get") == 0) 
        {
            if (sscanf(inputBuffer + strlen(command), "%19s", keyStr) != 1) 
            {
                printf("Error: Missing key\n");
                continue;
            }
            
            if (!isNumber(keyStr)) 
            {
                printf("Error: Key must be a number\n");
                continue;
            }
            
            key = atoi(keyStr);
            if (key < 0) 
            {
                printf("Error: Key must be non-negative\n");
                continue;
            }
            
            if (cache) 
            {
                char* result = get(cache, key);
                printf("%s\n", result ? result : "NULL");
            } 
            else 
            {
                printf("Error: Cache not initialized. Use 'createCache' first.\n");
            }
        }
        else if (strcmp(command, "exit") == 0) 
        {
            break;
        }
        else 
        {
            printf("Invalid command\n");
        }
    }
    
    if (cache) 
    {
        freeCache(cache);
    }
    
    return 0;
}