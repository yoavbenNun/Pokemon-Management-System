#include "ex6.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

# define INT_BUFFER 128
# define POKE_NUMS 151

// ================================================
// Basic struct definitions from ex6.h assumed:
//   PokemonData { int id; char *name; PokemonType TYPE; int hp; int attack; EvolutionStatus CAN_EVOLVE; }
//   PokemonNode { PokemonData* data; PokemonNode* left, *right; }
//   OwnerNode   { char* ownerName; PokemonNode* pokedexRoot; OwnerNode *next, *prev; }
//   OwnerNode* ownerHead;
//   const PokemonData pokedex[];
// ================================================

// --------------------------------------------------------------
// 1) Safe integer reading
// --------------------------------------------------------------

void trimWhitespace(char *str)
{
    // Remove leading spaces/tabs/\r
    int start = 0;
    while (str[start] == ' ' || str[start] == '\t' || str[start] == '\r')
        start++;

    if (start > 0)
    {
        int idx = 0;
        while (str[start])
            str[idx++] = str[start++];
        str[idx] = '\0';
    }

    // Remove trailing spaces/tabs/\r
    int len = (int)strlen(str);
    while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\t' || str[len - 1] == '\r'))
    {
        str[--len] = '\0';
    }
}

char *myStrdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dest = (char *)malloc(len + 1);
    if (!dest)
    {
        printf("Memory allocation failed in myStrdup.\n");
        return NULL;
    }
    strcpy(dest, src);
    return dest;
}

void displayAlphabetical(PokemonNode* root) 
{
    // Nothing to display.
    if (!root) 
    {
        printf("Pokedex is empty.\n");
        return; 
    }

    // Initialize the dynamic array 
    NodeArray nodeArray;
    // Start with a capacity of 10 nodes.
    initNodeArray(&nodeArray, 10); 

    // Collect all nodes from the BST.
    collectAll(root, &nodeArray);

    // Sort the array alphabetically by Pokémon names.
    qsort(nodeArray.nodes, nodeArray.size, sizeof(PokemonNode*), compareByNameNode);

    // Display the sorted Pokémon.
    for (int i = 0; i < nodeArray.size; ++i) {
        printPokemonNode(nodeArray.nodes[i]);
    }

    // Free the dynamic array.
    free(nodeArray.nodes);
}

// Initialize a NodeArray.
void initNodeArray(NodeArray* na, int cap) 
{
    na->nodes = malloc(cap * sizeof(PokemonNode*));
    na->size = 0;
    na->capacity = cap;
}

// Add a node to the NodeArray, reallocating if necessary.
void addNode(NodeArray* na, PokemonNode* node) 
{
    if (na->size >= na->capacity) 
    {
        // Double the capacity.
        na->capacity *= 2; 
        na->nodes = realloc(na->nodes, na->capacity * sizeof(PokemonNode*));
        if (!na->nodes) 
        {
            printf("Memory allocation failed in addNode.\n");
            exit(1); 
        }
    }
    na->nodes[na->size++] = node;
}

// Recursively collect all nodes into the NodeArray.
void collectAll(PokemonNode* root, NodeArray* na) 
{
    if (!root) return;

    // Traverse left subtree.
    collectAll(root->left, na);

    // Add the current node.
    addNode(na, root);

    // Traverse right subtree.
    collectAll(root->right, na);
}

// Comparator for qsort to sort nodes by Pokémon name.
int compareByNameNode(const void* a, const void* b) 
{
    PokemonNode* nodeA = *(PokemonNode**)a;
    PokemonNode* nodeB = *(PokemonNode**)b;

    return strcmp(nodeA->data->name, nodeB->data->name);
}

int readIntSafe(const char *prompt)
{
    char buffer[INT_BUFFER];
    int value;
    int success = 0;

    while (!success)
    {
        printf("%s", prompt);

        // If we fail to read, treat it as invalid
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("Invalid input.\n");
            clearerr(stdin);
            continue;
        }

        // 1) Strip any trailing \r or \n
        //    so "123\r\n" becomes "123"
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
            buffer[--len] = '\0';
        if (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
            buffer[--len] = '\0';

        // 2) Check if empty after stripping
        if (len == 0)
        {
            printf("Invalid input.\n");
            continue;
        }

        // 3) Attempt to parse integer with strtol
        char *endptr;
        value = (int)strtol(buffer, &endptr, 10);

        // If endptr didn't point to the end => leftover chars => invalid
        // or if buffer was something non-numeric
        if (*endptr != '\0')
        {
            printf("Invalid input.\n");
        }
        else
        {
            // We got a valid integer
            success = 1;
        }
    }
    return value;
}

