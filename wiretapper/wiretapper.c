#include<stdio.h>
#include<sys/fcntl.h>
#include<sys/mman.h>
#include<time.h>

#define TIME 1000

void print_char(unsigned char byte);
unsigned char read_byte(volatile unsigned char *p);
void sleep_ms(long ms);

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

        fprintf(
                stderr,
                "Shared Memory opened. file_descriptor:%d\n",
                fd
        );

        volatile unsigned char *p = mmap(
                NULL, 1,
                PROT_READ,
                MAP_SHARED,
                fd, 0
        );

        fprintf(stderr, "Wire caught.\n");

        if (p == MAP_FAILED) {
                perror("mmap");
                return -1;
        }

        unsigned char byte = 1;
        unsigned char previous_byte = 1;

        while (1) {
                fprintf(stderr, "\rprev:%d\tcurr:%d\t",
                                previous_byte,
                                p[0]
                        );

                if (p[0] == 0 && previous_byte == 1){ // falling edge
                        fprintf(stderr, "\nDetected edge.\n");
                        byte = read_byte(p);
                }

                previous_byte = p[0];
                sleep_ms(TIME);
        }

}

unsigned char read_byte(volatile unsigned char *p) {
        unsigned char byte = 0;

        // wait for half a cycle to verify if transmitter wants to send
        sleep_ms(1.5 * TIME);

        // if was false alarm
        // if (p[0] != 0) return -1;

        // if not false alarm, start sampling every time @ the center
        // sleep_ms(TIME);

        fprintf(stderr, "Started reading...\n");

        for (int i = 7; i >= 0; i--) {
                fprintf(stderr, "Gjendja e bajtit: "); print_char(byte);

                byte |= *p << i;

                sleep_ms(TIME);
        }

        fprintf(stderr, "\nBajti i lexum: %c\n", byte);
        fprintf(stderr, "Në binar: "); print_char(byte);

        return byte;
}

void print_char(unsigned char byte) {
        for (int i = 7; i >= 0; i--)
                fprintf(stderr, "%c", (byte & (1 << i)) ? '1' : '0' );

        fprintf(stderr, "\n");
}

void sleep_ms(long ms){
        struct timespec normal_ts = {
                ms / 1000,
                (ms % 1000) * 1000 * 1000
        };

        nanosleep(&normal_ts, NULL);
}