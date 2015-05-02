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
sem_t full3;
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
	err = sem_init(&full3,0,0);

	//allocation de mémoire des tableaux
	tabNbr = malloc(2*sizeof(*tabNbr)); //ok
	tabNbr[0]=calloc(N,sizeof(long));
	tabNbr[1]=calloc(N,sizeof(long));	
	
	tabFact = malloc(3*sizeof(*tabFact)); //ok
	tabFact[0]=calloc(N,sizeof(int));
	tabFact[1]=calloc(N,sizeof(int));
	tabFact[2]=calloc(N,sizeof(int));



	list = (struct prime *)malloc(sizeof(struct prime)); //ok
	struct prime *trois = (struct prime *)malloc(sizeof(struct prime));//ok
	trois->nombre=3;
	trois->compteur=0;
	trois->next=NULL;
	last = trois;
	list->nombre=2;
	list->compteur=0;
	list->next=trois;



	
	
}
/*
Cette fonction a donner en argument lors de la création du thread qui est consomteur du deuxième schéma de prod-cons et qui 
se charge de calculer les nombres premiers suivants
*/

void *calculateur(void *param){

	struct param2 *structure =(struct param2 *)param;
	//structure->tabFact = tabFact;
	//structure->list = list;
	int boolean =TRUE;
	int parcour;
	int nombre;
	int fichier;
	int mode; // Si 1, incrimenter un compteur, si 0, recherche d'un nb premier.
	struct prime *run;
	while(boolean){
		sem_wait(&full3); //attente de requete de calcul de nombre premier.
	
		pthread_mutex_lock(&mutex2);
		parcour =-1;
		do{   
			parcour++;
			//
			//
			if(parcour >= N){printf("On lui dit qu'il y a des entrées non vides mais elles sont vides 1\n");}
			//A RETIRER
			nombre = tabFact[0][parcour];
			mode = tabFact[1][parcour];
			//(structure->tabFact)[0][parcour]=0; //eviter de refactoriser plusieurs fois le même nombre.
		}while(nombre==0 || mode==1);
		//Changer ici
		tabFact[0][parcour]=0; //eviter de refactoriser plusieurs fois le même nombre.
		pthread_mutex_unlock(&mutex2);
		sem_post(&empty2);
		if(nombre==-1){
			
			boolean=FALSE;
		}
		
	//doit creer nombre premier suivant le nombre contenu dans "nombre". Seulement, pe qu'il a été calculé entre temps
//du coup il faut test si le dernier nombre premier de la liste est pas plus grand ou egal à ce nombre...
		else if( nombre >= last->nombre ){
			
			struct prime *diviseur = list;
			
			int nombreatest = nombre+2;
			int boolean2 =TRUE;
			while (boolean2){	
				if((nombreatest % diviseur->nombre) ==0){
				//Ce n'est pas un nombre premier. Il faut tester l'entier suivant.
					diviseur = list;
					nombreatest=nombreatest+2;
				}
				else{
					diviseur = diviseur->next;
					if(diviseur==NULL||( ((double) diviseur->nombre) > ((double) sqrt( (double) nombreatest)) )){
						boolean2 = FALSE;
					}
				}
			}	
			struct prime *suite = (struct prime *)malloc(sizeof(struct prime)); //ok
			suite->nombre = nombreatest;
			
			suite->compteur = 0;
			suite->next = NULL;
			last->next=suite; 
			last = suite;	
		}
		
	}
return NULL;
}


/*
Fonction a donner en argument lors de la création du thread qui est le consomateur du deuxième schéma prod-cons et qui se charge d'incrémenter les compteurs
*/
void *comptabilisateur(void *param){
	struct param2 *structure =(struct param2 *)param;
	//structure->tabFact = tabFact;
	//structure->list = list;
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
			if(parcour >= N){printf("On lui dit qu'il y a des entrées non vides mais elles sont vides2\n");}
			//A RETIRER
			nombre = tabFact[0][parcour];
			mode = tabFact[1][parcour];
			//(structure->tabFact)[0][parcour]=0; //eviter de refactoriser plusieurs fois le même nombre.
		}while(nombre==0 || mode==0);
		//Changer ici
		tabFact[0][parcour]=0; //eviter de refactoriser plusieurs fois le même nombre.
		
		fichier = tabFact[2][parcour];
	
		pthread_mutex_unlock(&mutex2);
		sem_post(&empty2);
		if(nombre==-1){
			
			boolean=FALSE;
		}
		else if(mode==1){
			
			run = list;
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
		else{	printf("Problème !! devrait rentrer dans else if mode : %d\n",mode);
		}
		
	}
return NULL;
}

