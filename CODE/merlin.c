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
//#include "curlopen.h"
#define TRUE 1
#define FALSE 0
#define OVER -5
#define POSSTD 0
//CHECK partout pour les boolean




//structure d'argument (Thread1)
struct tabArgThread1{
	const char **tab;
};
//structure utilisée pour la liste chainée de nombres
//premiers
struct prime{ 
	int nombre; 
	int compteur; 
	int fichier; 
	struct prime *next; 
}; 
//structure qui doit être passée en argument pour les threads qui gèrent la factorisation!
struct param1{
	
	int N; //indique la taille des tableaux d'échange prod-cons
	//sem_t *empty1; //echange dans prod-cons 1
	//sem_t *full1;  //echange dans prod-cons 1
	//pthread_mutex_t mutex1; //echange dans prod-cons 1
 	long **tabNbr;   //echange dans prod-cons 1

	struct prime *list; //reférence vers la liste des nb premiers

	//sem_t empty2; //echange dans prod-cons 2
	//sem_t full2;  //echange dans prod-cons 2
	//pthread_mutex_t mutex2; //echange dans prod-cons 2
	int **tabFact; //echange dans le prod-cons 2
};
//structure qui doit être passée en argument pour le thread qui gere la liste !
struct param2{
	

	int N; //indique la taille du tableau d'échange prod-cons

	struct prime *list; //reférence vers la liste des nb premiers

	//sem_t empty2; //echange dans prod-cons 2
	//sem_t full2;  //echange dans prod-cons 2
	//pthread_mutex_t mutex2; //echange dans prod-cons 2
	int **tabFact; //echange dans le prod-cons 2
};

//variables globales
int sizetabUrl =0;
int sizetabFile =0;
int N;
int err;
long **tabNbr;
int **tabFact;
pthread_mutex_t mutex1;
sem_t empty1;
sem_t full1;
pthread_mutex_t mutex2;
sem_t empty2;
sem_t full2;
struct param1 Arg1;
struct param2 Arg2;
struct prime *list;
struct prime *last;
int countSource;

//initialise les sémaphores/mutex ainsi que les structures d'arguments de threads
void init(){

	err=pthread_mutex_init(&mutex1,NULL);
	err = sem_init(&empty1,0,N);
	err = sem_init(&full1,0,0);
	err=pthread_mutex_init(&mutex2,NULL);
	err = sem_init(&empty2,0,N);
	err = sem_init(&full2,0,0);

	//allocation de mémoire des tableaux
	tabNbr = malloc(2*sizeof(*tabNbr));
	tabNbr[0]=calloc(N,sizeof(long));
	tabNbr[1]=calloc(N,sizeof(long));	
	
	tabFact = malloc(3*sizeof(*tabFact));
	tabFact[0]=calloc(N,sizeof(int));
	tabFact[1]=calloc(N,sizeof(int));
	tabFact[2]=calloc(N,sizeof(int));

	Arg1.N=N;
	//Arg1.empty1 =&empty1;
	//Arg1.full1=&full1;
	//Arg1.mutex1=mutex1;
	Arg1.tabNbr=tabNbr;
	//Arg1.empty2 = empty2;
	//Arg1.full2=full2;
	//Arg1.mutex2=mutex2;

	list = (struct prime *)malloc(sizeof(struct prime));
	last = list;
	list->nombre=2;
	list->compteur=0;
	list->next=NULL;

	Arg1.list=list;
	Arg2.N=N;
	Arg2.list=list;
	//Arg2.empty2=empty2;
	//Arg2.full2=full2;
	//Arg2.mutex2=mutex2;
	Arg2.tabFact=tabFact;

	
	
}

