//
// Created by 严洪钧 on 2022/11/19.
//

#include "path_helper.h"

void path_back(char *path) {
    for (int i = strlen(path) - 1; i >= 0; i--) {
        if (path[i] == '/') {
            path[i] = '\0';
            return;
        }
    }
}

void path_add(char *path, const char *dirname) {
    strcat(path, "/");
    strcat(path, dirname);
}
