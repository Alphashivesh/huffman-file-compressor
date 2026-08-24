#include "huffman.h"
#include <string.h>

// Helper function to get the size of a file in bytes
long get_file_size(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) return 0;
    
    fseek(file, 0, SEEK_END); // Jump to the end of the file
    long size = ftell(file);  // Ask where we are (which gives the size)
    fclose(file);
    
    return size;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Error: Invalid number of arguments.\n\n");
        printf("Usage Instructions:\n");
        printf("  To compress:   %s -c <input_file> <compressed_file>\n", argv[0]);
        printf("  To decompress: %s -d <compressed_file> <output_file>\n", argv[0]);
        return 1;
    }

    char *mode = argv[1];
    char *input_file = argv[2];
    char *output_file = argv[3];

    if (strcmp(mode, "-c") == 0) {
        printf("Compressing '%s' to '%s'...\n", input_file, output_file);
        
        unsigned int freq[256] = {0}; 
        calculate_frequencies(input_file, freq);
        
        MinHeapNode* root = buildHuffmanTree(freq);
        int arr[256], top = 0;
        char codes[256][256] = {{0}}; 
        
        if (root != NULL) {
            generateCodes(root, arr, top, codes);
            compress_file(input_file, output_file, codes, freq);
            
            // --- NEW STATISTICS LOGIC ---
            long original_size = get_file_size(input_file);
            long compressed_size = get_file_size(output_file);
            float savings = 100.0 - (((float)compressed_size / original_size) * 100.0);
            
            printf("\n--- Compression Summary ---\n");
            printf("Original Size:   %ld bytes\n", original_size);
            printf("Compressed Size: %ld bytes\n", compressed_size);
            printf("Space Saved:     %.2f%%\n", savings);
            printf("---------------------------\n");
            
        } else {
            printf("Error: Could not build tree. The input file might be empty.\n");
        }
    } 
    else if (strcmp(mode, "-d") == 0) {
        printf("Decompressing '%s' to '%s'...\n", input_file, output_file);
        decompress_file(input_file, output_file);
        printf("Success! File successfully decompressed.\n");
    } 
    else {
        printf("Error: Unknown flag '%s'. Please use '-c' or '-d'.\n", mode);
        return 1;
    }

    return 0;
}