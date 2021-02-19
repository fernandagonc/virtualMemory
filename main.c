#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
// https://www.alltestanswers.com/designing-a-virtual-memory-manager-using-c-program/
// https://github.com/ranadeep47/virtual-memory-simulation
// https://github.com/8tiqa/vm-mgt/blob/master/vm.c
// https://github.com/joelrlneto/memoriavirtual/blob/master/SMV.c



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
    int s = 0;
    int tmpPageSize = pageSize;
    while (tmpPageSize > 1) {
        tmpPageSize = tmpPageSize >> 1;
        s++;
    }

    unsigned page;
    unsigned addr;
    char rw;
    
    if(strlen(file) > 0){
        FILE *fileOpen = fopen(file, "r");
        
        clock_t begin = clock();

        while(fscanf(fileOpen,"%x %c",&addr,&rw) != EOF){
            operations++;
            
            // page = tmpAddress >> s;
            
            if(rw == 'W' || rw == 'w'){
                writes++;
            //     WriteAddress(tmpAddress);
             }
             else if(rw == 'R' || rw == 'r'){				
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
        clock_t end = clock();
        double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;



        printf("\nExecutando o simulador...\n");
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