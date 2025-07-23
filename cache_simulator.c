#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

// Function that converts big-endian to little-endian
uint32_t big_to_little_endian(uint8_t bytes[4]) {
    return (bytes[0] << 24) |
           (bytes[1] << 16) |
           (bytes[2] << 8)  |
           (bytes[3]);
}

//Prints stats in the given outputFormat (0 = Free format, 1 = Standard)
void printStats(const CacheStats *s, short int outputFormat){
    switch(outputFormat){
        case 0:
            printf("Acessos Totais   : %d\n", s->totalAccessCount);
            printf("Acertos Totais   : %d\n", s->totalHitCount);
            printf("Faltas Totais    : %d\n", s->totalMissCount);
            printf("Faltas Compulsórias : %d\n", s->compulsoryMissCount);
            printf("Faltas por Conflito : %d\n", s->conflictMissCount);
            printf("Faltas por Capacidade : %d\n", s->capacityMissCount);
            if (s->totalAccessCount) {
                double hit_rate = (double)s->totalHitCount / s->totalAccessCount * 100.0;
                double miss_rate = 100.0 - hit_rate;
                printf("Taxa de Acerto %%   : %.2f\n", hit_rate);
                printf("Taxa de Falha %%    : %.2f\n", miss_rate);
            }
            break;
        case 1:
            break;
        default:
            printf("Invalid output flag %d", outputFormat);
            exit(1);
    }
}

int randomReplacement(int assoc){
    return rand() % (assoc);
}

int fifoReplacement(uint32_t index, uint32_t tag,
                    int assoc,
                    bool *cache_val, uint32_t *cache_tag,
                    uint64_t *fifo_age, uint64_t *lru_age,
                    uint64_t access_ctr) {
    int target = 0;                 // Vai armazenar a via (way) que será substituída
    uint64_t oldest = UINT64_MAX;  // Inicializa com valor máximo para achar o mais antigo
    int base = index * assoc;      // Base do conjunto (set) no array linear

    // Percorre todas as vias do conjunto para encontrar a mais antiga (FIFO)
    for (int way = 0; way < assoc; way++) {
        int pos = base + way;      // Calcula posição linear da via no array
        if (fifo_age[pos] < oldest) { // Se essa via foi usada há mais tempo
            oldest = fifo_age[pos];   // Atualiza o valor mais antigo
            target = way;             // Marca essa via como candidata para substituição
        }
    }
    int pos = base + target;       // Posição da via a ser substituída
    cache_tag[pos] = tag;          // Atualiza a tag com a nova
    cache_val[pos] = true;         // Marca essa via como válida (ocupada)
    fifo_age[pos] = access_ctr;    // Atualiza a "idade" FIFO para o acesso atual
    lru_age[pos] = access_ctr;     // Também atualiza o contador LRU para consistência
    return target;                 // Retorna a via substituída
}

int lruReplacement(uint32_t index, uint32_t tag,
                   int assoc,
                   bool *cache_val, uint32_t *cache_tag,
                   uint64_t *fifo_age, uint64_t *lru_age,
                   uint64_t access_ctr) {
    int target = 0;                // Vai armazenar a via a ser substituída
    uint64_t oldest = UINT64_MAX; // Valor máximo para encontrar o menos recentemente usado
    int base = index * assoc;     // Base do conjunto no array linear

    // Percorre todas as vias do conjunto para achar a menos recentemente usada (LRU)
    for (int way = 0; way < assoc; way++) {
        int pos = base + way;     // Calcula posição linear da via
        if (lru_age[pos] < oldest) { // Se essa via foi a que menos recentemente foi usada
            oldest = lru_age[pos];   // Atualiza o valor mais antigo (menos recente)
            target = way;            // Marca a via para substituição
        }
    }
    int pos = base + target;       // Posição da via a ser substituída
    cache_tag[pos] = tag;          // Atualiza a tag da via com a nova
    cache_val[pos] = true;         // Marca a via como válida (ocupada)
    fifo_age[pos] = access_ctr;    // Atualiza também o contador FIFO para consistência
    lru_age[pos] = access_ctr;     // Atualiza o contador LRU com o acesso atual
    return target;                 // Retorna a via substituída
}

