//
// Created by 严洪钧 on 2022/11/19.
//

#include "path_helper.h"
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)

void merge_path(char *dest, const char *cwd, const char *path) {
    if (path[0] == '/')
        strcpy(dest, path);
    else {
        if (cwd[strlen(cwd) - 1] == '/')
            sprintf(dest, "%s%s", cwd, path);
        else
            sprintf(dest, "%s/%s", cwd, path);
    }
}

void t_mkdir(const char *dirname) {
    ASSERT(mkdir(dirname, MODE) == 0, ERR_MKDIR);
}

void t_chdir(char *cwd, size_t size, const char *dirname) {
    ASSERT(chdir(dirname) == 0, ERR_CHDIR);
    getcwd(cwd, size);
}