// --------------------------------------------------------------
// 2) Utility: Get type name from enum
// --------------------------------------------------------------
const char *getTypeName(PokemonType type)
{
    switch (type)
    {
    case GRASS:
        return "GRASS";
    case FIRE:
        return "FIRE";
    case WATER:
        return "WATER";
    case BUG:
        return "BUG";
    case NORMAL:
        return "NORMAL";
    case POISON:
        return "POISON";
    case ELECTRIC:
        return "ELECTRIC";
    case GROUND:
        return "GROUND";
    case FAIRY:
        return "FAIRY";
    case FIGHTING:
        return "FIGHTING";
    case PSYCHIC:
        return "PSYCHIC";
    case ROCK:
        return "ROCK";
    case GHOST:
        return "GHOST";
    case DRAGON:
        return "DRAGON";
    case ICE:
        return "ICE";
    default:
        return "UNKNOWN";
    }
}

// --------------------------------------------------------------
// Utility: getDynamicInput (for reading a line into malloc'd memory)
// --------------------------------------------------------------
char *getDynamicInput()
{
    char *input = NULL;
    size_t size = 0, capacity = 1;
    input = (char *)malloc(capacity);
    if (!input)
    {
        printf("Memory allocation failed.\n");
        return NULL;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        if (size + 1 >= capacity)
        {
            capacity *= 2;
            char* temp = (char*)realloc(input, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed.\n");
                free(input);
                return NULL;
            }
            input = temp;
        }
        input[size++] = (char)c;
    }
    input[size] = '\0';

    // Trim any leading/trailing whitespace or carriage returns
    trimWhitespace(input);

    return input;
}

// Function to print a single Pokemon node
void printPokemonNode(PokemonNode *node)
{
    if (!node)
        return;
    printf("ID: %d, Name: %s, Type: %s, HP: %d, Attack: %d, Can Evolve: %s\n",
           node->data->id,
           node->data->name,
           getTypeName(node->data->TYPE),
           node->data->hp,
           node->data->attack,
           (node->data->CAN_EVOLVE == CAN_EVOLVE) ? "Yes" : "No");
}

// --------------------------------------------------------------
// Display Menu
// --------------------------------------------------------------
void displayMenu(OwnerNode *owner)
{
    if (!owner->pokedexRoot)
    {
        printf("Pokedex is empty.\n");
        return;
    }

    printf("Display:\n");
    printf("1. BFS (Level-Order)\n");
    printf("2. Pre-Order\n");
    printf("3. In-Order\n");
    printf("4. Post-Order\n");
    printf("5. Alphabetical (by name)\n");

    int choice = readIntSafe("Your choice: ");

    switch (choice)
    {
    case 1:
        displayBFS(owner->pokedexRoot);
        break;
    case 2:
        preOrderTraversal(owner->pokedexRoot);
        break;
    case 3:
        inOrderTraversal(owner->pokedexRoot);
        break;
    case 4:
        postOrderTraversal(owner->pokedexRoot);
        break;
    case 5:
        displayAlphabetical(owner->pokedexRoot);
        break;
    default:
        printf("Invalid choice.\n");
    }
}

