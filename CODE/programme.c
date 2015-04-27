#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#define TRUE 1
#define FALSE 0
//CHECK partout pour les boolean

int main(int argc, const char *argv[]){

int N;
int err;
int i;

pthread_mutex_t mutex1;
sem_t empty1;
sem_t full1;
long **tabNbr;

char *tabFile;
int sizetabFile =0;
char *tabUrl;
int sizetabUrl =0;
int stdin_bool = FALSE;

//recherche des différents données sur les paramètres
for(i=1;i<argc;i++){
	if(strcmp("-maxthreads",argv[i])==0)
	N=atoi(argv[i+1]);

	if(strcmp("-stdin",argv[i])==0)
	stdin_bool = TRUE;

	if(strlen(argv[i])>13){ //nombre de caractère nécessaire pour une URL valide
		if(argv[i][4]==':'){
			
		} else {
			
		}
	} else {
		sizetabFile++;
 	}
	
}



//création des différents outils
err=pthread_mutex_init(&mutex1,NULL);
if(err==0){
err = sem_init(&empty1,0,N);
if(err==0){
err = sem_init(&full1,0,0);
}
}

//allocation de mémoire du tableau
tabNbr = malloc(sizeof(*tabNbr));
tabNbr[0]=calloc(N,sizeof(long));
tabNbr[1]=calloc(N,sizeof(long));



return(EXIT_SUCCESS);
}
