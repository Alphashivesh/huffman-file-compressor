#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>

// 1. Structure Definitions
typedef struct MinHeapNode {
    unsigned char data;
    unsigned int freq;
    struct MinHeapNode *left;
    struct MinHeapNode *right;
} MinHeapNode;

typedef struct MinHeap {
    unsigned int size;
    unsigned int capacity;
    MinHeapNode **array;
} MinHeap;

// 2. Function Prototypes
void calculate_frequencies(const char *filename, unsigned int freq[256]);
MinHeap* createMinHeap(unsigned capacity);
MinHeapNode* buildHuffmanTree(unsigned int freq[256]); 
int isLeaf(MinHeapNode* root);
void generateCodes(MinHeapNode* root, int arr[], int top, char codes[256][256]);
void compress_file(const char *in_filename, const char *out_filename, char codes[256][256], unsigned int freq[256]);
void decompress_file(const char *in_filename, const char *out_filename);

#endif