#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{

    if (argc < 2) {
        fprintf(2, "Usage: xargs command ...\n");
        exit(1);
    }

    int pid = fork();
    if (pid == 0) {

        char* args[MAXARG];
        for (int i = 1; i < argc; i++) {
            args[i - 1] = argv[i];
        }

        int param = argc - 1;
        char buf[512];
        int i = 0;
        int pre = 0;
        while (read(0, buf + i, 1) > 0)
        {
            if (buf[i] == '\n') {
                buf[i] = '\0';
                args[param++] = buf + pre;
                pre = i + 1;
            }
            i++;
        }

        args[argc - 1] = buf;
        exec(argv[1], args);
    }
    wait(0);
    exit(0);
}
