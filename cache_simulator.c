#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    // Addresses to benchmark
    char *intputFile = argv[6];

    return 0;
}