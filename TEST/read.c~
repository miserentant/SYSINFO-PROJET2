#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <endian.h>
#include <fcntl.h>

int main(int argc, const char *argv[]){
int err = 0;
long *s = malloc(sizeof(long));
long *s2 = malloc(sizeof(long));
int fd;
fd = open("test.txt", O_RDONLY, NULL);
if(fd==-1) {
	printf("error");
}
int i;
for(i=0;i<5;i++){
int err = read(fd, (void *) s, sizeof(long));
printf("donnée err: %d\n",err);
*s = be64toh(*s);
printf("%ld\n",*s);
}
free(s);
err= close(fd);
}
