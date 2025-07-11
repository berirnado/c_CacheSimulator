#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Function that converts big-endian to little-endian
uint32_t big_to_little_endian(uint8_t bytes[4]) {
    return (bytes[0] << 24) |
           (bytes[1] << 16) |
           (bytes[2] << 8)  |
           (bytes[3]);
}

// args: <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada
// Exemplo 1  ["256", "4", "1", "R", "1", "bin_100.bin"] 

int main(int argc, char **argv){

    if(argc != 7){
        printf("Invalid arguments\nExiting...");
        exit(1);
    }

    // Number of Sets
    const short int n_sets = atoi(argv[1]);

    // Block Size
    const short int b_size = atoi(argv[2]);

    // Cache Associativity
    const short int assoc = atoi(argv[3]);

    // Replacement Policy
    // R: random
    // F: FIFO
    // L: LRU
    const char repPolicy = argv[4][0];

    // Output Flag
    // 0: with text
    // 1: no text 
    const short int outputFlag = atoi(argv[5]);

    // Input File
    // Benchmark Addresses
    char *inputFile = argv[6];

    FILE *filePtr = fopen(inputFile, "rb");
    if(filePtr == NULL) {
        printf("Not able to open the file.");
        exit(1);
    }

    char currentAddress[32];

    int currentAdd;

    uint8_t buffer[4];  // stores the 4 bytes (sizeof(int))
    while (fread(buffer, sizeof(uint8_t), 4, filePtr) == 4) {
        uint32_t numero = big_to_little_endian(buffer);
        //TODO: cache logic
    }

    return 0;
}