typedef struct CacheStats {
    int totalAccessCount;
    int totalMissCount;
    int totalHitCount;
    int compulsoryMissCount;
    int conflictMissCount;
    int capacityMissCount;
} CacheStats;


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

    srand(0);

    //Data structure that will store the cache

    int nLines = n_sets * assoc;

    //Array that stores validity bits in cache
    bool cache_val[nLines];
    //Array that stores tag in cache
    uint32_t cache_tag[nLines];
    //Array that stores age for FIFO
    uint64_t fifo_age[nLines];
    //Array that stores age for LRU
    uint64_t lru_age[nLines];

    for(int i = 0; i < nLines; i++){
        cache_val[i] = false;
        cache_tag[i] = 0xFFFFFFFFu;
        fifo_age[i] = 0;
        lru_age[i] = 0;
    }

    int n_bits_offset = log2(b_size);
    int n_bits_index = log2(n_sets);
    int n_bits_tag = 32 - n_bits_offset - n_bits_index;

    // Counters for benchmark
    CacheStats stats = {0};

    int access_ctr = 0;

    uint8_t buffer[4];  // stores the 4 bytes (word size)

    //Main loop 
    while (fread(buffer, sizeof(uint8_t), 4, filePtr) == 4) {
        uint32_t currentAdd = big_to_little_endian(buffer);
        uint32_t tag = currentAdd >> (n_bits_offset + n_bits_index);
        uint32_t index = (currentAdd >> n_bits_offset) & ((1 << n_bits_index) - 1);

        stats.totalAccessCount++;

        bool hit = false;
        int firstAvailableWay = -1;

        //Actual index considering linear array for associativty
        int base = index * assoc;

        for(int way = 0; way < assoc; way++){ // percorre cada via do conjunto]
            int pos = base + way;

            if(cache_val[pos]){ // verifica se a via atual está suja

                if(cache_tag[pos] == tag){
                    hit = true;
                    stats.totalHitCount++;
                    lru_age[pos] = stats.totalAccessCount;
                    break;
                }

            }else if(firstAvailableWay == -1){ // se nao estiver suja e ainda nao tiver setado nenhuma via disponivel
                firstAvailableWay = way; // seta essa via como disponivel para tratar a falta
            }
        }

        if(!hit){
            stats.totalMissCount++;

            //Miss treatment:
            if(assoc == 1){
                //mapeamento direto apenas substitui a unica via que tem
                int pos = base;
                stats.conflictMissCount++;
                cache_tag[index] = tag;
                cache_val[index] = true;
                fifo_age[pos] = stats.totalAccessCount;
                lru_age[pos] = stats.totalAccessCount;
            }
            else if(firstAvailableWay != -1){ // se existir availableWay nao precisa da politica de substituição
                int pos = base + firstAvailableWay;
                stats.compulsoryMissCount++;
                cache_tag[pos] = tag;
                cache_val[pos] = true;
                fifo_age[pos] = stats.totalAccessCount;
                lru_age[pos] = stats.totalAccessCount;
            }else{
                //nao existe via disponivel, vamos precisar substituir alguma
                stats.conflictMissCount++;
                switch(repPolicy){
                case 'R':
                    //random
                    int way = randomReplacement(assoc);
                    int pos = base + way;
                    cache_tag[pos] = tag;
                    cache_val[pos] = true;
                    fifo_age[pos] = stats.totalAccessCount;
                    lru_age[pos] = stats.totalAccessCount;
                    break;
                case 'L':
                    //LRU
                    lruReplacement(index, tag, assoc,
                                       cache_val, cache_tag, fifo_age, lru_age,
                                       stats.totalAccessCount);
                    break;
                case 'F':
                    //FIFO
                    fifoReplacement(index, tag, assoc,
                                        cache_val, cache_tag, fifo_age, lru_age,
                                        stats.totalAccessCount);
                    break;
                default:
                    fprintf(stderr, "Unknown replacement policy %c\n", repPolicy);
                    exit(1);
                }
            }
        }
    }

    printStats(&stats, outputFlag);

    return 0;
}