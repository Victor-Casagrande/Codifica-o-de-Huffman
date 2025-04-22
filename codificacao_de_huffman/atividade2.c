#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#define MAX_WCHARS 65536
#define MAX_BIN    16000
#define MAX_LINE   2048

typedef struct Node {
    wchar_t c;
    int freq;
    struct Node *left, *right;
} Node;

static Node* newNode(wchar_t ch, int f) {
    Node *n = malloc(sizeof(Node));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->c = ch; n->freq = f; n->left = n->right = NULL;
    return n;
}

static void freeTree(Node *r) {
    if (!r) return;
    freeTree(r->left);
    freeTree(r->right);
    free(r);
}

static void swap(Node **a, Node **b) { Node *t=*a; *a=*b; *b=t; }
static void heapify(Node **h, int n, int i) {
    int s=i, l=2*i+1, r=2*i+2;
    if (l<n && h[l]->freq<h[s]->freq) s=l;
    if (r<n && h[r]->freq<h[s]->freq) s=r;
    if (s!=i) { swap(&h[i],&h[s]); heapify(h,n,s); }
}
static void buildHeap(Node **h, int n) {
    for (int i=n/2-1; i>=0; --i) heapify(h,n,i);
}
static Node* popMin(Node **h, int *n) {
    Node *m = h[0];
    h[0] = h[--(*n)];
    heapify(h,*n,0);
    return m;
}
static void heapInsert(Node **h, int *n, Node *x) {
    int i = (*n)++;
    h[i] = x;
    while (i && h[(i-1)/2]->freq > h[i]->freq) {
        swap(&h[i], &h[(i-1)/2]);
        i = (i-1)/2;
    }
}

static Node* buildFromText(const wchar_t *txt) {
    int freq[MAX_WCHARS] = {0};
    for (size_t i=0; txt[i]; ++i) freq[(unsigned)txt[i]]++;
    int uniq=0; for (int i=0; i<MAX_WCHARS; ++i) if (freq[i]) ++uniq;

    Node **heap = malloc(uniq * sizeof(Node*));
    int n = 0;
    for (int i=0; i<MAX_WCHARS; ++i)
        if (freq[i]) heap[n++] = newNode((wchar_t)i, freq[i]);

    buildHeap(heap, n);
    while (n > 1) {
        Node *l = popMin(heap, &n);
        Node *r = popMin(heap, &n);
        Node *p = newNode(L'\0', l->freq + r->freq);
        p->left = l; p->right = r;
        heapInsert(heap, &n, p);
    }
    Node *root = n ? heap[0] : NULL;
    free(heap);
    return root;
}

static void genCodes(Node *r, char **dict, char *buf, int d) {
    if (!r) return;
    if (!r->left && !r->right) {
        buf[d] = '\0';
        dict[(unsigned)r->c] = strdup(buf);
        return;
    }
    buf[d] = '0'; genCodes(r->left,  dict, buf, d+1);
    buf[d] = '1'; genCodes(r->right, dict, buf, d+1);
    buf[d] = '\0';
}
static void freeDict(char **d) {
    for (int i=0; i<MAX_WCHARS; ++i) if (d[i]) free(d[i]);
}

static void encodeText(const wchar_t *txt, char **dict, char *bin) {
    bin[0] = '\0';
    for (size_t i=0; txt[i]; ++i) {
        unsigned idx = (unsigned)txt[i];
        if (!dict[idx]) {
            fwprintf(stderr, L"Caractere '%lc' sem código.\n", txt[i]);
            exit(EXIT_FAILURE);
        }
        strcat(bin, dict[idx]);
    }
}

int main(void) {
    setlocale(LC_ALL, "");

    char frase_u8[] =
        u8"Não é a falta de café, nem tequila. É meu código que não compila!";

    wchar_t frase_w[] =
        L"Não é a falta de café, nem tequila. É meu código que não compila!";

        Node *root = buildFromText(frase_w);

    char *dict[MAX_WCHARS] = {0};
    char buf[256] = {0};
    genCodes(root, dict, buf, 0);

    char bin[MAX_BIN];
    encodeText(frase_w, dict, bin);

    printf("===== ATIVIDADE 2 =====\n");
    printf("Frase original: %s\n\n", frase_u8);
    printf("Binário: %s\n", bin);

    freeDict(dict);
    freeTree(root);
    return 0;
}
