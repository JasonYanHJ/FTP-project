//
// Created by 严洪钧 on 2022/11/19.
//

#ifndef PROJECT_PATH_HELPER_H
#define PROJECT_PATH_HELPER_H

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/stat.h>

void path_back(char *path);
void path_add(char *path, const char *dirname);

#endif //PROJECT_PATH_HELPER_H
