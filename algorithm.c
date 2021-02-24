typedef struct {
    int validBit; //free =0; notfree = 1;
    int dirtyBit; //writeback; =0 first write; =1 overwrite;
    int frameNumber;
    unsigned int pageNumber;
    char addr[8];
    int algID; 
} PageTableEntry;

void writeOnTable(int pageNumber, PageTableEntry * pageTable, char * addr, unsigned int virtualPage){

    pageTable[pageNumber].validBit = 1;
    memcpy(pageTable[pageNumber].addr, addr, strlen(addr)+1);
    pageTable[pageNumber].pageNumber = virtualPage;
    
    if(pageTable[pageNumber].dirtyBit == -1){
        pageTable[pageNumber].dirtyBit = 0;
    }
    else if(pageTable[pageNumber].dirtyBit == 0){
        pageTable[pageNumber].dirtyBit = 1;
    }
    
}

int findAddress(PageTableEntry * pageTable, char * addr, int numPages){

    for(int i=0; i < numPages; i++){
        if(pageTable[i].validBit == 1){//some address in this position
            int str = strncmp(pageTable[i].addr, addr, sizeof(char[8]));
            if(str == 0){ //found address
                return i;
            } 
        }
    }
    return -1; //address not in pageTable

}

int findFreeAddress(PageTableEntry * pageTable, int numPages){
    
    for(int i=0; i < numPages; i++){
        if(pageTable[i].validBit == 0){//no address in this position
            return i;
        }
    }
    return -1; //no free position found in pageTable

}

int LRU(PageTableEntry * pageTable, int numPages){
    int lru = -1;
    int minReference = pageTable[0].algID;
    //quanto maior o algID, mais recente o endereço foi acessado

    for(int i=0; i < numPages; i++){

        if(pageTable[i].algID <= minReference){
            lru = i;
            minReference = pageTable[i].algID;
        }
    }

    return lru; 
}

int FIFO(PageTableEntry * pageTable, int numPages){
    int fifo = -1;
    int firstReference = pageTable[0].algID;

    for(int i=0; i < numPages; i++){

        if(pageTable[i].algID <= firstReference){
            fifo = i;
            firstReference = pageTable[i].algID;
        }
    }

    return fifo; 
}


int secondChance(PageTableEntry * pageTable, int numPages, int secondChancePtr){
    if(secondChancePtr >= numPages){    
        secondChancePtr = 0;
    }
    
    while(pageTable[secondChancePtr].algID == 1) {
        pageTable[secondChancePtr].algID = 0;
        secondChancePtr++;
        if(secondChancePtr >= numPages){
            secondChancePtr = 0;
        }
    }
    return secondChancePtr;
}

int LeastReferenced (PageTableEntry * pageTable, int numPages, int leastReferencePtr){
    //tem que terminar de fazer a parte do main e nem sei se essa parte ta certa, so comecei a ideia mesmo
    if(leastReferencePtr >= numPages){    
        leastReferencePtr = 0;
    }
    
    while(pageTable[leastReferencePtr].algID != 0) {
        pageTable[leastReferencePtr].algID--;
        leastReferencePtr++;
        if(leastReferencePtr >= numPages){
            leastReferencePtr = 0;
        }
    }
    return leastReferencePtr;

}