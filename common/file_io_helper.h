//
// Created by Ethan on 2022/11/18.
//

#ifndef PROJECT_FILE_IO_HELPER_H
#define PROJECT_FILE_IO_HELPER_H
#include <stdio.h>
#include "try_catch.h"

#define _for_in_fp(BUF_SIZE, buffer, read_cnt, fp) char buffer[BUF_SIZE]; \
 for(size_t read_cnt = 0; (read_cnt = fread(&buffer, sizeof(char), BUF_SIZE, fp)) != 0;)

void t_remove(const char *path);

#endif //PROJECT_FILE_IO_HELPER_H
