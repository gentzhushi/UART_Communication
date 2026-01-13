/**
 * This is the source cod eif the program that will 
 * recieve bytes over the simulated UART
 */

#include<fcntl.h>
#include<sys/mman.h>
#include<stdio.h>
#include<time.h>

#define _POSIX_C_SOURCE 200809L
#define TIME 1000

void read(unsigned char *out, unsigned char *in, struct timespec cycle);

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

        volatile unsigned char *p = mmap(
                        NULL, 1,
                        PROT_READ,
                        MAP_SHARED,
                        fd, 0
                        );
        
        if (p == MAP_FAILED){
                perror("mmap");
                return -1;
        }

        printf("Wire connected.\n");

        struct timespec half_cycle = {
        .tv_sec = TIME/2 / 1000,
        .tv_nsec= ((TIME/2) % 1000) * 1000 * 1000
        };

        struct timespec full_cycle = {
        .tv_sec = TIME/2 / 1000,
        .tv_nsec= ((TIME/2) % 1000) * 1000 * 1000
        }; 
        
        while (1) {
		printf("\rp:%d",*p);
                if (p[0] == 0) { // falling edge
                        nanosleep(&half_cycle, NULL);
                        if (p[0] != 0) nanosleep(&half_cycle, NULL);
                        nanosleep(&full_cycle, NULL);

                        unsigned char byte;
                        read(&byte, p, full_cycle);
                }
        }

        return 0;
}

void read(unsigned char *out, unsigned char *in, struct timespec cycle){
	printf("trying to read...");
        for (int i = 7, digit = 128; i >= 0; i--, digit /= 2){
                if (*in == 1) *out = *out | digit;
                else *out = *out | 0;
                
                nanosleep(&cycle, NULL);
        }
        
        if(*in != 1) printf("u bo berllog [framing error]");
	
	printf("Bajti qe u lexu: %c", *out);

        return;
}
