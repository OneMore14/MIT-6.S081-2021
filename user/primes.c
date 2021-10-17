#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int* in) {

    close(in[1]);
    int prime;
    if (read(in[0], &prime, sizeof(int)) <= 0) {
        close(in[0]);
        exit(0);
    }
    printf("prime %d\n", prime);

    int out[2]; pipe(out);
    int pid = fork();
    if (pid == 0) {

        primes(out);
        exit(0);
    }

    close(out[0]);
    int number;
    while (read(in[0], &number, sizeof(int)) > 0)
    {
        if (number % prime) {
            write(out[1], &number, sizeof(int));
        }
    }
    close(in[0]);
    close(out[1]);
    wait(0);
}

int
main(int argc, char *argv[])
{

    int fd[2];
    pipe(fd);
    int pid = fork();
    if (pid == 0) {

        primes(fd);
        exit(0);
    }
    close(fd[0]);
    for (int i = 2; i <= 35;i++) {
        write(fd[1], &i, sizeof(int));
    }
    close(fd[1]);

    wait(0);
    exit(0);
}