/*
Fonction a donner en argument lors de la création du thread qui est le consomateur du deuxième schéma prod-cons.
Cette fonction permet de calculer un argument 
*/
void *comptabilisateur(void *param){
	struct param2 *structure =(struct param2 *)param;
	structure->tabFact = tabFact;
	structure->list = list;
	int boolean =TRUE;
	int parcour;
	int nombre;
	int fichier;
	int mode; // Si 1, incrimenter un compteur, si 0, recherche d'un nb premier.
	struct prime *run;
	while(boolean){ // Touver un moyen de dire quand il faut arrêter!
		sem_wait(&full2); //attente de chiffre.	
	
		pthread_mutex_lock(&mutex2);
		parcour =-1;
		do{   
			parcour++;
			//
			//
			if(parcour >= structure->N){printf("On lui dit qu'il y a des entrées non vides mais elles sont vides\n");}
			//A RETIRER
			nombre = (structure->tabFact)[0][parcour];
			(structure->tabFact)[0][parcour]=0; //eviter de refactoriser plusieurs fois le même nombre.
		}while(nombre==0);
		
		mode = (structure->tabFact)[1][parcour];
		
		fichier = (structure->tabFact)[2][parcour];
	
		pthread_mutex_unlock(&mutex2);
		sem_post(&empty2);
		if(nombre==-1){
			
			boolean=FALSE;
		}
		else if(mode==1){
			
			run = structure->list;
			while(run->nombre!=nombre){
				run = run->next;
				if(run==NULL){printf("Probleme, on donne un compteur d'un facteur a incrémenter mais le facteur n'existe pas\n");
				//commentaire a enlever
				}
			}
			run->fichier=fichier;
			run->compteur++;
			
		}
	//doit creer nombre premier suivant le nombre contenu dans "nombre". Seulement, pe qu'il a été calculé entre temps
//du coup il faut test si le dernier nombre premier de la liste est pas plus grand ou egal à ce nombre...
		else if(mode==0 && (nombre >= last->nombre) ){
			
			struct prime *diviseur = structure->list;
			
			int nombreatest = nombre+1;
			int boolean2 =TRUE;
			while (boolean2){	
				if((nombreatest % diviseur->nombre) ==0){
				//Ce n'est pas un nombre premier. Il faut tester l'entier suivant.
					diviseur = structure->list;
					nombreatest++;
				}
				else{
					diviseur = diviseur->next;
					if(diviseur==NULL||( ((double) diviseur->nombre) > ((double) sqrt( (double) nombreatest)) )){
						boolean2 = FALSE;
					}
				}
			}	
			struct prime *suite = (struct prime *)malloc(sizeof(struct prime));
			suite->nombre = nombreatest;
			
			suite->compteur = 0;
			suite->next = NULL;
			last->next=suite; 
			last = suite;	
		}
		
	}
return NULL;
}

