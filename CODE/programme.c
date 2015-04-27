#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#define TRUE 1
#define FALSE 0
//CHECK partout pour les boolean

void * importFromFile(char **tab){
printf("OK\n");
}

struct tabArgThread1{
	char **tab;
};

int main(int argc, const char *argv[]){

int N;
int err;
int i;

pthread_mutex_t mutex1;
sem_t empty1;
sem_t full1;
long **tabNbr;

const char **tabFile;
int sizetabFile =0;
const char **tabUrl;
int sizetabUrl =0;
int stdin_bool = FALSE;

pthread_t file; 

//recherche des différents données sur les paramètres
for(i=1;i<argc;i++){
	if(strcmp("-maxthreads",argv[i])==0)
	N=atoi(argv[i+1]);

	if(strcmp("-stdin",argv[i])==0)
	stdin_bool = TRUE;

	if(strlen(argv[i])>13){ //nombre de caractère nécessaire pour une URL valide
		if(argv[i][4]==':'){
			sizetabUrl++;
		} else {
			sizetabFile++;
		}
	} else {
		sizetabFile++;
 	}
	
}

//création des tableau d'argument par type
tabFile = malloc(sizetabFile*sizeof(char *));
tabUrl = malloc(sizetabFile*sizeof(char *));
int runnerf=0;
int runnerurl=0;
for(i=1;i<argc;i++){
	if(strlen(argv[i])>13){ //nombre de caractère nécessaire pour une URL valide
		if(argv[i][4]==':'){
			tabFile[runnerf] = argv[i];
			runnerf++;
		} else {
			tabUrl[runnerurl] = argv[i];
			runnerurl++;
		}
	} else {
		tabFile[runnerf] = argv[i];
		runnerf++;
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


//lancement des threads de récupération

err = pthread_create(&file, NULL,&importFromFile,(void *) tabFile);

return(EXIT_SUCCESS);
}
