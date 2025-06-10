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
    int price;
    int quantity;
    struct Furniture *next;
} Furniture;

// Linked List - Main Inventory
Furniture *inventory = NULL;

// Stack - Undo Delete
typedef struct StackNode {
    Furniture data;
    struct StackNode *next;
} StackNode;
StackNode *undoStack = NULL;

// Queue - Incoming Items
Furniture *queue[MAX_QUEUE];
int front = -1, rear = -1;

// BST - Search by Name
typedef struct BSTNode {
    Furniture *data;
    struct BSTNode *left, *right;
} BSTNode;
BSTNode *bstRoot = NULL;

// Hash Table - Group by Category
Furniture *hashTable[HASH_SIZE] = {NULL};

// ==================== UTILITY FUNCTIONS =======================
unsigned int hash(const char *key) {
    unsigned int value = 0;
    while (*key) value += *key++;
    return value % HASH_SIZE;
}

void insertToHash(Furniture *item) {
    Furniture *newItem = (Furniture *)malloc(sizeof(Furniture));
    *newItem = *item;
    newItem->next = hashTable[hash(item->category)];
    hashTable[hash(item->category)] = newItem;
}


void insertToBST(Furniture *item) {
    BSTNode *newNode = (BSTNode *)malloc(sizeof(BSTNode));
    newNode->data = item;
    newNode->left = newNode->right = NULL;

    if (!bstRoot) {
        bstRoot = newNode;
        return;
    }
    BSTNode *curr = bstRoot, *parent = NULL;
    while (curr) {
        parent = curr;
        if (strcmp(item->name, curr->data->name) < 0)
            curr = curr->left;
        else
            curr = curr->right;
    }
    if (strcmp(item->name, parent->data->name) < 0)
        parent->left = newNode;
    else
        parent->right = newNode;
}

Furniture *searchBST(BSTNode *root, const char *name) {
    if (!root) return NULL;
    int cmp = strcmp(name, root->data->name);
    if (cmp == 0) return root->data;
    else if (cmp < 0) return searchBST(root->left, name);
    else return searchBST(root->right, name);
}

void pushUndo(Furniture item) {
    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    node->data = item;
    node->next = undoStack;
    undoStack = node;
}

int popUndo(Furniture *item) {
    if (!undoStack) return 0;
    *item = undoStack->data;
    StackNode *temp = undoStack;
    undoStack = undoStack->next;
    free(temp);
    return 1;
}

void enqueueFurniture(Furniture *item) {
    if ((rear + 1) % MAX_QUEUE == front) return; // full
    if (front == -1) front = 0;
    rear = (rear + 1) % MAX_QUEUE;
    queue[rear] = item;
}

Furniture *dequeueFurniture() {
    if (front == -1) return NULL;
    Furniture *item = queue[front];
    if (front == rear) front = rear = -1;
    else front = (front + 1) % MAX_QUEUE;
    return item;
}

void cleanup() {
    Furniture *temp;
    while (inventory) {
        temp = inventory;
        inventory = inventory->next;
        free(temp);
    }
    while (undoStack) {
        StackNode *temp = undoStack;
        undoStack = undoStack->next;
        free(temp);
    }
}

// ==================== FUNCTION IMPLEMENTATIONS =======================

int isIDExist(int id) {
        Furniture *temp = inventory;
        while (temp) {
            if (temp->id == id) return 1;
            temp = temp->next;
        }
        return 0;
    }
    
int validateID(int id) {
    if (id <= 0) {
        printf("Error: ID must be a positive number.\n");
        return 0;
    }
    if (isIDExist(id)) {
        printf("Error: ID %d already exists.\n", id);
        return 0;
    }
    return 1;
}

