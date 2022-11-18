//
// Created by 严洪钧 on 2022/11/18.
//

#include "request.h"

void init_request(struct request *req) {
    req->command[0] = '\0';
    for (int i = 0; i < REQUEST_MAX_ARGS; i++) {
        req->args[i][0] = '\0';
    }
}

void t_read_request(struct request *req, const char *raw_req_buf) {
    init_request(req);

    int start = 0, current;
    int segment_cnt = -1;
    while (start < strlen(raw_req_buf)) {
        // trim
        if (raw_req_buf[start] == ' ' || raw_req_buf[start] == '\n') {
            start++;
            continue;
        }

        // determine the delimiter
        char deli = ' ';
        if (raw_req_buf[start] == '\"') {
            deli = '\"';
            start++;
        }
        current = start;

        // find the position of delimiter
        while (raw_req_buf[current] != deli && raw_req_buf[current] != '\n' && current < strlen(raw_req_buf)) {
            ASSERT(raw_req_buf[current] != '\"', ERR_REQUEST_SYNTAX)
            current++;
        }
        if (deli == '\"') {
            ASSERT(raw_req_buf[current] == deli, ERR_REQUEST_SYNTAX)
            ASSERT(current + 1 == strlen(raw_req_buf) || raw_req_buf[current + 1] == ' ' || raw_req_buf[current + 1] == '\n', ERR_REQUEST_SYNTAX)
        }

        // write to req
        if (segment_cnt == -1) {
            memcpy(req->command, raw_req_buf + start, current - start);
            req->command[current - start] = '\0';
        } else {
            memcpy(req->args[segment_cnt], raw_req_buf + start, current - start);
            req->args[segment_cnt][current - start] = '\0';
        }
        segment_cnt++;
        ASSERT(segment_cnt < REQUEST_MAX_ARGS, ERR_REQUEST_SYNTAX);

        start = current + 1;
    }
}