//fonction du thread factorisateur.
void *factorisation(void *param){
	
	struct param1 *structure = (struct param1 *)param;
	structure->tabFact = tabFact;
	int boolean = TRUE;
	long nombre;
	long fichier;
	int parcour;
	int test;
	while(boolean){ //tant qu'il y a des chiffres a factoriser. 

		sem_wait(&full1); //attente de chiffre.	
	
		pthread_mutex_lock(&mutex1);
		parcour =0;
		do{   // les threads peuvent aller chercher les nombre n'importe ou dans le premier buffer. 
			//
			//
			if(parcour >= structure->N){printf("On lui dit qu'il y a des entrées non vides mais elles sont vides");}
			//A RETIRER
			
			nombre = (structure->tabNbr)[0][parcour];
			
			(structure->tabNbr)[0][parcour]=0; //eviter de refactoriser plusieurs fois le même nombre. 
			parcour++;
		}while(nombre==0);
		
		fichier = (structure->tabNbr)[1][parcour-1];
		pthread_mutex_unlock(&mutex1);
		sem_post(&empty1);
		if(nombre ==-1){ boolean =FALSE; //plus de nombre a tester
		
		}
		else{  //Cas ou il y a un nombre a factoriser
			
			struct prime *run = structure->list;
			
			while(nombre> (long) 1){
				
				if(nombre % ((long) run->nombre)==0){
					
					nombre  = nombre / ((long) run->nombre);
				
					// Il faut incrémenter ce facteur en passant pas le dernier consomateur
					parcour=-1; 
					sem_wait(&empty2);
					pthread_mutex_lock(&mutex2);
					do{	
						parcour++;
						if(parcour >= structure->N){printf("On lui dit qu'il y a des entrées vides mais elles sont toutes non vides");}
						//A RETIRER
						
						test=(structure->tabFact)[0][parcour];
						
					}while(test!=0);
					(structure->tabFact)[0][parcour]=run->nombre; // Permet de dire que c'est ce facteur là.
					(structure->tabFact)[1][parcour]=1; //permet de dire qu'il faut incrémenter de 1.
					(structure->tabFact)[2][parcour]=(int) fichier;

					pthread_mutex_unlock(&mutex2);
					sem_post(&full2);
					
				}
				else if(run->next==NULL){

					// Il faut calculer le nombre premier suivent en passant par le dernier consomateur
					// Dans prime.c je faisais : run = ajoutprime(list,run);
					parcour=-1; 
					sem_wait(&empty2);
					pthread_mutex_lock(&mutex2);
					
					do{	
						
						parcour++;
						
						if(parcour >= structure->N){printf("On lui dit qu'il y a des entrées non vides mais elles sont vides");}
						
						//A RETIRER
						
						test=(structure->tabFact)[0][parcour];
						
					}while(test!=0);
					
					(structure->tabFact)[0][parcour]=run->nombre; // Permet de dire après quelle nombre premier il faut chercher le suivant.
					(structure->tabFact)[1][parcour]=0; //permet de dire qu'il faut creer un objet de type struct prime
					(structure->tabFact)[2][parcour]=(int) fichier;
					
					pthread_mutex_unlock(&mutex2);
					sem_post(&full2);
				}
				else{
					run = run->next;
				}
			}
		}
	}
	//Pe faut le mettre a la place de boolean = FALSE;
	return NULL;
		
		

}