void addFurniture() {
    int tempID;
    printf("Enter ID: "); 
    if (scanf("%d", &tempID) != 1) {
        printf("Error: Invalid ID format.\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    
    if (!validateID(tempID)) {
        return;
    }

    Furniture *item = (Furniture *)malloc(sizeof(Furniture));
    if (!item) {
        printf("Error: Memory allocation failed.\n");
        return;
    }

    item->id = tempID;
    getchar();
    printf("Enter Name: "); 
    if (!fgets(item->name, MAX_NAME, stdin)) {
        printf("Error: Invalid name input.\n");
        free(item);
        return;
    }
    item->name[strcspn(item->name, "\n")] = 0;
    
    printf("Enter Category: "); 
    if (!fgets(item->category, MAX_CATEGORY, stdin)) {
        printf("Error: Invalid category input.\n");
        free(item);
        return;
    }
    item->category[strcspn(item->category, "\n")] = 0;
    
    printf("Enter Price: "); 
    if (scanf("%d", &item->price) != 1 || item->price < 0) {
        printf("Error: Invalid price.\n");
        free(item);
        return;
    }
    
    printf("Enter Quantity: "); 
    if (scanf("%d", &item->quantity) != 1 || item->quantity < 0) {
        printf("Error: Invalid quantity.\n");
        free(item);
        return;
    }

    item->next = inventory;
    inventory = item;
    enqueueFurniture(item);
    insertToBST(item);
    insertToHash(item);
    printf("Furniture added successfully!\n");
}

void updateFurniture() {
    int id;
    printf("Enter ID to update: ");
    scanf("%d", &id);
    Furniture *temp = inventory;
    while (temp) {
        if (temp->id == id) {
            getchar(); // flush
            printf("Enter New Name: "); fgets(temp->name, MAX_NAME, stdin);
            temp->name[strcspn(temp->name, "\n")] = 0;
            printf("Enter New Category: "); fgets(temp->category, MAX_CATEGORY, stdin);
            temp->category[strcspn(temp->category, "\n")] = 0;
            printf("Enter New Price: "); scanf("%d", &temp->price);
            printf("Enter New Quantity: "); scanf("%d", &temp->quantity);
            printf("Furniture updated successfully.\n");
            return;
        }
        temp = temp->next;
    }
    printf("Furniture with ID %d not found.\n", id);
}


void displayInventory() {
    Furniture *temp = inventory;
    printf("\n--- Current Inventory ---\n");
    while (temp) {
        printf("ID: %d | Name: %s | Category: %s | Price: %d | Qty: %d\n",
               temp->id, temp->name, temp->category, temp->price, temp->quantity);
        temp = temp->next;
    }
}

void displayMenu() {
    printf("\n==== Warehouse Furniture Management System ====\n");
    printf("1. Inventory Management\n");
    printf("2. Search & View\n");
    printf("3. Sort & Export\n");
    printf("4. Exit\n");
    printf("Choose an option: ");
}

void displayInventoryMenu() {
    printf("\n=== Inventory Management ===\n");
    printf("1. Add Furniture\n");
    printf("2. Update Furniture\n");
    printf("3. Display Inventory\n");
    printf("4. Delete Furniture\n");
    printf("5. Undo Delete\n");
    printf("6. Back to Main Menu\n");
    printf("Choose an option: ");
}

void displaySearchMenu() {
    printf("\n=== Search & View ===\n");
    printf("1. Search by ID\n");
    printf("2. Search by Name\n");
    printf("3. View by Category\n");
    printf("4. Back to Main Menu\n");
    printf("Choose an option: ");
}

void displaySortMenu() {
    printf("\n=== Sort & Export ===\n");
    printf("1. Sort Inventory\n");
    printf("2. Export to File\n");
    printf("3. Back to Main Menu\n");
    printf("Choose an option: ");
}

void displaySortCriteria() {
    printf("\nSort By:\n");
    printf("1. ID\n");
    printf("2. Name\n");
    printf("3. Category\n");
    printf("4. Price\n");
    printf("5. Quantity\n");
    printf("Choose sorting criteria: ");
}

void sortFurniture(int choice) {
    if (!inventory || !inventory->next) {
        printf("Not enough items to sort.\n");
        return;
    }

    Furniture *current, *next;
    int swapped;
    
    do {
        swapped = 0;
        current = inventory;
        
        while (current->next != NULL) {
            next = current->next;
            int shouldSwap = 0;
            
            switch (choice) {
                case 1:
                    shouldSwap = current->id > next->id;
                    break;
                case 2:
                    shouldSwap = strcmp(current->name, next->name) > 0;
                    break;
                case 3:
                    shouldSwap = strcmp(current->category, next->category) > 0;
                    break;
                case 4:
                    shouldSwap = current->price > next->price;
                    break;
                case 5:
                    shouldSwap = current->quantity > next->quantity;
                    break;
                default:
                    printf("Invalid sorting choice.\n");
                    return;
            }
            
            if (shouldSwap) {
                // Swap data
                Furniture temp = *current;
                *current = *next;
                *next = temp;
                
                // Preserve next pointers
                Furniture *tempNext = current->next;
                current->next = next->next;
                next->next = tempNext;
                
                swapped = 1;
            }
            current = current->next;
        }
    } while (swapped);
    
    printf("Inventory sorted successfully.\n");
    displayInventory();
}

void deleteFurniture() {
    int id;
    printf("Enter ID to delete: ");
    scanf("%d", &id);
    Furniture *curr = inventory, *prev = NULL;
    while (curr && curr->id != id) {
        prev = curr;
        curr = curr->next;
    }
    if (!curr) {
        printf("Not found.\n"); return;
    }
    if (!prev) inventory = curr->next;
    else prev->next = curr->next;
    pushUndo(*curr);
    printf("Furniture deleted.\n");
    free(curr);
}

void undoDelete() {
    Furniture item;
    if (popUndo(&item)) {
        Furniture *restored = (Furniture *)malloc(sizeof(Furniture));
        *restored = item;
        restored->next = inventory;
        inventory = restored;
        insertToBST(restored);
        insertToHash(restored);
        printf("Undo successful.\n");
    } else printf("No undo available.\n");
}

void searchFurnitureByID() {
    int id;
    printf("Enter ID: "); scanf("%d", &id);
    Furniture *temp = inventory;
    while (temp) {
        if (temp->id == id) {
            printf("Found: %s | %s | %d | %d\n", temp->name, temp->category, temp->price, temp->quantity);
            return;
        }
        temp = temp->next;
    }
    printf("Not found.\n");
}

void searchFurnitureByName() {
    char name[MAX_NAME];
    getchar();
    printf("Enter Name: "); fgets(name, MAX_NAME, stdin);
    name[strcspn(name, "\n")] = 0;
    Furniture *result = searchBST(bstRoot, name);
    if (result)
        printf("Found: %d | %s | %s | %d | %d\n", result->id, result->name, result->category, result->price, result->quantity);
    else
        printf("Not found.\n");
}

void viewByCategory() {
    char cat[MAX_CATEGORY];
    getchar();
    printf("Enter Category: "); fgets(cat, MAX_CATEGORY, stdin);
    cat[strcspn(cat, "\n")] = 0;
    unsigned int index = hash(cat);
    Furniture *temp = hashTable[index];
    while (temp) {
        if (strcmp(temp->category, cat) == 0)
            printf("%d | %s | %d | %d\n", temp->id, temp->name, temp->price, temp->quantity);
        temp = temp->next;
    }
}

void exportToFile() {
    FILE *f = fopen("furniture_seed.txt", "w");
    Furniture *temp = inventory;
    while (temp) {
        fprintf(f, "%d|%s|%s|%d|%d\n", temp->id, temp->name, temp->category, temp->price, temp->quantity);
        temp = temp->next;
    }
    fclose(f);
    printf("Exported to furniture_seed.txt\n");
}

void loadFromFile() {
    FILE *f = fopen("furniture_seed.txt", "r");
    if (!f) {
        printf("No seed file found. Skipping load.\n");
        return;
    }
    
    char line[256];
    while (fgets(line, sizeof(line), f)) {
        Furniture *item = (Furniture *)malloc(sizeof(Furniture));
        if (sscanf(line, "%d|%49[^|]|%29[^|]|%d|%d",
                   &item->id, item->name, item->category, &item->price, &item->quantity) == 5) {
            item->next = inventory;
            inventory = item;
            enqueueFurniture(item);
            insertToBST(item);
            insertToHash(item);
        } else {
            free(item);
        }
    }
    fclose(f);
    printf("Seed data loaded successfully.\n");
}

void handleInventoryMenu() {
    int choice;
    do {
        displayInventoryMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1: addFurniture(); break;
            case 2: updateFurniture(); break;
            case 3: displayInventory(); break;
            case 4: deleteFurniture(); break;
            case 5: undoDelete(); break;
            case 6: return;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (1);
}

void handleSearchMenu() {
    int choice;
    do {
        displaySearchMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1: searchFurnitureByID(); break;
            case 2: searchFurnitureByName(); break;
            case 3: viewByCategory(); break;
            case 4: return;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (1);
}

void handleSortMenu() {
    int choice;
    do {
        displaySortMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }
        
        switch (choice) {
            case 1: {
                int sortChoice;
                displaySortCriteria();
                if (scanf("%d", &sortChoice) != 1) {
                    printf("Invalid sorting choice.\n");
                    while (getchar() != '\n');
                    break;
                }
                sortFurniture(sortChoice);
                break;
            }
            case 2: exportToFile(); break;
            case 3: return;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (1);
}

void menu() {
    int choice;
    loadFromFile();
    
    do {
        displayMenu();
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1: handleInventoryMenu(); break;
            case 2: handleSearchMenu(); break;
            case 3: handleSortMenu(); break;
            case 4: printf("Thank you for using the system. Goodbye!\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);
}

int main() {
    menu();  // Fungsi utama untuk menjalankan program
    cleanup(); // Optional: membersihkan memory sebelum keluar
    return 0;
}
