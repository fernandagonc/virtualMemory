#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
// https://www.alltestanswers.com/designing-a-virtual-memory-manager-using-c-program/
// https://github.com/ranadeep47/virtual-memory-simulation
// https://github.com/8tiqa/vm-mgt/blob/master/vm.c
// https://github.com/joelrlneto/memoriavirtual/blob/master/SMV.c
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

int main(int argc, char *argv[]){
    char *alg, *file;
    alg = argv[1];
	file = argv[2];
	int pageSize = atoi(argv[3]);
	int memSize = atoi(argv[4]);

    if(argc != 5){
        printf("ERRO: Para executar use: ./tp2virtual [algoritmo] arquivo.log 4 128");
        exit(1);
    }

	if(pageSize < 2 || pageSize > 64){	
		printf("ERRO: O tamanho de cada página deve estar entre 2 e 64.");
		return 0;
	}
		
	if(memSize < 128 || memSize > 16384){
		printf("ERRO: O tamanho da memória deve estar entre 128 e 16384.");
		return 0;
	}	
	
	if(strcmp(alg, "lru") && strcmp(alg, "fifo") && strcmp(alg,"2a")){
		printf("ERRO: O algoritmo deve ser lru, fifo ou 2a.");
		return 0;	
	}

    int numPages = memSize/pageSize;
    int operations = 0; 
    int writes = 0;
    int reads = 0;
    int pageFaults = 0;
    int writeBacks = 0;

    unsigned offset = 0; 
    int tmpPageSize = pageSize;
    while (tmpPageSize > 1) {
        tmpPageSize = tmpPageSize >> 1;
        offset++;
    }

    unsigned virtualPageNumber; 
    int pageFoundAt, freePageAt;
    char addr[8];
    char rw;
    int i, j;
    unsigned int addrInt;
    PageTableEntry pageTable[numPages];

    for(i = 0; i < numPages; i++){
        pageTable[i].dirtyBit = -1;
        pageTable[i].validBit = 0;    
        memcpy(pageTable[i].addr, "", strlen("")+1);
    }
    FILE *fileOpen = fopen(file, "r");  
    if(fileOpen != NULL){       
        clock_t begin = clock();
        printf("TABELA DE SUBSTIUIÇÕES\n");

        while(fscanf(fileOpen,"%s %c", addr,&rw) != EOF){
            operations++;
            addr[8] = '\0';
            // addrInt = (int)strtol(addr, NULL, 16);
            // printf("addr: %d\n", addrInt);
            //virtualPageNumber = addrInt >> offset;
            pageFoundAt = findAddress(pageTable, addr, numPages);

            if(rw == 'W' || rw == 'w'){
                writes++;                
            }
            else if(rw == 'R' || rw == 'r'){				
                reads++;
            }

            if(pageFoundAt == -1){//page not found
                pageFaults++;
                freePageAt = findFreeAddress(pageTable, numPages);
                if(freePageAt == -1){//no free page
                    writeBacks++;

                    if(!strcmp(alg, "lru")){
                        freePageAt = LRU(pageTable, numPages);
                        writeOnTable(freePageAt, pageTable, addr);
                        pageTable[freePageAt].lruID = operations;

                    }
                    //algoritmos de substituição
                }
                else{
                    writeOnTable(freePageAt, pageTable, addr);
                    pageTable[freePageAt].lruID = operations;
                }
            }
            else{
                // printf("Page found at %d \n", pageFoundAt);
                pageTable[pageFoundAt].lruID = operations;
            }

            printf("\n");// tabela
            for(j = 0; j < numPages; j++){     
                printf("%s ", pageTable[j].addr);
            }
            printf("\n");


        }

        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

        printf("\nExecutando o simulador...\n");
        printf("Offset: %x\n", offset);
        printf("Arquivo de entrada: %s\n", file);
	    printf("Tamanho da memória: %iKB\n", memSize);
	    printf("Tamanho das páginas: %iKB\n", pageSize);
	    printf("Técnica de reposição: %s\n", alg);
	    printf("Número de páginas: %i\n", numPages);
	    printf("Operações no arquivo de entrada: %i\n", operations);
	    printf("Operações de leitura: %i\n", reads);
	    printf("Operações de escrita: %i\n", writes);
        printf("Número de pagefaults: %d\n", pageFaults);
        printf("Tempo de execução: %lf segundos \n \n", time_spent);

        fclose(fileOpen);

    }
    else{
        printf("ERRO: Arquivo de entrada inválido.\n");
        return 0;
    }
    return 1;
}