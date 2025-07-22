#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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

    //Data structure that will store the cache
    bool cache_val[n_sets * assoc];
    int cache_tag[n_sets * assoc];

    int n_bits_offset = log2(b_size);
    int n_bits_index = log2(n_sets);
    int n_bits_tag = 32 - n_bits_offset - n_bits_index;

    // Counters for benchmark
    int totalAccessCount = 0;
    int totalMissCount = 0;
    int totalHitCount = 0;
    int compulsoryMissCount = 0;
    int capacityMissCount = 0;
    int conflictMissCount = 0;

    uint8_t buffer[4];  // stores the 4 bytes (word size)
    while (fread(buffer, sizeof(uint8_t), 4, filePtr) == 4) {
        uint32_t currentAdd = big_to_little_endian(buffer);
        uint32_t tag = currentAdd >> (n_bits_offset + n_bits_index);
        uint32_t index = (currentAdd >> n_bits_offset) & ((1 << n_bits_index) - 1);

        printf("Add: 0x%08X | tag: 0x%08X | index: %u\n",
       currentAdd,
       tag,
       index);
        //TODO: cache logic

        totalAccessCount++;

        //  Direct mapping (devemos mover isso pra uma função talvez)
        //not valid (miss)
        if(cache_val[index] == false){
            totalMissCount++;
            compulsoryMissCount++;

            //Miss treatment (writes tag in index) (sets validity bit to 1)
            cache_val[index] = true;
            cache_tag[index] = tag;


        }else{
            if(cache_tag[index] == tag){
                totalHitCount++; 
            }else{
                //conflito ou capacidade?
                totalMissCount++;

                //Miss treatment (writes tag in index) (sets validity bit to 1)
                cache_val[index] = true;
                cache_tag[index] = tag;
            }
        }
    }

    return 0;
}