#include "MMM.h"

void init_memory(){
    memset(ram, 0, RAM_SIZE); //inizializziamo lo spazio della RAM a 0
    head = (block*)ram; //effettua il casting di ram, spazio di memoria a cui è stato assegnato tutto lo spazio disponibile
    head->size = RAM_SIZE - sizeof(block); //impostiamo size alla dimensione rimasta menio la dimensione occupata dal blocco
    head->free = 1;  //impostiamo il blocco come libero 
    head->next = NULL;  //imposta il puntatore a next su NULL
}

void split_block(block* b, size_t size){
    if(b->size> size + sizeof(block)){ //controlliamo di avere abbastanza spazio per lo split
        block *newBlock = (block*)malloc(sizeof(block)); //allochiamo lo spazio per un nuovo blocco
        unsigned char* newData = b->data + size; //creazione dell'area di memoria
        newBlock->size = b->size-size - sizeof(block);  
        newBlock->next = NULL;
        newBlock->data = newData; //puntatore all'inizio dell'area dati in RAM
        newBlock -> free = 1;

        b->size=size; //impostiamo la dimensione del blocco sullo spazio che abbiamo specificato 
        b->free=0;
        b->next = newBlock;
    }
    else{
        b->free=0;
    }
}

void* memory_alloc(size_t size){
    if(head == NULL){ //se head non è stata ancora inizializzata, la inizializziamo
        init_memory();
    }
    block *current = head; //inizializziamo un blocco current 
    while(current != NULL){ //finché non arriviamo al primo NUL della catena
        if(current->free==1 && current ->size>=size){ //se il blocco è libero (free=1) e la dimensione è sufficiente
            split_block(current, size); //splittiamo il blocco
            return (void*)((byte*)current+sizeof(block)); //restituiamo il puntatore allo spazio allocato, quindi dall'indirizzo di inizio blocco + la dimensione del blocco
        }
        current = current ->next; //scorriamo se non siamo ancora arrivati all'ultimo blocco   
    }
    return NULL; //se siamo arrivati alla fine e non abbiamo allocato nessun blocco allora restituiamo un puntatre nullo 
}

void memory_free(void *ptr){
    if (ptr == NULL) return; // se il puntatore è nullo non abbiamo referenzesu cosa liberare
    block* to_free = (block*)((byte*)ptr - sizeof(block));
    /*sottraendo dal ptr la dimensione del blocco ottengo il blocco header*/
    if (to_free->free == 0) { // se è occupato allora c'è effettivamente qualcosa da liberare
        to_free->free = 1; // impostiamo il blocco come libero 
        memset((byte*)to_free + sizeof(block), 0, to_free->size); //reimposta a zero lo spazio in questione
        /*to_free è un puntatore a blocco che punta all'header, aggiungendo la dimensione del blocco otteniamo l'inizio dello spazio allocato */
        //PROBLEMA: si crea frammentazione con l'implementazione attuale, questo ci impedisce anche di deallocare l'header perchè altrimenti perderemmo qualsiasi riferimento allo spazio allocato precedentemente
    } //non è di per sé necessario liberare la memoria perchè alla prossima allocazione i dati comunque verrebbero sovrascritti
} 
