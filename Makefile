# Define the compiler and optimization flags
CC = gcc
CFLAGS = -Wall -O3

# Define the name of your final executable
TARGET = main

# The default rule when you just type "make"
all: $(TARGET)

# How to build the target
$(TARGET): main.c huffman.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c huffman.c

# A command to easily delete old compiled files and test data
clean:
	del $(TARGET).exe *.bin restored.txt