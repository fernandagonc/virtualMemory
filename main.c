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

} PageTableEntry;

void writeOnTable(int pageNumber, PageTableEntry * pageTable){
    pageTable[pageNumber].validBit = 1;
    if(pageTable[pageNumber].dirtyBit == -1){
        pageTable[pageNumber].dirtyBit = 0;
    }
    else if(pageTable[pageNumber].dirtyBit == 0){
        pageTable[pageNumber].dirtyBit = 1;
    }
    
    //write

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
    char addr[8];
    char rw;
    int i, j;
    unsigned int addrInt;
    PageTableEntry pageTable[numPages];
    for(i = 0; i < numPages; i++){
        pageTable[i].dirtyBit = -1;
        pageTable[i].validBit = 0;
    }

    
    if(strlen(file) > 0){
        FILE *fileOpen = fopen(file, "r");
        
        clock_t begin = clock();

        while(fscanf(fileOpen,"%s %c", addr,&rw) != EOF){
            operations++;
            addrInt = (int)strtol(addr, NULL, 16);
            // printf("addr: %d\n", addrInt);
            virtualPageNumber = addrInt >> offset;

            if(rw == 'W' || rw == 'w'){
                writes++;
                if(pageTable[virtualPageNumber].validBit == 0){
                    writeOnTable(virtualPageNumber, pageTable);

                }
                else{ //find free page
                    for (j=0;j<numPages;j++){
						if(pageTable[j].validBit == 0){
                            writeOnTable(j, pageTable);
                            break;
						}
					}
                    if(j == numPages){ //no free pages
                        writeBacks++;
                        //algoritmos de substituição
                    }	
                }

            }
            else if(rw == 'R' || rw == 'r'){				
                reads++;
                if(pageTable[virtualPageNumber].validBit == 0){//empty page
                    pageFaults++;
                }

            }

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
        printf("Tempo de execução: %lf segundos \n \n", time_spent);

    }
    else{
        printf("ERRO: Arquivo de entrada inválido.");
        return 0;
    }
    return 1;
}