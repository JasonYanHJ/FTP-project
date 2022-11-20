//
// Created by 严洪钧 on 2022/11/19.
//

#include "path_helper.h"
#define MODE (S_IRWXU | S_IRWXG | S_IRWXO)

void t_mkdir(const char *dirname) {
    ASSERT(mkdir(dirname, MODE) == 0, ERR_MKDIR);
}

void t_chdir(char *cwd, size_t size, const char *dirname) {
    ASSERT(chdir(dirname) == 0, ERR_CHDIR);
    getcwd(cwd, size);
}
