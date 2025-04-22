#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char c;
    struct Node *left, *right;
} Node;

static Node* newNode(char ch) {
    Node *n = malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->c = ch;
    n->left = n->right = NULL;
    return n;
}

static void insertCode(Node **root, char ch, const char *code) {
    if (!*root) *root = newNode('\0');
    Node *cur = *root;
    for (int i = 0; code[i]; i++) {
        if (code[i] == '0') {
            if (!cur->left) cur->left = newNode('\0');
            cur = cur->left;
        } else {
            if (!cur->right) cur->right = newNode('\0');
            cur = cur->right;
        }
    }
    cur->c = ch;
}

static void decode(Node *root, const char *bits) {
    Node *cur = root;
    for (int i = 0; bits[i]; i++) {
        cur = (bits[i] == '0') ? cur->left : cur->right;
        if (!cur->left && !cur->right) {
            putchar(cur->c);
            cur = root;
        }
    }
    putchar('\n');
}

static void freeTree(Node *r) {
    if (!r) return;
    freeTree(r->left);
    freeTree(r->right);
    free(r);
}

int main(void) {
    const char *map[] = {
        "a 00",
        "c 01000",
        "e 101",
        "i 1110",
        "l 1101",
        "m 100",
        "o 0101",
        "P 1100",
        "p 01001",
        "r 1111",
        "s 0110",
        "_ 0111",
        NULL
    };

    const char *bits =
        "1100001101100101111011110001100111010000010001001101000101";

    Node *root = NULL;
    for (int i = 0; map[i]; i++) {
        char ch; char code[16];
        sscanf(map[i], "%c %15s", &ch, code);
        if (ch == '_') ch = ' ';
        insertCode(&root, ch, code);
    }

    printf("===== ATIVIDADE 1 =====\n");
    printf("Bits codificados: %s\n", bits);
    printf("Texto decodificado: ");
    decode(root, bits);

    freeTree(root);
    return 0;
}
