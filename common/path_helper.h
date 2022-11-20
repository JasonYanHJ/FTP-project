//
// Created by 严洪钧 on 2022/11/19.
//

#ifndef PROJECT_PATH_HELPER_H
#define PROJECT_PATH_HELPER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "try_catch.h"

void merge_path(char *dest, const char *cwd, const char *path);

void t_mkdir(const char *dirname);
void t_chdir(char *cwd, size_t size, const char *dirname);

#endif //PROJECT_PATH_HELPER_H
