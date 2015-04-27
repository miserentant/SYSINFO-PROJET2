#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <endian.h>

int main(int argc, const char *argv[]){

long un =be64toh(24);
long deux = be64toh(20);
long trois = be64toh(9);
long tab[3] = {un,deux,trois};
int fd;
fd = open("test.txt", O_WRONLY, NULL);
if(fd==-1) {
	printf("error");
}
int i;
int err;
for(i=0;i<3;i++){
printf("valeur : %ld\n", tab[i]);
err = write(fd, (void *) &(tab[i]), sizeof(long));
}



}