//fonction d'insertion de nbr dans le tableau
void insert(long nbr, int stdin_b,int pos){
err = sem_wait(&empty1);
	err=pthread_mutex_lock(&mutex1);
		int boolean= TRUE;
		int iterator =0;
		while(boolean){
			if(tabNbr[0][iterator]== 0){ // Putain charles, ta merde m'a bien fait iech.
				
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
//const char *filename;

for(it=0;it<sizetabFile;it++){//sizetabFile
	const char *filename = tabn[it];
	printf(":>IMPORT FILE : %s\n",filename);
	
	fd = open(filename, O_RDONLY, NULL);
	err=1;	
	while(err!=0 && err!=-1){
		long *nbr=malloc(sizeof(long));
		err = read(fd, (void *) nbr, sizeof(long));
		if(err!=0){
			if(err!=-1){
			*nbr = be64toh(*nbr);
			insert(*nbr, FALSE, it+1);
			} else {
			printf("FILE '%s' NOT FOUND\n",filename);
			}
		}
	}
	close(fd);
}
return NULL;
}

//fonction de chargement de nombre via Stdin.
void * importFromStdin(void * tabl){

int err = 1;

	printf(":>READING FROM STDIN\n");
	err=1;	
	while(err!=0 && err!=-1){
		long *nbr=malloc(sizeof(long));
		err = read(0, (void *) nbr, sizeof(long));
		if(err!=0){
			if(err!=-1){
			*nbr = be64toh(*nbr);
			insert(*nbr, FALSE, 0);
			} else {
			printf("STDIN NOT FOUND\n");
			}
		}
	}

return NULL;
}

//retourne le nom de fichier du prime trouvé grace à son identifiant
const char * getFileName(int id, const char **file, const char **url){
const char *retour;
	if(id==0){
		retour="STDIN";
	} 
	else if(id>0) {
		retour = file[id-1];
	} 
	else {
		retour = url[-id-1];
	}
return retour;
}

//Affiche les résultats
void showResults(const char **file, const char **url){
int retour;
int nbdefacteur=0; //nombre de nombre premiers qui ne sont facteur qu'une fois.
int fichierretour;
struct prime *run=Arg1.list;
struct prime *suivant;
while(run!=NULL){
	//printf("Facteur : %d  compteur : %d\n",run->nombre,run->compteur);
	if(run->compteur==1){
		retour=run->nombre;
		fichierretour=run->fichier;
		nbdefacteur++;
	}
	suivant=run->next;
	free(run);
	run=suivant;
}
printf("Prime factor count =  %d \n",nbdefacteur);
if(nbdefacteur!=1){
	printf("[ERROR]:More than one prime number used once\n");
}
else{
	printf("Researched prime number = %d\n[Found in file/URL:'%s']\n",retour,getFileName(fichierretour,file,url));
}
}


//MAIN FUNCTION
int main(int argc, const char *argv[]){
int i;

const char **tabFile;
const char **tabUrl;
int stdin_bool = FALSE;

pthread_t file;
pthread_t url;
pthread_t Stdin;

//recherche des différents données sur les paramètres
for(i=1;i<argc;i++){
	if(strcmp("-maxthreads",argv[i])==0){
	
	N=atoi(argv[i+1]);

	i++;}
	else if(strcmp("-stdin",argv[i])==0){
	stdin_bool = TRUE;}
	else if(strlen(argv[i])>9){ //nombre de caractère nécessaire pour une URL valide
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
	if(strlen(argv[i])>9){ //nombre de caractère nécessaire pour une URL valide A CHANGER P E
		if(argv[i][4]==':'){
			tabUrl[runnerurl] = argv[i];
			runnerurl++;
		} else {
			tabFile[runnerf] = argv[i];
			runnerf++;
		}
	} else {
		tabFile[runnerf] = argv[i];
		runnerf++;
 	}
	
}


//intialisation
init();

//création des arguments pour les threads de récupération
struct tabArgThread1 *arg1 = malloc(sizeof(struct tabArgThread1));
arg1->tab = tabFile;
struct tabArgThread1 *arg2 = malloc(sizeof(struct tabArgThread1));
arg2->tab = tabUrl;

//lancement des threads de récupération
if(stdin_bool){
err=pthread_create(&Stdin, NULL, &importFromStdin, NULL);
}
err = pthread_create(&file, NULL,&importFromFile,(void *) arg1);
//err = pthread_create(&url, NULL,&importFromUrl,(void *) arg2);


pthread_t tabThread[N];
int j;
for(j=0;j<N;j++){
	err=pthread_create(&tabThread[j],NULL,&factorisation,(void *) &Arg1);
}

pthread_t compteur;
err=pthread_create(&compteur,NULL,&comptabilisateur,(void *) &Arg2);


err=pthread_join(file,NULL);
//err=pthread_join(url,NULL);
if(stdin_bool){
err=pthread_join(Stdin,NULL);
}

int boolean_wait = TRUE;
int elem;
int iterateur;

while(boolean_wait){
	iterateur =0;
	err=pthread_mutex_lock(&mutex1);
	elem= 0;
	while(elem==0){
	
		
		if(iterateur==N){
			boolean_wait=FALSE;
			elem=1;//permet de sortir du do...while
			
			for(j=0;j<N;j++){
				tabNbr[0][j]=-1; 
				
				sem_post(&full1);
			}
		}
		else{
			elem = tabNbr[0][iterateur];
			iterateur++;
		}
	} 
	err=pthread_mutex_unlock(&mutex1);
}

for(j=0;j<N;j++){
	err=pthread_join(tabThread[j],NULL);
}

boolean_wait=1;
while(boolean_wait){
	iterateur =0;
	err=pthread_mutex_lock(&mutex2);

	do{
		if(iterateur==N){
			boolean_wait=FALSE;
			elem=1;//permet de sortir du do...while
			tabFact[0][0]=-1;
			// fichier = (structure->tabNbr)[1][parcour]; C'est quoi cette daube?
			sem_post(&full2);
		}
		else{
			elem = tabFact[0][iterateur];
			iterateur++;
		}
	} while(elem==0);
	err=pthread_mutex_unlock(&mutex2);
}

err=pthread_join(compteur,NULL);



showResults(tabFile, tabUrl);


//PAS oublier de libérer tableau!!
//Regarder comment on est sensé retourner les valeurs!!!!
//printf("%s\n",tabFile[1]);
return(EXIT_SUCCESS);
}
