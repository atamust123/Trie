#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define CHAR_TO_INDEX(C) ((int)C - (int)'a')

struct Trie
{
    struct Trie *subTrie[26];
    bool isLeaf;
    char character;
    char password[10];
};

struct Trie *Create()
{ //it creates a Trienode with 26 word
    struct Trie *temp = (struct Trie *)malloc(sizeof(struct Trie));
    int i;
    temp->isLeaf = false;
    for (i = 0; i < 26; i++)
        temp->subTrie[i] = NULL;
    return temp;
}

struct Trie *add(struct Trie *root, const char *name, char *password,FILE *output)
{
    int i;
    int leng = strlen(name);
    int index;

    struct Trie *temp = root;
    for (i = 0; i < leng; i++)
    {
        index = CHAR_TO_INDEX(name[i]); //it goes i. element of subtrie
        if (temp->subTrie[index] == NULL)
        {
            temp->subTrie[index] = Create();           //subtrie created
            temp->subTrie[index]->character = name[i]; //word is passing
        }
        temp = temp->subTrie[index];
    }
    if (temp->isLeaf == true)
    {
        fprintf(output, "\"%s\" reserved username\n", name);
        return root;
    }
    temp->isLeaf = true;
    strcpy(temp->password, password); //password passing
    fprintf(output, "\"%s\" was added\n", name);
    return root;
}

void search(struct Trie *root, char *name,FILE *output)
{
    int i, len, index;
    len = strlen(name);
    struct Trie *temp = root;

    index = CHAR_TO_INDEX(name[0]);
    for (i = 0; i < 26; i++)
    {
        if (!temp->subTrie[index])
        { //No record beacuse first char is NULL
            fprintf(output, "\"%s\" no record\n", name);
            return;
        }
    }

    for (i = 0; i < len; i++)
    {
        index = CHAR_TO_INDEX(name[i]);
        if (!temp->subTrie[index])
        { //if any other than first char is NULL print incorrect
            fprintf(output, "\"%s\" incorrect username\n", name);
            return;
        }
        else if (i == (len - 1) && temp->subTrie[index]->isLeaf == false) //if we are on the last word
        {
            fprintf(output, "\"%s\" not enough username\n", name);
            return;
        }
        else if (i == (len - 1) && temp->subTrie[index]->isLeaf == true)
        {
            fprintf(output, "\"%s\" password \"%s\"\n", name, temp->subTrie[index]->password);
            return;
        }
        temp = temp->subTrie[index];
    }
    return;
}

void query(struct Trie *root, char *name, char *password,FILE *output)
{
    int i, len, index;
    len = strlen(name);
    index = CHAR_TO_INDEX(name[0]);
    for (i = 0; i < 26; i++)
    {
        if (!root->subTrie[index])
        { //No record beacuse first char is NULL
            fprintf(output, "\"%s\" no record\n", name);
            return;
        }
    }
    for (i = 0; i < len; i++)
    {
        index = CHAR_TO_INDEX(name[i]);
        if (!root->subTrie[index])
        {
            fprintf(output, "\"%s\" incorrect username\n", name); //subtrie is empty
            return;
        }
        else if (i == (len - 1) && root->subTrie[index]->isLeaf == false) // if we are on the last element and there is no password
        {
            fprintf(output, "\"%s\" not enough username\n", name);
            return;
        }
        else if (i == (len - 1) && root->subTrie[index]->isLeaf == true && strcmp(root->subTrie[index]->password, password) != 0)
        { //word are correct but password is incorrect
            fprintf(output, "\"%s\" incorrect password\n", name);
            return;
        }
        else if (i == (len - 1) && root->subTrie[index]->isLeaf == true && strcmp(root->subTrie[index]->password, password) == 0)
        { //print if everything is correct
            fprintf(output, "\"%s\" successful login\n", name);
            return;
        }
        root = root->subTrie[index];
    }
    return;
}

bool subTrieExist(struct Trie *root)
{ //is there any subtrie
    int i;
    for (i = 0; i < 26; i++)
        if (root->subTrie[i] != NULL)
            return true;
    return false;
}

