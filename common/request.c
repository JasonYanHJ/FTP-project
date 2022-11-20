//
// Created by 严洪钧 on 2022/11/18.
//

#include "request.h"

void init_request(struct request *req) {
    req->cmd_no = CMD_UNKNOWN;
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
            ASSERT(raw_req_buf[current] != '\"', ERR_REQUEST_SYNTAX);
            current++;
        }
        if (deli == '\"') {
            ASSERT(raw_req_buf[current] == deli, ERR_REQUEST_SYNTAX);
            ASSERT(current + 1 == strlen(raw_req_buf) || raw_req_buf[current + 1] == ' ' || raw_req_buf[current + 1] == '\n', ERR_REQUEST_SYNTAX);
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
        ASSERT(segment_cnt < REQUEST_MAX_ARGS, ERR_TOO_MANY_ARGS);

        start = current + 1;
    }
}

void t_verify_request(struct request *req) {
    if (strcmp(req->command, "put") == 0)
        req->cmd_no = CMD_PUT;
    else if (strcmp(req->command, "get") == 0)
        req->cmd_no = CMD_GET;
    else if (strcmp(req->command, "ls") == 0)
        req->cmd_no = CMD_LS;
    else if (strcmp(req->command, "cd") == 0)
        req->cmd_no = CMD_CD;
    else if (strcmp(req->command, "pwd") == 0)
        req->cmd_no = CMD_PWD;
    else if (strcmp(req->command, "mkdir") == 0)
        req->cmd_no = CMD_MKDIR;
    else if (strcmp(req->command, "delete") == 0)
        req->cmd_no = CMD_DELETE;
    else if (strcmp(req->command, "quit") == 0)
        req->cmd_no = CMD_QUIT;
    else
        throw(ERR_UNKNOWN_CMD);

    switch (req->cmd_no) {
        case CMD_LS: case CMD_PWD: case CMD_QUIT:
            ASSERT(strlen(req->args[0]) == 0, ERR_TOO_MANY_ARGS);
            ASSERT(req->cmd_no != CMD_QUIT, SUC_REQ_QUIT);
            break;
        case CMD_CD: case CMD_MKDIR: case CMD_DELETE:
            ASSERT(strlen(req->args[0]) != 0, ERR_TOO_FEW_ARGS);
            ASSERT(strlen(req->args[1]) == 0, ERR_TOO_MANY_ARGS);
            break;
        case CMD_GET: case CMD_PUT:
            ASSERT(strlen(req->args[1]) != 0, ERR_TOO_FEW_ARGS);
            ASSERT(strlen(req->args[3]) == 0, ERR_TOO_MANY_ARGS);
            if (strlen(req->args[2]) != 0)
                ASSERT(strcmp(req->args[2], "ascii") == 0, ERR_INVALID_ARGS);
            break;
    }
}