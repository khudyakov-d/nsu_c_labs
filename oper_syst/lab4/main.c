#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

struct Node{
    char * value;
    struct Node * next;
};
typedef struct Node Node;

void screeningValue(char * value){
    for (int i = 0; value[i] != '\0' ; ++i) {
        if (value[i] == 24 || value[i] == 25 || value[i] == 26 || value[i] == 27){
            value[i]=' ';
        }
    }
}

void push(Node **head, char * value) {
    Node * node = (Node *)malloc(sizeof(Node));
    node->value = value;
    node->next = (*head);
    (*head) = node;
}

char * pop(Node ** head) {
    Node * prev = NULL;
    char * value = NULL;

    if ((*head) == NULL) {
        return value;
    }

    prev = (*head);
    value = (*head)->value;

    (*head) = (*head)->next;
    free(prev);

    return value;
}

void printList(const Node *head) {
    while (head) {
        screeningValue(head->value);
        printf("%s\n", head->value);
        head = head->next;
    }
}


void deleteList(Node **head) {
    Node* prev = NULL;
    if (*head == NULL)
        return;

    while ((*head)->next) {
        prev = (*head);
        (*head) = (*head)->next;
        free(prev->value);
        free(prev);
    }

    free((*head)->value);
    free(*head);
}


int main() {
    Node * head = NULL;
    char * s = (char *)malloc(sizeof(char)*256);
    if (s == NULL)
        return 1;

    while(gets(s)) {
        if ((strlen(s) == 1) && ((*s) == '.'))
            break;

        s = realloc(s,strlen(s)+1);
        push(&head, s);

        s = (char *)malloc(sizeof(char)*256);
        if (s == NULL)
            return 1;
    }

    printList(head);
    deleteList(&head);

    return 0;
}
