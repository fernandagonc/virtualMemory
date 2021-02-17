#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]){
    char *alg, *file, line[20], tmpAddress[9];

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
    if(strlen(file) > 0){
        FILE *fileOpen = fopen(file, "r");
        
        while(fgets(line, 20, fileOpen) != NULL){
            operations++;
            // strncpy(tmpAddress, line, 8);
            // tmpAddress[8] = '\0';
            if(line[9] == 'W' || line[9] == 'w'){
                writes++;
            //     WriteAddress(tmpAddress);
             }
             else if(line[9] == 'R' || line[9] == 'r'){				
            //     if(Find(tmpAddress)){
            //         hits++;	
                // }
            //     else{
            //         misses++;
            //         WriteAddress(tmpAddress);
            //     }
                reads++;
             }
            // else{
            //     printf("ERRO: Os dados do arquivo de entrada estao em formato incorreto.");
            //     return 0;	
            // }	
        }
        printf("Operações: %d \n Read: %d Write: %d", operations, reads, writes);
    }
    else{
        printf("ERRO: Arquivo de entrada inválido.");
        return 0;
    }
    return 1;
}