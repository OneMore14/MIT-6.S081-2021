#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"


void find(char* path, char* file) {

    char buf[512], *p;
    int fd;
    struct stat st;
    struct dirent de;

    if((fd = open(path, 0)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    if (st.type != T_DIR) {
        //fprintf(2, "find: %s is not a directory\n", path);
        return;
    }

    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }

      if (strcmp(file, de.name) == 0) {
          printf("%s\n", buf);
      }
      if (st.type == T_DIR && strcmp(".", de.name) && strcmp("..", de.name)) {
          find(buf, file);
      }
    }
}

int
main(int argc, char *argv[])
{
    char* file_name = argv[2];
    char* path = argv[1];
    if (argc < 2) {
        fprintf(2, "usage: find directory path\n");
        exit(1);
    } else if (argc == 2) {
        path = ".";
    }

    find(path, file_name);

    exit(0);
}
