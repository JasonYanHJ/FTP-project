//
// Created by Ethan on 2022/11/18.
//

#include "file_io_helper.h"

void t_remove(const char *path) {
    ASSERT(remove(path) == 0, ERR_DELETE);
}