#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <endian.h>
#include <fcntl.h>
#include <math.h>
#define TRUE 1
#define FALSE 0
#define OVER -5
#define POSSTD 0
//CHECK partout pour les boolean

//structure d'argument (Thread1)
struct tabArgThread1{
	const char **tab;
};

//variables globales
int sizetabUrl =0;
int sizetabFile =0;
int err;
long **tabNbr;
pthread_mutex_t mutex1;
sem_t empty1;
sem_t full1;
int countSource;

//fonction d'insertion de nbr dans le tableau
void insert(long nbr, int stdin_b,int pos){
err = sem_wait(&empty1);
	if(pthread_mutex_lock(&mutex1)==0){
		int boolean= TRUE;
		int iterator =0;
		while(boolean==TRUE){
			if(tabNbr[iterator]== (long) 0){
				printf("ajout : %ld\n",nbr);
				tabNbr[0][iterator]=nbr;
				boolean=FALSE;
				if(stdin_b==TRUE){
				tabNbr[1][iterator]=POSSTD;
				} else {
				tabNbr[1][iterator]=pos;
				}
				
			}
			iterator++;
		}
		err = pthread_mutex_unlock(&mutex1);
	}
err = sem_post(&full1);

}

//fonction de chargement de nombre via des fichiers locaux.
void * importFromFile(void * tabl){
int it;
struct tabArgThread1 *ptr = (struct tabArgThread1 *) tabl;
const char **tabn;
tabn=ptr->tab;

int fd;
int err = 1;
const char *filename;

for(it=0;it<sizetabFile;it++){
	filename = tabn[it];
	fd = open(filename, O_RDONLY, NULL);	
	while(err!=0){
		long *nbr = malloc(sizeof(long));
		err = read(fd, (void *) nbr, sizeof(long));
		*nbr = be64toh(*nbr);
		insert(*nbr, FALSE, it);
	}
	close(fd);
}
return NULL;
}


int main(int argc, const char *argv[]){

int N;
int i;


const char **tabFile;
const char **tabUrl;
int stdin_bool = FALSE;

pthread_t file; 

//recherche des différents données sur les paramètres
for(i=1;i<argc;i++){
	if(strcmp("-maxthreads",argv[i])==0){
	N=atoi(argv[i+1]);
	i++;}
	else if(strcmp("-stdin",argv[i])==0){
	stdin_bool = TRUE;}
	else if(strlen(argv[i])>13){ //nombre de caractère nécessaire pour une URL valide
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
	if(strcmp("-maxthreads",argv[i])==0 || strcmp("-stdin",argv[i])==0){
	i++;} else 
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
err = sem_init(&empty1,0,N);
err = sem_init(&full1,0,0);


//allocation de mémoire des tableaux
tabNbr = malloc(sizeof(*tabNbr));
tabNbr[0]=calloc(N,sizeof(long));
tabNbr[1]=calloc(N,sizeof(long));


//lancement des threads de récupération
struct tabArgThread1 *arg = malloc(sizeof(struct tabArgThread1));
arg->tab = tabFile;
if(stdin_bool){
printf("STDIN_OK\n");
}
err = pthread_create(&file, NULL,&importFromFile,(void *) arg);
sleep(5);


return(EXIT_SUCCESS);
}

