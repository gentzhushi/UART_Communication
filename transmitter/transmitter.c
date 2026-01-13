/**
 * This is the source code of the program that will transmit 
 * data to the reciever program via simulated UART.
 */

#define _POSIX_C_SOURCE 200809L
#include<fcntl.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h>

#define TIME 1000

int setup(char *fname){
        shm_unlink(fname);
	int fd = shm_open(
			fname, 
			O_CREAT | O_RDWR,
			0600
			);

	if (fd == -1){
		perror("shm_open");
		return -1;
	}

	fprintf(stderr, "Shared Memory opened. file_descriptor:%d\n", fd);

	if (ftruncate(fd, 1) == -1){
		perror("ftruncate");
		return -1;
	}
	
	fprintf(stderr, "Wire created.\n");

	return fd;
}

void char2bits(unsigned char buff, bool* bits){
        for (int i = 7; i >= 0; i--, buff = buff >> 1)
                bits[i] = (buff % 2 == 1) ? 1 : 0;
}

static inline int wire_write(volatile unsigned char *p, bool value){

        if (p == MAP_FAILED){
                return -1;
        }

        p[0] = value;

        return 0;
}

static void sleep_ms(long ms){
        struct timespec ts;
        ts.tv_sec  = ms / 1000;
        ts.tv_nsec = (ms % 1000) * 1000 * 1000;

		nanosleep(&ts, NULL);
}

int send_bits2wire(bool* bits, int fd){
	volatile unsigned char *p = mmap(NULL, 1, PROT_WRITE, MAP_SHARED, fd, 0);

        if (wire_write(p, 0) == -1) return -1;
        sleep_ms(TIME);
        
        fprintf(stderr, "Tu e shkru: 0b0");

        for(int i = 0; i < 8; i++){
                fprintf(stderr, "%d", bits[i]);
                wire_write(p, bits[i]);
                sleep_ms(TIME);
        }
        
        wire_write(p, 1);
        fprintf(stderr, "1\n");
        
        return 0;
}

int loop(int fd){
        int cnt = 0;
	while (1){
                unsigned char buffer;

                fprintf(stderr, "\nShënoje shkronjën %d: ", ++cnt);
                scanf(" %c", &buffer);

                bool bits[8];
                char2bits(buffer, bits);

                if (send_bits2wire(bits, fd) == -1){
                        fprintf(stderr, "Error, s'mujtëm me shkru n'file.");
                        continue;
                }
	}

        return 0;
}

void shutdown(int fd){
	close(fd);
}

int main(void){
        // trap exit signal, and call shutdown

	int file_descriptor = setup("/wire");

        if (file_descriptor == -1) return 1;

	loop(file_descriptor);

	shutdown(file_descriptor);

	return 0;
}
