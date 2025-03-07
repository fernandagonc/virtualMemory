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
	
	if(strcmp(alg, "lru") && strcmp(alg, "fifo") && strcmp(alg,"2a") && strcmp(alg, "lr")){
		printf("ERRO: O algoritmo deve ser lru, fifo, 2a ou lr(least referenced).");
		return 0;	
	}

    int numPages = memSize/pageSize;
    int i, j, operations = 0, writes = 0, reads = 0, noCommand = 0, pageFaults = 0, writeBacks = 0, hits=0;
    PageTableEntry pageTable[numPages];

    for(i = 0; i < numPages; i++){
        pageTable[i].pageNumber = -1;
        pageTable[i].validBit = 0;
        pageTable[i].dirtyBit = -1;
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

    int secondChanceIt = 0;
    int leastReferencedIt = 0;
    int oppW, oppR;
    
    FILE *fileOpen = fopen(file, "r");  
    if(fileOpen != NULL){     
        printf("\nExecutando o simulador...\n");
        printf("TABELA DE SUBSTIUIÇÕES INICIAL\n");
        printf("\n");// tabela
        printf("  Endereço | Dirty Bit \t \n");
        printf("-----------------------------\n");

        for(j = 0; j < numPages; j++){     
            printf("%10x | %i \t \n", pageTable[j].pageNumber, pageTable[j].dirtyBit);
        }
        printf("\n");  

        clock_t begin = clock();


        while(fscanf(fileOpen,"%s %c", addr,&rw) != EOF){
            
            oppW = rw == 'W' || rw == 'w';
            oppR = rw == 'R' || rw == 'r';

            if(oppW || oppR){

                if(oppW){
                    writes++;     
                }
                else if(oppR){				
                    reads++;
                }

                operations++;

                addr[8] = '\0';
                
                addrInt = (int)strtol(addr, NULL, 16);
                virtualPageNumber = addrInt >> offset;
                pageFoundAt = findAddress(pageTable, addr, numPages);


                if(pageFoundAt == -1){//page not found
                    pageFaults++;
                    freePageAt = findFreeAddress(pageTable, numPages);
                    if(freePageAt == -1){//no free page
                        if(oppW)
                            writeBacks++;

                        if(!strcmp(alg, "lru")){
                            freePageAt = LRU(pageTable, numPages);
                            writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                            pageTable[freePageAt].algID = operations;
                        }
                        else if(!strcmp(alg, "2a")){
                            freePageAt = secondChance(pageTable, numPages, secondChanceIt);
                            writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                            secondChanceIt = freePageAt+1;
                        }
                        else if(!strcmp(alg, "fifo")){
                            freePageAt = FIFO(pageTable, numPages);
                            writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                            pageTable[freePageAt].algID = operations;
                        }
                        else if(!strcmp(alg, "lr")){
                            freePageAt = LeastReferenced(pageTable, numPages, leastReferencedIt);
                            writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                            leastReferencedIt = freePageAt+1;
                            pageTable[freePageAt].algID = 1;                         
                        }
                    }
                    else{
                        writeOnTable(freePageAt, pageTable, addr, virtualPageNumber);
                        if(!strcmp(alg, "lru") || !strcmp(alg, "fifo"))
                            pageTable[freePageAt].algID = operations;
                        if(!strcmp(alg, "lr"))
                            pageTable[freePageAt].algID++; 
                    }
                }
                else{
                    hits++;
                    if(pageTable[pageFoundAt].algID == 0 && !strcmp(alg, "2a"))
                        pageTable[pageFoundAt].algID = 1;
                    if(!strcmp(alg, "lru"))
                        pageTable[pageFoundAt].algID = operations;
                    if(!strcmp(alg, "lr"))
                        pageTable[pageFoundAt].algID++;
                }

            }
            else{
                noCommand++;
            }

        }


        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
        fclose(fileOpen);

        printf("TABELA DE SUBSTIUIÇÕES FINAL\n");
        printf("\n");// tabela
        printf("  Endereço | Dirty Bit \t \n");
        printf("-----------------------------\n");
        for(j = 0; j < numPages; j++){     
            printf("%10x | %5i \t \n", pageTable[j].pageNumber, pageTable[j].dirtyBit);
        }
        printf("\n");  


        printf("Offset: %x\n", offset);
        printf("Arquivo de entrada: %s\n", file);
	    printf("Tamanho da memória: %iKB\n", memSize);
	    printf("Tamanho das páginas: %iKB\n", pageSize);
	    printf("Técnica de reposição: %s\n", alg);
	    printf("Número de páginas: %i\n", numPages);
	    printf("Operações no arquivo de entrada: %i\n", operations);
        printf("Linhas sem comandos: %i\n", noCommand);
	    printf("Operações de leitura: %i\n", reads);
	    printf("Operações de escrita: %i\n", writes);
        printf("Número de pagefaults: %d\n", pageFaults);
        printf("Número de páginas sujas: %d\n", writeBacks);
        printf("Número de acertos de páginas : %d\n", hits);
        printf("Tempo de execução: %lf segundos \n \n", time_spent);


    }
    else{
        printf("ERRO: Arquivo de entrada inválido.\n");
        return 0;
    }
    return 1;
}