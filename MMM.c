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
        block* newBlock = (block*)((byte*)b + sizeof(block) + size);
        //allochiamo lo spazio per un nuovo blocco
        /*Il nuovo blocco avrà come area assegnata inizio di b + dimensione del blocco + dimensione dell'area allocata*/
        byte* newData = b->data + size + sizeof(block); //creazione del puntatore che referenzierà l'inizio dell'area dati del blocco
        newBlock->size = b->size- size - sizeof(block);   
        //la dimensione del nuovo blocco che farà da tail sarà la dimensione che aveva b prima - la dimensione da allocare - la dimensione che useremo per il nuovo blocco header
        newBlock->next = NULL; //il puntatore al prossimo nodo viene inizializzato a NULL
        newBlock->data = newData; //visto che la dimensione del blocco è fissa possiamo calcolare l'inizio dell'area dati del blocco successivo come il vecchio inizio + la dimensione
        newBlock -> free = 1; //settiamo il blocco come libero

        b->size=size; //la dimensione dell'area dati del blocco b diverrà quella richiesta dall'utente
        b->free=0; //il blocco verrà impostato come occupato
        b->next = newBlock; //il campo next referenzierà il nuovo blocco di coda
    }
    else{
        b->free=0; //se non c'è abbastanza spazio impostiamo il blocco come occupato per indicare l'impossibilità di usarlo
    } 
    /*se non è la prima volta che allochiamo il blocco finiremo qua e setteremo il blocco 
    semplicemente su occupato per riusarlo*/
}

void* memory_alloc(size_t size){
    if(head == NULL){ //se head non è stata ancora inizializzata, la inizializziamo
        init_memory();
    }
    block *current = head; //inizializziamo un blocco current 
    while(current != NULL){ //finché non arriviamo al primo NUL della catena
        if(current->free==1 && current ->size>=size){ //se il blocco è libero (free=1) e la dimensione è sufficiente (first-fit)
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

void* memory_realloc(void *ptr, size_t new_size){
    if(ptr==NULL){
        return memory_alloc(new_size); //se il puntatore è nullo allochiamo nuovo spazio
    }
    block *current_block = (block*)((byte*)ptr - sizeof(block));
    //otteniamo il blocco header sottraendo la dimensione del blocco al puntatore
    //Il puntatore punta all'inizio dell'area dati, sottraendo la dimensione del blocco otteniamo l'inizio del blocco header
    if(current_block->size >= new_size){
        split_block(current_block, new_size); //se la dimensione attuale è sufficiente splittiamo il blocco per adattarlo alla nuova dimensione
        return ptr; //restituiamo lo stesso puntatore perchè l'area dai rimane invariata
    }
    else{
        void* new_ptr = memory_alloc(new_size); //allochiamo un nuovo spazio di dimensione new_size
        if(new_ptr != NULL){
            memcpy(new_ptr, ptr, current_block->size); 
            //copiamo il contenuto dell'area puntata da ptr nel nuovo spazio allocato
            memory_free(ptr); //liberiamo la vecchia area di memoria
            return new_ptr; //restituiamo il nuovo puntatore
        }
    }
}

