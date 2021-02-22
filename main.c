#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "algorithm.c"

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

    int numPages = 4;
    int i, j, operations = 0, writes = 0, reads = 0, pageFaults = 0, writeBacks = 0;
    PageTableEntry pageTable[numPages];

    for(i = 0; i < numPages; i++){
        pageTable[i].dirtyBit = -1;
        pageTable[i].pageNumber = -1;
        pageTable[i].validBit = 0;
        memcpy(pageTable[i].addr, "", strlen("")+1);
        pageTable[i].algID = 0;

    }

    unsigned offset = 0; 
    int tmpPageSize = pageSize;
    while (tmpPageSize > 1) {
        tmpPageSize = tmpPageSize >> 1;
        offset++;
    }

    unsigned virtualPageNumber; 
    int pageFoundAt, freePageAt = 0;
    char addr[8];
    char rw;
    unsigned int addrInt;

    int secondChancePtr = 0;

    FILE *fileOpen = fopen(file, "r");  
    if(fileOpen != NULL){       
        clock_t begin = clock();
        printf("\nExecutando o simulador...\n");
        printf("TABELA DE SUBSTIUIÇÕES\n");

        while(fscanf(fileOpen,"%s %c", addr,&rw) != EOF){
            operations++;
            addr[8] = '\0';
            
            addrInt = (int)strtol(addr, NULL, 16);
            virtualPageNumber = addrInt >> offset;
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
                        writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                        pageTable[freePageAt].algID = operations;
                    }
                    else if(!strcmp(alg, "2a")){
                        freePageAt = secondChance(pageTable, numPages, secondChancePtr);
                        writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                        secondChancePtr = freePageAt+1;
                    }
                    else if(!strcmp(alg, "fifo")){
                        freePageAt = FIFO(pageTable, numPages);
                        writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                        pageTable[freePageAt].algID = operations;
                    }
                    // else{
                    
                    // }
                }
                else{
                    writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                    if(!strcmp(alg, "lru") || !strcmp(alg, "fifo"))
                        pageTable[freePageAt].algID = operations;
                }
            }
            else{
                if(pageTable[pageFoundAt].algID == 0 && !strcmp(alg, "2a"))
                    pageTable[pageFoundAt].algID = 1;
                if(!strcmp(alg, "lru"))
                    pageTable[pageFoundAt].algID = operations;
            }

            printf("\n");// tabela
            for(j = 0; j < numPages; j++){     
                printf("%d", pageTable[j].algID);
                printf("%s ", pageTable[j].addr);
            }
            printf("\n");

        }

        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

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