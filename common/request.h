//
// Created by 严洪钧 on 2022/11/18.
//

#ifndef PROJECT_REQUEST_H
#define PROJECT_REQUEST_H

#include <string.h>
#include "try_catch.h"

#define REQUEST_MAX_ARGS 4

struct request {
    int cmd_no;
    char command[16];
    char args[REQUEST_MAX_ARGS][BUF_SIZE];
};

void t_read_request(struct request *req, const char *raw_req_buf);
void t_verify_request(struct request *req);

#endif //PROJECT_REQUEST_H
