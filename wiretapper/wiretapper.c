#include<stdio.h>
#include<sys/fcntl.h>
#include<sys/mman.h>
#include<time.h>

#define TIME 1000

int main(){
        
        int fd = shm_open(
                        "/wire",
                        O_RDONLY,
                        0600
                        );

        if (fd == -1){
                perror("shm_open");
                return -1;
        }

        printf("shm_open ok, file_descriptor:%d\n", fd); 

        unsigned char *p = 
                mmap(NULL, 1, PROT_READ, MAP_SHARED, fd, 0);

        printf("Wire caught.\n");

        if (p == MAP_FAILED) {
                perror("mmap");
                return -1;
        }

        printf("timespecs starting...\n");

        struct timespec init_ts = {
                TIME / 1000,
                (TIME % 1000) * 1000 * 1000
        };

        struct timespec normal_ts = {
                TIME / 1000,
                (TIME % 1000) * 1000 * 1000
        };
        
        printf("timespecs done...\n");

        while (1) {
                p[0] == 1 ? printf("\rHIGH") : printf("\rLOW ");
                fflush(stdout);
                nanosleep(&normal_ts, NULL);
        }

}
