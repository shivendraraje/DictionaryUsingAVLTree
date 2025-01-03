#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_WORD_LENGTH 50
#define MAX_DEFINITION_LENGTH 200

struct AVLNode
{
    char word[50];
    char definition[200];
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
};
void updateDictionaryFile(FILE *fp, struct AVLNode *node);
int height(struct AVLNode *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return node->height;
}
int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}
struct AVLNode *newNode(char *word, char *definition)
{
    struct AVLNode *node = (struct AVLNode *)malloc(sizeof(struct AVLNode));
    strcpy(node->word, word);
    strcpy(node->definition, definition);
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}
struct AVLNode *rightRotate(struct AVLNode *y)
{
    struct AVLNode *x = y->left;
    struct AVLNode *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}
struct AVLNode *leftRotate(struct AVLNode *x)
{
    struct AVLNode *y = x->right;
    struct AVLNode *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}
int getBalance(struct AVLNode *node)
{
    if (node == NULL)
    {
        return 0;
    }
    return height(node->left) - height(node->right);
}
struct AVLNode *insert(struct AVLNode *node, char *word, char *definition)
{
    if (node == NULL)
    {
        struct AVLNode *new_node = newNode(word, definition);
        FILE *fp = fopen("dictionary.txt", "a");
        if (fp == NULL)
        {
            printf("Error: Unable to open file.\n");
            return new_node;
        }
        fprintf(fp, "%s:%s\n", word, definition);
        fclose(fp);
        return new_node;
    }

    if (strcmp(word, node->word) < 0)
    {
        node->left = insert(node->left, word, definition);
    }
    else if (strcmp(word, node->word) > 0)
    {
        node->right = insert(node->right, word, definition);
    }
    else
    {
        return node;
    }
    node->height = 1 + max(height(node->left), height(node->right));//update the height of tree after insertion 
    int balance = getBalance(node);
    if (balance > 1 && strcmp(word, node->left->word) < 0)
    {
        return rightRotate(node);
    }
    if (balance < -1 && strcmp(word, node->right->word) > 0)
    {
        return leftRotate(node);
    }
    if (balance > 1 && strcmp(word, node->left->word) > 0)
    {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && strcmp(word, node->right->word) < 0)
    {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }
    return node;
}
struct AVLNode *search(struct AVLNode *node, char *word)
{
    if (node == NULL || strcmp(node->word, word) == 0)
    {
        return node;
    }
    if (strcmp(word, node->word) < 0)
    {
        return search(node->left, word);
    }
    else
    {
        return search(node->right, word);
    }
}
void printDefinition(struct AVLNode *node, char *word)
{
    struct AVLNode *result = search(node, word);
    if (result == NULL)
    {
        printf("Error: Word not found in dictionary.\n");
    }
    else
    {
        printf("%s\n", result->definition);
    }
}
struct AVLNode *deleteNode(struct AVLNode *root, char *word)
{
    if (root == NULL)
    {
        printf("Word not Found");
        return root;
    }
    if (strcmp(word, root->word) < 0)
    {
        root->left = deleteNode(root->left, word);
    }
    else if (strcmp(word, root->word) > 0)
    {
        root->right = deleteNode(root->right, word);
    }
    else
    {
        if (root->left == NULL)
        {
            struct AVLNode *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            struct AVLNode *temp = root->left;
            free(root);
            return temp;
        }
        struct AVLNode *temp = root->right;
        while (temp->left != NULL)
        {
            temp = temp->left;
        }

        strcpy(root->word, temp->word);
        strcpy(root->definition, temp->definition);

        root->right = deleteNode(root->right, temp->word);
    }

    if (root == NULL)
    {
        return root;
    }
    root->height = 1 + max(height(root->left), height(root->right));
    int balance = getBalance(root);
    if (balance > 1 && getBalance(root->left) >= 0)
    {
        return rightRotate(root);
    }
    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }
    if (balance < -1 && getBalance(root->right) <= 0)
    {
        return leftRotate(root);
    }
    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }
    FILE *fp = fopen("dictionary.txt", "w");
    if (fp == NULL)
    {
        printf("Error: Unable to open file.\n");
        return root;
    }
    updateDictionaryFile(fp, root);
    fclose(fp);

    return root;
}

void updateDictionaryFile(FILE *fp, struct AVLNode *root)
{
    if (root != NULL)
    {
        updateDictionaryFile(fp, root->left);
        fprintf(fp, "%s: %s\n", root->word, root->definition);
        updateDictionaryFile(fp, root->right);
    }
}

void displayWords(struct AVLNode *node)
{
    if (node != NULL)
    {
        displayWords(node->left);
        printf("%s\n", node->word);
        displayWords(node->right);
    }
}
void displayDictionary(struct AVLNode *node)
{
    FILE *fp = fopen("dictionary.txt", "r");
    if (fp == NULL)
    {
        printf("Error: Unable to open file.\n");
        return;
    }
    char dictio[256];
    while (fgets(dictio, sizeof(dictio), fp))
    {
        printf("%s", dictio);
    }
    fclose(fp);
}

int main()
{
    struct AVLNode *root = NULL;
     FILE *fp = fopen("dictionary.txt", "r");
            if (fp == NULL)
            {
                printf("Error opening dictionary file!\n");
                exit(1);
            }
            char line[256];

            while (fgets(line, sizeof(line), fp))
            {
                char *word = strtok(line, ":");
                char *meaning = strtok(NULL, "\n");
                root = insert(root, word, meaning);
            }
            fclose(fp);
    while (1)
    {
        printf("------------------------------------------------------------------------------------------------------\n");
        printf("\nMenu:\n");
        printf("1. Add a word to the dictionary\n");
        printf("2. Search for the definition of a word\n");
        printf("3. Delete a word from the dictionary\n");
        printf("4. Display the words in the dictionary\n");
        printf("5. Display the words and their definitions \n");
        printf("6. Quit\n");
        printf("Enter your choice: ");
        printf("\n------------------------------------------------\n");
        int choice;
        scanf("%d", &choice);
        printf("------------------------------------------------\n");

        switch (choice)
        {
        case 1:
        {
            char word[MAX_WORD_LENGTH];
            printf("Enter the word to add: ");
            scanf("%s", word);
            if (search(root, word) != NULL)
            {
                printf("Error: Word already in dictionary.\n");
                break;
            }

            char definition[MAX_DEFINITION_LENGTH];
            printf("Enter the definition of the word: ");
            getchar();
            fgets(definition, MAX_DEFINITION_LENGTH, stdin);

            root = insert(root, word, definition);
            printf("Word added to dictionary.\n");
            break;
        }
        case 2:
        {
            char word[MAX_WORD_LENGTH];
            printf("Enter the word to search for: ");
            scanf("%s", word);
            printDefinition(root, word);
            break;
        }
        case 3:
        {
            char word[MAX_WORD_LENGTH];
            printf("Enter the word to delete: ");
            scanf("%s", word);
            root = deleteNode(root, word);
           
            break;
        }
        case 4:
        {
            printf("Words in dictionary:\n");
            displayWords(root);
            break;
        }
        case 5:
        {
            printf("Words and definitions in dictionary:\n");
            displayDictionary(root);
            break;
        }
        case 6:
        {
            printf("Exiting...\n");
            exit(0);
        }
        default:
        {
            printf("Invalid choice.\n");
            printf("------------------------------------------------\n");
            break;
        }
        }
    }
}