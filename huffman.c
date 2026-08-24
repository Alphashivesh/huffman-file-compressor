#include "huffman.h"

// Pre-allocate the maximum possible nodes to prevent fragmentation
MinHeapNode node_pool[511]; 
int pool_count = 0;

// Helper function to grab the next available node from our pool
MinHeapNode* get_new_node(unsigned char data, unsigned int freq) {
    MinHeapNode* node = &node_pool[pool_count++];
    node->data = data;
    node->freq = freq;
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Optimized function using a 4KB buffer
void calculate_frequencies(const char *filename, unsigned int freq[256]) {
    FILE *file = fopen(filename, "rb"); 
    
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        exit(1);
    }

    // Create a 4 Kilobyte buffer in memory
    unsigned char buffer[4096]; 
    size_t bytes_read;
    
    // Read up to 4096 bytes at once into the buffer
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        // Iterate through the buffer in RAM (which is incredibly fast)
        for (size_t i = 0; i < bytes_read; i++) {
            freq[buffer[i]]++; 
        }
    }

    fclose(file);
}

// --- MISSING MIN-HEAP AND TREE LOGIC START ---

// Function to create a new, empty Min-Heap
MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(minHeap->capacity * sizeof(MinHeapNode*));
    return minHeap;
}

// Swap helper function
void swapNodes(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

// Function to maintain the min-heap structure (sorts smallest to top)
void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

// Extract the node with the lowest frequency
MinHeapNode* extractMin(MinHeap* minHeap) {
    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

// Insert a new combined node back into the heap
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    
    // Bubble up the node if its frequency is smaller than its parent
    while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = minHeapNode;
}

// Function to build the Huffman tree and return the root node
MinHeapNode* buildHuffmanTree(unsigned int freq[256]) {
    // 1. Initialize the Heap
    MinHeap* minHeap = createMinHeap(256);
    
    for (int i = 0; i < 256; ++i) {
        if (freq[i] > 0) {
            minHeap->array[minHeap->size++] = get_new_node((unsigned char)i, freq[i]);
        }
    }
    
    // 2. Sort the initial heap
    for (int i = (minHeap->size - 2) / 2; i >= 0; --i) {
        minHeapify(minHeap, i);
    }

    MinHeapNode *left, *right, *top;

    // 3. Build the tree
    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);

        // '$' is a dummy character for internal parent nodes
        top = get_new_node('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    MinHeapNode* root = extractMin(minHeap);
    free(minHeap->array); 
    free(minHeap);
    
    return root;
}

// Helper function to check if a node is a leaf (has no children)
int isLeaf(MinHeapNode* root) {
    return !(root->left) && !(root->right);
}

// Recursive function to generate and store the codes
void generateCodes(MinHeapNode* root, int arr[], int top, char codes[256][256]) {
    // Travel left: add 0 to the array
    if (root->left) {
        arr[top] = 0;
        generateCodes(root->left, arr, top + 1, codes);
    }

    // Travel right: add 1 to the array
    if (root->right) {
        arr[top] = 1;
        generateCodes(root->right, arr, top + 1, codes);
    }

    // If we hit a leaf node, save the accumulated 0s and 1s as a string
    if (isLeaf(root)) {
        for (int i = 0; i < top; ++i) {
            codes[root->data][i] = arr[i] ? '1' : '0';
        }
        codes[root->data][top] = '\0'; // Null-terminate the string
    }
}

// Function to actually compress the file and write it to disk
void compress_file(const char *in_filename, const char *out_filename, char codes[256][256], unsigned int freq[256]) {
    FILE *in_file = fopen(in_filename, "rb");
    FILE *out_file = fopen(out_filename, "wb"); 

    if (!in_file || !out_file) {
        printf("Error: Could not open files for compression.\n");
        return;
    }

    // --- PROGRESS BAR SETUP ---
    // Jump to the end of the file to find its total size, then rewind to the beginning
    fseek(in_file, 0, SEEK_END);
    long total_bytes = ftell(in_file);
    fseek(in_file, 0, SEEK_SET); 
    long bytes_processed = 0;

    // 1. Write the Header (OPTIMIZED)
    unsigned int unique_chars = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) unique_chars++;
    }
    
    fwrite(&unique_chars, sizeof(unsigned int), 1, out_file);
    
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            unsigned char c = (unsigned char)i;
            fwrite(&c, sizeof(unsigned char), 1, out_file);
            fwrite(&freq[i], sizeof(unsigned int), 1, out_file);
        }
    }

    unsigned char bit_buffer = 0;
    int bit_count = 0;
    unsigned char buffer[4096];
    size_t bytes_read;

    // 2. Read and Compress Chunk by Chunk
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), in_file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            unsigned char current_char = buffer[i];
            char *code = codes[current_char];

            for (int j = 0; code[j] != '\0'; j++) {
                bit_buffer = bit_buffer << 1; 
                if (code[j] == '1') {
                    bit_buffer = bit_buffer | 1; 
                }
                bit_count++;

                if (bit_count == 8) {
                    fwrite(&bit_buffer, 1, 1, out_file);
                    bit_buffer = 0;
                    bit_count = 0;
                }
            }
        }
        
        // --- DRAW THE PROGRESS BAR ---
        bytes_processed += bytes_read;
        int percent = (int)(((float)bytes_processed / total_bytes) * 100);
        int bar_width = 30; // How wide the bar is in the terminal
        int pos = (percent * bar_width) / 100;
        
        // \r forces the terminal cursor back to the start of the line!
        printf("\r["); 
        for (int k = 0; k < bar_width; ++k) {
            if (k < pos) printf("=");
            else if (k == pos) printf(">");
            else printf(" ");
        }
        printf("] %d%%", percent);
        fflush(stdout); // Force the terminal to update the screen immediately
    }

    // 3. Flush remaining bits
    if (bit_count > 0) {
        bit_buffer = bit_buffer << (8 - bit_count);
        fwrite(&bit_buffer, 1, 1, out_file);
    }

    printf("\n"); // Move to a new line when the progress bar hits 100%
    fclose(in_file);
    fclose(out_file);
}

