#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

    int p2c[2];
    int c2p[2];
    pipe(p2c);
    pipe(c2p);

    int pid = fork();
    if (pid == 0) {

        close(p2c[1]);
        close(c2p[0]);

        char ping;
        if (read(p2c[0], &ping, 1) == 1) {
            printf("%d: received ping\n", getpid());
        }
        close(p2c[0]);

        char pong = '#';
        write(c2p[1], &pong, 1);
        close(c2p[1]);

        exit(0);
    }

    close(p2c[0]);
    char ping = '#';
    write(p2c[1], &ping, 1);
    close(p2c[1]);

    char pong;
    if (read(c2p[0], &pong, 1) == 1) {
        printf("%d: received pong\n", getpid());
    }
    close(c2p[0]);

    exit(0);
}
