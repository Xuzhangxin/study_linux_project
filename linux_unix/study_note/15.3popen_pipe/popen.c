#include "apue.h"
#include <sys/sem.h>
int main(int argc, char **argv)
{
    FILE *file_r = NULL;
    FILE *file_w = NULL;
    char buffer[1024 + 1];
    memset(buffer, 0, 1024 + 1);
    file_r = popen("ls -l", "r");
    if (file_r == NULL) {
        err_sys("popen error");
    }
    file_w = fopen("test.txt", "w");
    if (file_w == NULL) {
        err_sys("f_open error");
    }
    fread(buffer, sizeof(buffer[0]), 1024, file_r);
    fwrite(buffer, sizeof(buffer[0]), 1024, file_w);

    pclose(file_r);
    fclose(file_w);

    return 0;
}