// Function to decompress a .bin file back to its original state
void decompress_file(const char *in_filename, const char *out_filename) {
    FILE *in_file = fopen(in_filename, "rb");
    FILE *out_file = fopen(out_filename, "wb"); 

    if (!in_file || !out_file) {
        printf("Error: Could not open files for decompression.\n");
        return;
    }

    // 1. Read the frequency header (OPTIMIZED)
    unsigned int freq[256] = {0};
    unsigned int unique_chars = 0;
    
    // Find out how many unique characters we need to read
    fread(&unique_chars, sizeof(unsigned int), 1, in_file);
    
    // Rebuild the frequency array using only the saved characters
    for (unsigned int i = 0; i < unique_chars; i++) {
        unsigned char c;
        unsigned int f;
        fread(&c, sizeof(unsigned char), 1, in_file);
        fread(&f, sizeof(unsigned int), 1, in_file);
        freq[c] = f;
    }

    // Calculate exactly how many characters we need to decode
    unsigned long total_chars = 0;
    for (int i = 0; i < 256; i++) {
        total_chars += freq[i];
    }

    // 2. Rebuild the exact same Huffman tree
    pool_count = 0; // Reset our memory pool so we don't run out of nodes!
    MinHeapNode* root = buildHuffmanTree(freq);
    MinHeapNode* current = root;

    // 3. Read the compressed bits and traverse the tree
    unsigned char buffer[4096];
    size_t bytes_read;
    unsigned long chars_decoded = 0;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), in_file)) > 0) {
        for (size_t i = 0; i < bytes_read; i++) {
            unsigned char current_byte = buffer[i];
            
            // Read bit by bit, from left to right (7 down to 0)
            for (int j = 7; j >= 0; j--) {
                int bit = (current_byte >> j) & 1;
                
                // Traverse the tree: 0 = left, 1 = right
                if (bit == 0) {
                    current = current->left;
                } else {
                    current = current->right;
                }

                // 4. If we hit a leaf, we found the character!
                if (isLeaf(current)) {
                    fputc(current->data, out_file);
                    chars_decoded++;
                    current = root; // Reset back to the top of the tree
                    
                    // Stop if we have decoded all original characters 
                    // (This ignores the extra padding 0s at the very end of the file)
                    if (chars_decoded == total_chars) {
                        fclose(in_file);
                        fclose(out_file);
                        return;
                    }
                }
            }
        }
    }

    fclose(in_file);
    fclose(out_file);
}