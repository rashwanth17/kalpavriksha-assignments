#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Node 
{
    int key;
    char* value;
    struct Node* prev;
    struct Node* next;
} Node;

typedef struct 
{
    Node* head;
    Node* tail;
    Node** hashMap;
    int capacity;
    int size;
} LRUCache;

static Node* createNode(int key, const char* value)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode)
    {
        return NULL;
    }
    
    newNode->key = key;
    newNode->value = strdup(value);
    if (!newNode->value) 
    {
        free(newNode);
        return NULL;
    }
    newNode->prev = NULL;
    newNode->next = NULL;
    return newNode;
}

static void removeNode(LRUCache* cache, Node* node) 
{
    if (node->prev) 
    {
        node->prev->next = node->next;
    } 
    else 
    {
        cache->head = node->next;
    }
    
    if (node->next) 
    {
        node->next->prev = node->prev;
    } 
    else 
    {
        cache->tail = node->prev;
    }
    
    cache->size--;
}

static void addToFront(LRUCache* cache, Node* node) 
{
    node->next = cache->head;
    node->prev = NULL;
    
    if (cache->head) 
    {
        cache->head->prev = node;
    }
    cache->head = node;
    
    if (!cache->tail) 
    {
        cache->tail = node;
    }
    
    cache->size++;
}

static void moveToFront(LRUCache* cache, Node* node) 
{
    if (cache->head == node)
    {
        return;
    }
    
    removeNode(cache, node);
    addToFront(cache, node);
}

LRUCache* createCache(int capacity) 
{
    if (capacity <= 0) 
    {
        fprintf(stderr, "Error: Capacity must be a positive integer\n");
        return NULL;
    }

    LRUCache* cache = (LRUCache*)malloc(sizeof(LRUCache));
    if (!cache)
    {
        return NULL;
    }
    
    int hashSize = 1009;
    cache->hashMap = (Node**)calloc(hashSize, sizeof(Node*));
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

char* get(LRUCache* cache, int key) {
    if (!cache || key < 0)
    {
        return NULL;
    } 
    
    int hashIndex = key % 1009;
    Node* node = cache->hashMap[hashIndex];
    
    while (node) 
    {
        if (node->key == key) 
        {
            moveToFront(cache, node);
            return node->value;
        }
        node = node->next;
    }
    
    return NULL;
}

void put(LRUCache* cache, int key, const char* value) {
    if (!cache || key < 0 || !value)
    {
        return;
    } 
    
    int hashIndex = key % 1009;
    Node* node = cache->hashMap[hashIndex];
    Node* prev = NULL;
    
    while (node) 
    {
        if (node->key == key) 
        {
            free(node->value);
            node->value = strdup(value);
            moveToFront(cache, node);
            return;
        }
        prev = node;
        node = node->next;
    }
    
    node = createNode(key, value);
    if (!node) 
    {
        return;
    }
    
    if (cache->size >= cache->capacity) 
    {
        Node* lru = cache->tail;
        if (lru) 
        {
            int lruHashIndex = lru->key % 1009;
            Node* curr = cache->hashMap[lruHashIndex];
            Node* prev = NULL;
            
            while (curr)
            {
                if (curr == lru)
                {
                    if (prev)
                    {
                        prev->next = curr->next;
                    } 
                    else 
                    {
                        cache->hashMap[lruHashIndex] = curr->next;
                    }
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
            
            removeNode(cache, lru);
            free(lru->value);
            free(lru);
        }
    }
    
    node->next = cache->hashMap[hashIndex];
    cache->hashMap[hashIndex] = node;
    
    addToFront(cache, node);
}

void freeCache(LRUCache* cache) {
    if (!cache)
    {
        return;
    } 
    
    Node* curr = cache->head;
    while (curr) 
    {
        Node* next = curr->next;
        free(curr->value);
        free(curr);
        curr = next;
    }
    
    free(cache->hashMap);
    free(cache);
}

int main() {
    printf("LRU Cache Implementation\n");
    printf("Available commands:\n");
    printf("1. createCache <capacity>\n");
    printf("2. put <key> <value>\n");
    printf("3. get <key>\n");
    printf("4. exit\n\n");
    
    LRUCache* cache = NULL;
    char command[20];
    int key, capacity;
    char value[100];
    
    while (1) 
    {
        printf("Command> ");
        if (scanf("%s", command) != 1) 
        {
            printf("Error reading command\n");
            while (getchar() != '\n'); 
            continue;
        }
        if (strcmp(command, "createCache") == 0) 
        {
            if (scanf("%d", &capacity) != 1)
            {
                printf("Error: Invalid capacity\n");
                while (getchar() != '\n'); 
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
            if (scanf("%d %s", &key, value) != 2)
            {
                printf("Error: Invalid arguments for put command\n");
                while (getchar() != '\n');
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
            if (scanf("%d", &key) != 1)
            {
                printf("Error: Invalid key\n");
                while (getchar() != '\n'); 
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
            while (getchar() != '\n'); 
        }
    }
    
    if (cache) 
    {
        freeCache(cache);
    }
    
    return 0;
}