#include <stdlib.h> 
#include <stdio.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h>
 #include <fcntl.h> 
#include<endian.h>
void f(int **tab, long *s){
	int i = 0;
	while(*s>1)
		if(tab[i][0]==0){printf("Error\n");}
		if( *s % ((long)tab[i][0])==0 ){
			tab[i][1]++;	
		}
		else{
			i++;
		}	
	}	
}
int main(int argc, const char *argv[]){ 
	int **tab;
	tab = malloc(20*sizeof(*tab));
	int j;
	for(j=0;j<20;j++){
		tab[j] = calloc(2,sizeof(int));
	}
	long *s=malloc(sizeof(long)); 
	tab[0][0]=2;
	tab[1][0]=3;
	tab[2][0]=5;
	tab[3][0]=7;
	int fd; 
	fd = open("test.txt", O_RDONLY, NULL); 
	if(fd==-1) { 
		printf("error"); 
	} 
	int i = 1; 
	while(i){
		if(read(fd, (void *) s, sizeof(long)) == 0){ 
			i =0;
		} 
		else{
			*s = be64toh(*s);
			printf("%ld\n",*s);
			f(tab,s);
		} 
	}
	free(s);
	int err = close(fd);
	if(err!=0){
		printf("error\n");	
	}
	i=1;
	int count =0;
	while(i){
		if(tab[count][1]==1){
			printf("C'est : %d\n",tab[count][0]);
			i=0;
		}
		count ++;
	}
}

