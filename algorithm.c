typedef struct {
    int validBit; //free =0; notfree = 1;
    int dirtyBit; //writeback; =0 first write; =1 overwrite;
    int frameNumber;
    int pageNumber;
    char addr[8];
    int lruID; //quanto maior o ID, mais recente o endereço foi acessado
} PageTableEntry;

void writeOnTable(int pageNumber, PageTableEntry * pageTable, char * addr){

    pageTable[pageNumber].validBit = 1;
    memcpy(pageTable[pageNumber].addr, addr, strlen(addr)+1);

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
    int minReference = pageTable[0].lruID;

    for(int i=0; i < numPages; i++){

        if(pageTable[i].lruID <= minReference){
            lru = i;
            minReference = pageTable[i].lruID;
        }
    }

    return lru; 
}