void delete (struct Trie **root, char *name, int whichword,FILE *output)
{
    if (*root == NULL) //root is empty
        return;
    int index = CHAR_TO_INDEX(name[whichword]);

    if (!(*root)->subTrie[index])
    {
        fprintf(output, "\"%s\" incorrect username\n", name);
        return;
    }

    if (whichword == strlen(name) - 1 && (*root)->subTrie[index]->isLeaf == false)
    { //if we are on the last word and and root has continue then printf
        fprintf(output, "\"%s\" not enough username\n", name);
        return;
    }

    if (whichword == strlen(name) - 1 && (*root)->subTrie[index]->password) // are we on the the last word otherwise continue
    {
        if ((*root)->subTrie[index]->isLeaf == true) //if everything is correct delete
        {
            (*root)->subTrie[index]->isLeaf = false;
            fprintf(output, "\"%s\" deletion is successful\n", name);
        }
        if (subTrieExist((*root)->subTrie[index]) == false) //  free and delete if there is no subtrie
        {
            free((*root)->subTrie[index]);
            ((*root)->subTrie[index]) = NULL;
        }

        return;
    }
    delete (&((*root)->subTrie[index]), name, whichword + 1,output); //recursion go until last word

    if (subTrieExist((*root)->subTrie[index]) == false && //last control is there any indentation
        (*root)->subTrie[index]->isLeaf == false)
    {
        free((*root)->subTrie[index]);
        ((*root)->subTrie[index]) = NULL;
    }

    return;
}

void list(struct Trie *root, char *temp, int whichword, bool isOnly,FILE *output)
{ //it takes 4 parameter first one Trie root second one is just an array third one word counter
    //boolean one is controls is there any subtrie more than one
    int i, subTrieCounter = 0;
    //whichword==0 we are on the root 
    for (i = 0; i < 26; i++) //is there any subtrie more than one
        if (root->subTrie[i] != NULL)
            subTrieCounter++;

    if ((subTrieCounter > 1 && whichword != 0)) //if subtrie number is more than one
    {
        temp[whichword] = '\0';
        fprintf(output, "-%s\n", temp);
    }

    if (subTrieCounter > 1 && whichword != 0) //subtries are more than one
        isOnly = false;

    else if (isOnly == true && root->isLeaf == true) //it prints that if the string has no branch
    {
        temp[whichword] = '\0';
        fprintf(output, "-%s\n", temp);
    }

    if (root->isLeaf == true) //prints if we are on the last word
    {
        temp[whichword] = '\0';
        fprintf(output, "\t-%s\n", temp);
    }

    for (i = 0; i < 26; i++) //go recursion until the end
    {
        if (root->subTrie[i] != NULL)
        {
            temp[whichword] = root->subTrie[i]->character;
            list(root->subTrie[i], temp, whichword + 1, isOnly,output);
        }
    }
    return;
}

void freeTrie(struct Trie **root)
{ //free trie
    int i;
    if (*root == NULL)
        return;

    for (i = 0; i < 26; i++)
        if ((*root)->subTrie[i] != NULL)
            freeTrie(&(*root)->subTrie[i]);
    free(*root);
    *root = NULL;
}

int main(int argc, char const *argv[])
{
    FILE *inputF = fopen(argv[1], "r");
    FILE *output = fopen("output.txt", "w+");
    char *token, command, name[20], password[20];
    char key[256];
    int index;
    struct Trie *root = Create();
    char temp[256];
    while (!feof(inputF))
    {
        fgets(key, 256, inputF);
        if (key[1] == 'a')
        {
            token = strtok(key, " ");
            command = token[1];
            token = strtok(NULL, " ");
            strcpy(name, token);
            token = strtok(NULL, " \n");
            strcpy(password, token);
            root = add(root, name, password,output);
        }
        else if (key[1] == 's')
        {
            token = strtok(key, " ");
            command = token[1];
            token = strtok(NULL, "\n");
            strcpy(name, token);
            search(root, name,output);
        }
        else if (key[1] == 'q')
        {
            token = strtok(key, " ");
            command = token[1];
            token = strtok(NULL, " ");
            strcpy(name, token);
            token = strtok(NULL, " \n");
            strcpy(password, token);
            query(root, name, password,output);
        }
        else if (key[1] == 'd')
        {
            token = strtok(key, " ");
            command = token[1];
            token = strtok(NULL, "\n");
            strcpy(name, token);
            index = CHAR_TO_INDEX(name[0]);
            if (root->subTrie[index] == NULL)
                fprintf(output, "\"%s\" no record\n", name);
            else
                delete (&root, name, 0,output);
        }
        else if (key[1] == 'l')
        {
            list(root, temp, 0, true,output);
        }
    }
    freeTrie(&root);
    fclose(inputF);
    fclose(output);
    return 0;
}