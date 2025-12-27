#ifndef MMM_H
#define MMM_H
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> 
#include <string.h>

#define RAM_SIZE 1024*1024 //simuliamo una ram da 10 mb

typedef unsigned char byte; 

typedef struct block{
    size_t size; //dimensione del blocco 
    struct block *next; //puntatore al prossimo blocco
    int free; // 1 se libero, 0 se occupato
    byte* data; // punta all'inizio della memoria dati
} block;

byte ram[RAM_SIZE];
block *head = NULL;

typedef struct allocationInfo{
    void *ptr; //puntatore void al blocco allocato
    size_t size; //dimensione allocata
    const char *filename; //opzionale: può contenere il nom del file dalla quale è stato allocato
    int line; //opzionale: riga di codice
    struct allocationInfo *next; //puntatore alla prossima allocationTable
} allocationInfo;

typedef struct memory_block{
    size_t size; //dimensione totale della memoria
    unsigned char *start; //blocco iniziale della RAM
} memory_block;

// Prototipi delle funzioni
void init_memory();
void split_block(block* b, size_t size);
void* memory_alloc(size_t size);
void memory_free(void *ptr);
void* memory_realloc(void *ptr, size_t new_size);

#endif