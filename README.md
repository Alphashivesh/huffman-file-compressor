# Huffman File Compressor in C

A robust, professional-grade lossless file compression and decompression utility built entirely from scratch in C using Huffman Coding, Min-Heaps, and custom bit-packing algorithms.

## Features
- **Lossless Compression:** Bit-exact data recovery verified via binary file comparison.
- **Min-Heap & Tree Optimization:** Dynamically builds optimal prefix codes based on character frequencies.
- **Dynamic Header Serialization:** Strips unnecessary overhead by writing only active character frequencies to the compressed header.
- **Bit-Level Packing:** Packs individual bits into compact bytes to achieve real disk-space reduction.
- **Interactive CLI & UX:** Built-in terminal progress bar and clear usage instructions.
- **Build Automation:** Fully supported with a custom `Makefile` for high-speed compilation (`-O3`).

## Project Structure
- `main.c` - Command-line interface, argument routing, file size tracking, and progress bar animation.
- `huffman.c` - Core compression, decompression, min-heap, tree building, and bit-manipulation logic.
- `huffman.h` - Function prototypes and data structure definitions.
- `Makefile` - Automated build and cleanup instructions.

## Getting Started

### 1. Compile the Program
Make sure you have MinGW installed, then run:
```bash
mingw32-make
```

# To Compress a File:
```bash
main -c <input_file> <output_file.bin>
```

# To Decompress a File:
```bash
main -d <input_file.bin> <output_file.txt>
```

# To remove old binaries and test text files, run:
```bash
mingw32-make clean
```