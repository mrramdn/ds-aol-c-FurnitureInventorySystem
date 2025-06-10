#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 50
#define MAX_CATEGORY 30
#define HASH_SIZE 10
#define MAX_QUEUE 100
#define MAX_ARRAY 100

// ==================== STRUCTURE DEFINITIONS =======================
typedef struct Furniture {
    int id;
    char name[MAX_NAME];
    char category[MAX_CATEGORY];
    float price;
    int quantity;
    struct Furniture *next;
} Furniture;