//fonction du thread factorisateur.
void *factorisation(void *param){
	//structure->tabFact = tabFact;
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
			if(parcour >= N){printf("On lui dit qu'il y a des entrées non vides mais elles sont vides3\n");}
			//A RETIRER
			nombre = tabNbr[0][parcour];
			tabNbr[0][parcour]=0; //eviter de refactoriser plusieurs fois le même nombre. 
			parcour++;
		}while(nombre==0);
		fichier = tabNbr[1][parcour-1];
		pthread_mutex_unlock(&mutex1);
		sem_post(&empty1);
		if(nombre ==-1){ boolean =FALSE; //plus de nombre a tester
		}
		else{  //Cas ou il y a un nombre a factoriser
			struct prime *run = list;
			while(nombre> (long) 1){
				if(nombre % ((long) run->nombre)==0){
					nombre  = nombre / ((long) run->nombre);
					// Il faut incrémenter ce facteur en passant pas le dernier consomateur -> mettre dans le tableau d'échange
					parcour=-1; 
					sem_wait(&empty2);
					pthread_mutex_lock(&mutex2);
					do{	
						parcour++;
						if(parcour >= N){printf("On lui dit qu'il y a des entrées vides mais elles sont toutes non vides4\n");}
						//A RETIRER
						test=tabFact[0][parcour];
					}while(test!=0);
					tabFact[0][parcour]=run->nombre; // Permet de dire que c'est ce facteur là.
					tabFact[1][parcour]=1; //permet de dire qu'il faut incrémenter de 1.
					tabFact[2][parcour]=(int) fichier;
					pthread_mutex_unlock(&mutex2);
					sem_post(&full2);		
				}
				else if(run->next==NULL){
					// Il faut calculer le nombre premier suivent en passant par le dernier consomateur-> le dire dans le tableau d'échange
					parcour=-1; 
					sem_wait(&empty2);
					pthread_mutex_lock(&mutex2);
					do{	
						parcour++;
						if(parcour >= N){printf("On lui dit qu'il y a des entrées non vides mais elles sont vides5\n");}				
						//A RETIRER
						test=tabFact[0][parcour];
					}while(test!=0);
					tabFact[0][parcour]=run->nombre; // Permet de dire après quelle nombre premier il faut chercher le suivant.
					tabFact[1][parcour]=0; //permet de dire qu'il faut creer un objet de type struct prime
					tabFact[2][parcour]=(int) fichier;		
					pthread_mutex_unlock(&mutex2);
					sem_post(&full3);
					while(run->next==NULL){}//Boucle qui permet de ne lancer qu'une requete au thread comptabilisateur pour pas le submerger de requete identique
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
			if(tabNbr[0][iterator]== 0){ 		
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
//ICI
//long nbr;
int fd;
int err = 1;
//const char *filename;

for(it=0;it<sizetabFile;it++){//sizetabFile
	const char *filename = tabn[it];
	printf(":>IMPORT FILE : %s\n",filename);
	
	fd = open(filename, O_RDONLY, NULL);
	err=1;	
	while(err!=0 && err!=-1){
		long *nbr=malloc(sizeof(long)); //ICI
		
		err = read(fd, (void *) nbr, sizeof(long));//ici
		if(err!=0){
			if(err!=-1){
			*nbr = be64toh(*nbr);//ici
			insert(*nbr, FALSE, it+1);//ici 
			} else {
			printf("FILE '%s' NOT FOUND\n",filename);
			}
			free(nbr); //HERE
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
			insert(*nbr, FALSE, 0); //ici 
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
int count=0; // A retirer
int nbdefacteur=0; //nombre de nombre premiers qui ne sont facteur qu'une fois.
int fichierretour;
struct prime *run=list;
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
	count++;
}
printf("Prime factor count =  %d \n",nbdefacteur);
if(nbdefacteur!=1){
	printf("[ERROR]:More than one prime number used once\n");
}
else{
	printf("Researched prime number = %d\n[Found in file/URL:'%s']\n",retour,getFileName(fichierretour,file,url));
	printf("Count : %d", count);
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
tabFile = malloc(sizetabFile*sizeof(char *)); //ok
tabUrl = malloc(sizetabFile*sizeof(char *)); //ok
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
	err=pthread_create(&tabThread[j],NULL,&factorisation,NULL);
}

pthread_t compteur;
err=pthread_create(&compteur,NULL,&comptabilisateur,NULL);
pthread_t calcul;
err=pthread_create(&calcul,NULL,&calculateur,NULL);

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

boolean_wait=TRUE;
while(boolean_wait){
	iterateur =0;
	err=pthread_mutex_lock(&mutex2);

	do{
		if(iterateur==N){
			boolean_wait=FALSE;
			elem=1;//permet de sortir du do...while
			tabFact[0][0]=-1;
			tabFact[0][1]=-1;
			tabFact[1][0]=1;
			tabFact[1][1]=0;
			// fichier = (structure->tabNbr)[1][parcour]; C'est quoi cette daube?
			printf("On lui a dit de finir\n");
			sem_post(&full2);
			sem_post(&full3);
		}
		else{
			elem = tabFact[0][iterateur];
			iterateur++;
		}
	} while(elem==0);
	err=pthread_mutex_unlock(&mutex2);
}

err=pthread_join(compteur,NULL);
err=pthread_join(calcul,NULL);


showResults(tabFile, tabUrl);
//Libération de la mémoire
free(arg1);
free(arg2);
free(tabFile);
free(tabUrl);
free(tabNbr[0]);
free(tabNbr[1]);
free(tabFact[0]);
free(tabFact[1]);
free(tabFact[2]);
free(tabNbr);
free(tabFact);
//PAS oublier de libérer tableau!!
//Regarder comment on est sensé retourner les valeurs!!!!
//printf("%s\n",tabFile[1]);
return(EXIT_SUCCESS);
}