// --------------------------------------------------------------
// Sub-menu for existing Pokedex
// --------------------------------------------------------------
void enterExistingPokedexMenu()
{
    int index = 1;
    // list owners
    if (!ownerHead)
    {
        printf("No existing Pokedexes.\n");
        return;
    }
    printf("\nExisting Pokedexes:\n");
    OwnerNode* cur = ownerHead;
    do
    {
        printf("%d. %s\n", index, cur->ownerName);
        cur = cur->next;
        index++;

    } while (cur != ownerHead);

    printf("Choose a Pokedex by number:");
    int choice = readIntSafe("");
    cur = ownerHead;
    for (int i = 1; i < choice;i++)
    {
        cur = cur->next;
    }

    
    printf("\nEntering %s's Pokedex...\n", cur->ownerName);

    int subChoice;
    do
    {
        printf("\n-- %s's Pokedex Menu --\n", cur->ownerName);
        printf("1. Add Pokemon\n");
        printf("2. Display Pokedex\n");
        printf("3. Release Pokemon (by ID)\n");
        printf("4. Pokemon Fight!\n");
        printf("5. Evolve Pokemon\n");
        printf("6. Back to Main\n");

        subChoice = readIntSafe("Your choice: ");

        switch (subChoice)
        {
        case 1:
            addPokemon(cur);
            break;
        case 2:
            displayMenu(cur);
            break;
        case 3:
            freePokemon(cur);
            break;
        case 4:
            pokemonFight(cur);
            break;
        case 5:
            evolvePokemon(cur);
            break;
        case 6:
            printf("Back to Main Menu.\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (subChoice != 6);
}

void addPokemon(OwnerNode* owner) 
{
    if (!owner) 
    {
        printf("Invalid owner.\n");
        return;
    }

    //enter the ID
    printf("Enter ID to add: ");
    int id = readIntSafe("");
    if (id < 1 || id >POKE_NUMS)
    {
        printf("Invalid choice.\n");
        return;
    }


    //check if the pokemon already exist
    PokemonNode* existing = searchPokemonBFS(owner->pokedexRoot, id);
    if (existing) 
    {
        printf("Pokemon with ID %d is already in the Pokedex. No changes made.\n", id);
        return;
    }

    //add the new pokenode to the binary tree
    PokemonNode* newNode = createPokemonNode(&pokedex[id - 1]); 
    if (!newNode) 
    {
        printf("Failed to create Pokemon node.\n");
        return;
    }

    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, newNode);
    printf("Pokemon %s (ID %d) added.\n", newNode->data->name, newNode->data->id);
}

OwnerNode* findOwnerByName(const char* name) 
{
    // List is empty.
    if (!ownerHead) 
    {
        return NULL; 
    }

    OwnerNode* current = ownerHead;
    do {
        if (strcmp(current->ownerName, name) == 0) 
        {
            // Found the owner.
            return current; 
        }
        current = current->next;
    } while (current != ownerHead); // Loop back to the start if necessary.

    // Owner not found.
    return NULL; 
}

void freePokemon(OwnerNode* owner) 
{
    if (!owner || !owner->pokedexRoot) 
    {
        printf("No Pokemon to release.\n");
        return;
    }

    // Prompt for the ID of the Pokémon to release.
    int id = readIntSafe("Enter Pokemon ID to release: ");

    // Search for the Pokémon in the BST.
    PokemonNode* target = searchPokemonBFS(owner->pokedexRoot, id);
    if (!target) 
    {
        printf("No Pokemon with ID %d found.\n", id);
        return; // Pokémon not found.
    }
    //save the name before removing 
    char* pokename = target->data->name;
    // Remove the Pokémon from the BST.
    owner->pokedexRoot = removeNodeBST(owner->pokedexRoot, id);

    // Confirm removal.
    printf("Removing Pokemon %s (ID %d).\n",pokename, id);
}

//recursive function to free single node in the binary tree
void freePokemonNode(PokemonNode* node) 
{
    //stop condition
    if (!node) 
    {
        return; 
    }

    // Free the data
    if (node->data) 
    { 
        free(node->data);       
    }

    free(node);
}
//recursive function to free all the tree 
void freePokemonTree(PokemonNode* root) 
{
    //stop condition
    if (!root) 
    {
        return; 
    }

    // Recursively free the left subtree.
    freePokemonTree(root->left);

    // Recursively free the right subtree.
    freePokemonTree(root->right);

    // Free the current node.
    freePokemonNode(root);
}

void freeOwnerNode(OwnerNode* owner) 
{
    //there is nothing to free
    if (!owner) 
    {
        return; 
    }

    // Free the Pokedex tree (BST)
    if (owner->pokedexRoot) 
    {
        freePokemonTree(owner->pokedexRoot);
    }

    free(owner->ownerName);
    free(owner);
}

void freeAllOwners() 
{
    // List is empty.
    if (!ownerHead) 
    {
        return; 
    }

    OwnerNode* current = ownerHead;
    OwnerNode* nextNode;

    // Traverse the circular list and free each node.
    do 
    {
        // Save the next node.
        nextNode = current->next; 
        // Free the current owner node.
        freeOwnerNode(current);   
        // Move to the next node.
        current = nextNode;       
    } while (current != ownerHead); 

    // Clear the head pointer after all nodes are freed.
    ownerHead = NULL; 
}

void removeOwnerFromCircularList(OwnerNode* temp) 
{
    if (!ownerHead || !temp) 
    {
        printf("No owners to remove or invalid target.\n");
        return; 
    }

    //Single node in the list
    if (ownerHead == temp && ownerHead->next == ownerHead) 
    {
        freeOwnerNode(ownerHead); 
        ownerHead = NULL;         
        return;
    }

    //Multiple nodes in the list.
    OwnerNode* prev = temp->prev;
    OwnerNode* next = temp->next;

    // Bypass the target node.
    prev->next = next; 
    // Link the previous node to the next.
    next->prev = prev; 

    // Update ownerHead if needed.
    if (ownerHead == temp) 
    {
        ownerHead = next; 
    }

    freeOwnerNode(temp); 
}

void displayBFS(PokemonNode* root) 
{
    // Nothing to display.
    if (!root) 
    {
        printf("Pokedex is empty.\n");
        return; 
    }

    // Initialize the queue for BFS traversal.
    PokemonNode** queue = malloc(100 * sizeof(PokemonNode*));
    if (!queue) 
    {
        printf("Memory allocation failed for BFS queue.\n");
        return;
    }

    int front = 0, rear = 0; 
    queue[rear++] = root;    

    while (front < rear) 
    {
        PokemonNode* current = queue[front++];

        // Print the current node's data.
        printPokemonNode(current);

        // Enqueue the left child if it exists.
        if (current->left) 
        {
            queue[rear++] = current->left;
        }

        // Enqueue the right child if it exists.
        if (current->right) 
        {
            queue[rear++] = current->right;
        }
    }

    free(queue); 
}

PokemonNode* removeNodeBST(PokemonNode* root, int id) 
{
    //stop condition
    if (!root) 
    {
        return NULL; 
    }

    // Navigate left or right to find the node.
    if (id < root->data->id) 
    {
        root->left = removeNodeBST(root->left, id);
    }
    else
    {
        if (id > root->data->id)
        {
            root->right = removeNodeBST(root->right, id);
        }
        else
        {
            // The node is found
            //No children
            if (!root->left && !root->right)
            {
                freePokemonNode(root);
                return NULL;
            }

            //One child
            if (!root->left)
            {
                PokemonNode* temp = root->right;
                freePokemonNode(root);
                return temp;
            }
            if (!root->right)
            {
                PokemonNode* temp = root->left;
                freePokemonNode(root);
                return temp;
            }

            //Two children
            // Find the smallest node in the right subtree 
            PokemonNode* successor = root->right;
            while (successor->left)
            {
                successor = successor->left;
            }

            // Copy the data to the current node
            *(root->data) = *(successor->data);

            // Recursively delete the successor
            root->right = removeNodeBST(root->right, successor->data->id);
        }
    }
    //return the updated root
    return root; 
}

void pokemonFight(OwnerNode* owner) 
{
    //pokedex is empty
    if (!owner || !owner->pokedexRoot) 
    {
        printf("Pokedex is empty.\n");
        return; 
    }

    // Prompt for the IDs of the Pokémon to fight.
    printf("Enter ID of the first Pokemon: ");
    int id1 = readIntSafe("");
    printf("Enter ID of the second Pokemon: ");
    int id2 = readIntSafe("");

    // Search for both Pokémon in the BST.
    PokemonNode* pokemon1 = searchPokemonBFS(owner->pokedexRoot, id1);
    PokemonNode* pokemon2 = searchPokemonBFS(owner->pokedexRoot, id2);

    if (!pokemon1 || !pokemon2) {
        printf("One or both Pokemon IDs not found.\n");
        return; // At least one Pokémon is missing.
    }

    // Calculate the fight outcome.
    float score1 = pokemon1->data->attack * 1.5 + pokemon1->data->hp * 1.2;
    float score2 = pokemon2->data->attack * 1.5 + pokemon2->data->hp * 1.2;

    printf("Pokemon 1: %s (Score = %.2f)\n", pokemon1->data->name, score1);
    printf("Pokemon 2: %s (Score = %.2f)\n", pokemon2->data->name, score2);



    if (score1 > score2) 
    {
        printf("%s wins!\n",pokemon1->data->name);
        return;
    }
    if (score2 > score1) 
    {
        printf("%s wins!\n", pokemon2->data->name);
        return;
    }
    if (score1 == score2)
    {
        // Tie case.
        printf("It’s a tie!\n");
        return;
    }

}


PokemonNode* searchPokemonBFS(PokemonNode* root, int id) 
{
    if (!root) 
        return NULL;

    PokemonNode** queue = malloc(100 * sizeof(PokemonNode*)); 
    int front = 0, rear = 0;

    queue[rear++] = root;

    while (front < rear) 
    {
        PokemonNode* current = queue[front++];
        if (current->data->id == id) 
        {
            free(queue);
            return current;
        }

        if (current->left)
            queue[rear++] = current->left;

        if (current->right)
            queue[rear++] = current->right;
    }

    free(queue);
    return NULL;
}
//recursive function to insert pokenode to the binary tree
PokemonNode* insertPokemonNode(PokemonNode* root, PokemonNode* newNode) 
{
    if (!root) return newNode;

    if (newNode->data->id < root->data->id) {
        root->left = insertPokemonNode(root->left, newNode);
    }
    else {
        root->right = insertPokemonNode(root->right, newNode);
    }
    return root;
}

PokemonNode* createPokemonNode(const PokemonData* data) 
{
    if (!data) 
        return NULL;

    PokemonNode* node = malloc(sizeof(PokemonNode));
    if (!node) 
    {
        return NULL;
    }

    node->data = malloc(sizeof(PokemonData));
    if (!node->data) 
    {
        free(node);
        return NULL;
    }

    *(node->data) = *data;
    node->left = node->right = NULL;
    return node;
}

void printOwnersCircular() 
{
    // List is empty.
    if (!ownerHead) 
    {
        printf("No owners.\n");
        return; 
    }

    // Get the direction and number of prints from the user.
    char direction;
    printf("Enter direction (F or B): ");
    scanf(" %c", &direction);
    //clean the buffer
    getchar();

    // Validation
    if (direction != 'F' && direction != 'B' && direction != 'f' && direction != 'b') {
        printf("Invalid direction. Use 'F' for Forward or 'B' for Backward.\n");
        return;
    }

    int times = readIntSafe("How many prints? ");
    if (times <= 0) 
    {
        printf("Invalid number of prints.\n");
        return;
    }

    // Start printing based on the specified direction.
    OwnerNode* current = ownerHead;
    for (int i = 0; i < times; ++i) 
    {
        printf("[%d] %s\n", i + 1, current->ownerName);

        // Move to the next node based on direction.
        if (direction == 'F' || direction == 'f') 
        {
            current = current->next;
        }
        if (direction == 'B' || direction == 'b') 
        {
            current = current->prev;
        }
    }
}

void mergePokedexMenu() 
{
    
    // Less than two owners.
    if (!ownerHead || ownerHead->next == ownerHead) 
    {
        printf("Not enough owners to merge.\n");
        return; 
    }

    printf("\n=== Merge Pokedexes ===\n");
    // Prompt for the first owner.
    printf("Enter name of first owner: ");
    char* firstOwnerName = getDynamicInput();
    OwnerNode* firstOwner = findOwnerByName(firstOwnerName);
    //searching the user name in the binary tree
    if (!firstOwner) 
    {
        printf("Owner '%s' not found.\n", firstOwnerName);
        free(firstOwnerName);
        return;
    }

    // Prompt for the second owner.
    printf("Enter name of second owner: ");
    char* secondOwnerName = getDynamicInput();
    OwnerNode* secondOwner = findOwnerByName(secondOwnerName);
    //searching the user name in the binary tree
    if (!secondOwner) 
    {
        printf("Owner '%s' not found.\n", secondOwnerName);
        free(firstOwnerName);
        free(secondOwnerName);
        return;
    }

    // Prevent merging an owner with themselves.
    if (firstOwner == secondOwner) 
    {
        printf("Cannot merge an owner with themselves.\n");
        free(firstOwnerName);
        free(secondOwnerName);
        return;
    }

    printf("Merging %s and %s...\n", firstOwnerName, secondOwnerName);

    //Merging
    if (secondOwner->pokedexRoot) 
    {
        PokemonNode** queue = malloc(100 * sizeof(PokemonNode*)); 
        int front = 0, rear = 0;

        queue[rear++] = secondOwner->pokedexRoot;

        while (front < rear) 
        {
            PokemonNode* current = queue[front++];

            // Insert current Pokémon into the first owner's Pokedex.
            PokemonNode* newNode = createPokemonNode(current->data);
            firstOwner->pokedexRoot = insertPokemonNode(firstOwner->pokedexRoot, newNode);

            // left and right children
            if (current->left) 
            {
                queue[rear++] = current->left;
            }
            if (current->right) 
            {
                queue[rear++] = current->right;
            }
        }

        free(queue);
    }

    // Remove the second owner after the merge.
    removeOwnerFromCircularList(secondOwner);

    printf("Merge completed.\n Owner '%s' has been removed after merging.\n", secondOwnerName);

    free(firstOwnerName);
    free(secondOwnerName);
}

void evolvePokemon(OwnerNode* owner) 
{
    if (!owner || !owner->pokedexRoot) 
    {
        printf("Cannot evolve. Pokedex empty.\n");
        return; // No owner or empty Pokedex.
    }

    int oldID = readIntSafe("Enter ID of Pokemon to evolve: ");

    // Search for the Pokémon in the BST.
    PokemonNode* current = searchPokemonBFS(owner->pokedexRoot, oldID);
    if (!current) 
    {
        printf("No Pokemon with ID %d found.\n", oldID);
        return;
    }

    // Check if the Pokémon can evolve.
    if (current->data->CAN_EVOLVE == CANNOT_EVOLVE) 
    {
        printf("%s (ID %d) cannot evolve.\n", current->data->name, current->data->id);
        return;
    }

    int newID = oldID + 1;
  
    // Check if the evolved Pokémon already exists in the BST.
    if (searchPokemonBFS(owner->pokedexRoot, newID)) 
    {
        printf("Pokemon with ID %d already exists. Removing duplicate.\n", newID);
        owner->pokedexRoot = removeNodeBST(owner->pokedexRoot, oldID);
        return;
    }

    // Perform the evolution.
    PokemonNode* evolvedNode = createPokemonNode(&pokedex[newID - 1]);
    if (!evolvedNode) 
    {
        printf("Failed to create evolved Pokemon node.\n");
        return;
    }

    printf("Pokemon evolved from %s (ID %d) to %s (ID %d).\n", current->data->name, oldID,
        evolvedNode->data->name, newID);

    // Remove the old Pokémon.
    owner->pokedexRoot = removeNodeBST(owner->pokedexRoot, oldID); 
    // Add the evolved one.
    owner->pokedexRoot = insertPokemonNode(owner->pokedexRoot, evolvedNode); 

    
}

void openPokedexMenu()
{
    char* name;
    printf("Your name: ");
    name = getDynamicInput();
    if (findOwnerByName(name))
    {
        printf("Owner '%s' already exists. Not creating a new Pokedex.", name);
        free(name);
        return;
    }
    printf("Choose Starter:\n1. Bulbasaur\n2. Charmander\n3. Squirtle\nYour choice:");
    int choice = readIntSafe("");

    PokemonNode* starter = NULL;
    switch (choice)
    {
    case(1):
        //Bulbasaur
        starter = createPokemonNode(&pokedex[0]);
        break;

    case(2):
        //Charmander
        starter = createPokemonNode(&pokedex[3]);
        break;

    case(3):
        // Squirtle
        starter = createPokemonNode(&pokedex[6]);
        break;


    default:
        printf("Invalid choice.\n");

        free(name);
        return;
    }

    addOwner(name, starter); 
    printf(" New Pokedex created for %s with starter %s.", name, starter->data->name); 
    free(name);
}

void postOrderTraversal(PokemonNode* root) 
{
    //stop condition
    if (!root) 
    {
        return; 
    }

    // Traverse the left subtree.
    postOrderTraversal(root->left);

    // Traverse the right subtree.
    postOrderTraversal(root->right);

    // Print the current node.
    printPokemonNode(root);
}

void inOrderTraversal(PokemonNode* root) 
{
    //stop condition
    if (!root) 
    {
        return; 
    }

    // Traverse the left subtree.
    inOrderTraversal(root->left);

    // Print the current node.
    printPokemonNode(root);

    // Traverse the right subtree.
    inOrderTraversal(root->right);
}

void preOrderTraversal(PokemonNode* root) 
{
    //stop condition
    if (!root) 
    {
        return; 
    }

    // Print the current node.
    printPokemonNode(root);

    // Traverse the left subtree.
    preOrderTraversal(root->left);

    // Traverse the right subtree.
    preOrderTraversal(root->right);
}

void sortOwners() 
{
    //only one owner or no owners at all
    if (!ownerHead || ownerHead->next == ownerHead) 
    {
        printf("0 or 1 owners only => no need to sort.\n");
        return; 
    }

    int swapped;
    OwnerNode* current;
    OwnerNode* next;

    do 
    {
        swapped = 0;
        current = ownerHead;

        // Traverse the list.
        do 
        {
            next = current->next;

            // Compare names and swap if out of order.
            if (strcmp(current->ownerName, next->ownerName) > 0) 
            {
                swapOwnerData(current, next);
                swapped = 1;
            }

            current = next;
            
        } while (current->next != ownerHead);
        
    } while (swapped);

    printf("Owners sorted by name.\n");
}

// Helper function to swap the data of two OwnerNodes.
void swapOwnerData(OwnerNode* a, OwnerNode* b)
{
    //temp variables
    char* tempName = a->ownerName;
    PokemonNode* tempPokedexRoot = a->pokedexRoot;

    
    a->ownerName = b->ownerName;
    a->pokedexRoot = b->pokedexRoot;

    b->ownerName = tempName;
    b->pokedexRoot = tempPokedexRoot;
}

void deletePokedex()
{
    int index = 1;
    

    // Check if the list is empty
    if (!ownerHead)
    {
        printf("No existing Pokedexes to delete.\n");
        return;
    }
    printf("\n=== Delete a Pokedex ===\n");
    // Print all Pokedex owners
    OwnerNode* cur = ownerHead;
    do
    {
        printf("%d. %s\n", index, cur->ownerName); // Print each owner with an index
        cur = cur->next;
        index++;
    } while (cur != ownerHead);

    // Get the user's choice for which Pokedex to delete
    printf("Choose a Pokedex to delete by number: ");
    int num = readIntSafe("");

    // Validate the user's choice
    if (num < 1 || num >= index)
    {
        printf("Invalid choice. Returning to menu.\n");
        return;
    }

    // Navigate to the selected node
    cur = ownerHead;
    for (int i = 1; i < num; i++)
    {
        cur = cur->next;
    }

    printf("Deleting %s's entire Pokedex...\n", cur->ownerName);
    //if its the only node in the list
    if (cur->next == NULL && cur->prev == NULL)
    {
        ownerHead = NULL;
    }
    else
    {
        // Remove the node from the circular linked list
        removeOwnerFromCircularList(cur);
    }
    
    

    printf("Pokedex deleted.\n");
}


void addOwner(char* ownerName, PokemonNode* starter)
{
    OwnerNode* new = malloc(sizeof(OwnerNode));
    if (!new)
        exit(1);
    
    //creating new owner
    new->ownerName = myStrdup(ownerName);
    new->pokedexRoot = starter;
    new->next = NULL;
    new->prev = NULL;

    //if its the first owner
    if (!ownerHead)
    {
        new->next = new;
        new->prev = new;
        ownerHead = new;
    }
    else
    {
        OwnerNode* last = ownerHead->prev;
        new->next = ownerHead;
        new->prev = last;

        last->next = new;
        ownerHead->prev = new;
    }
}

// --------------------------------------------------------------
// Main Menu
// --------------------------------------------------------------
void mainMenu()
{
    int choice;
    do
    {
        printf("\n=== Main Menu ===\n");
        printf("1. New Pokedex\n");
        printf("2. Existing Pokedex\n");
        printf("3. Delete a Pokedex\n");
        printf("4. Merge Pokedexes\n");
        printf("5. Sort Owners by Name\n");
        printf("6. Print Owners in a direction X times\n");
        printf("7. Exit\n");
        choice = readIntSafe("Your choice: ");

        switch (choice)
        {
        case 1:
            openPokedexMenu();
            break;
        case 2:
            enterExistingPokedexMenu();
            break;
        case 3:
            deletePokedex();
            break;
        case 4:
            mergePokedexMenu();
            break;
        case 5:
            sortOwners();
            break;
        case 6:
            printOwnersCircular();
            break;
        case 7:
            printf("Goodbye!\n");
            break;
        default:
            printf("Invalid.\n");
        }
    } while (choice != 7);
}

int main()
{
    
    mainMenu();
    freeAllOwners();
    return 0;
}
