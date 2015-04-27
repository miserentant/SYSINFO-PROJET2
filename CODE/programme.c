#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

void * importFile

int main(int argc, const char *argv[]){
//création des différents variables et outils
int N;
pthread_mutex_t nbrTab;
sem_t empty1;
sem_t full1;
int err;
long **tabNbr;

//création des sémaphores et du mutex
err=pthread_mutex_init(&nbrTab,NULL);
if(err==0){
	err=sem_init(&empty1,0,N);
	if(err==0){
		err=sem_init(&full1,0,0);
	}
}

//analyse des paramètre au cas par cas
int i;
for(i=1;i<argc;i++){
//on considère que cet argument est en premier.
if(strcmp(argv[i],"-maxthreads")==0){
	N=atoi(argv[i+1]);
	tabNbr = calloc(sizeof(*tabNbr));
	tabNbr[0] = calloc(N*sizeof(long));
	tabNbr[1] = calloc(N*sizeof(int));
}

if(strcmp(argv[i],"-stdin")==0){
	printf("stdin repéré \n");
}

if(strlen(argv[i])>13){
	if(argv[i][4] == ':'){
		printf("HTTP trouvée \n");
	} else {
		printf("fichier trouvé \n");
	}
} else {
	printf("fichier trouver. NAME=%s\n",argv[i]);
}

}

pthread_t bolos;
err=pthread_create(&bolos,NULL,&show,NULL);
return(EXIT_SUCCESS);
}
