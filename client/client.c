#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../common/exitable_functions.h"
#include "../common/defines.h"
#include "../common/socket_helper.h"
#include "../common/try_catch.h"
#include "common/request.h"
#include "common/file_io_helper.h"

void create_data_socket(int *socket_data, int socket_control);

int main(int argc, char *argv[])
{
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }

    int socket_control;
    e_socket_connect(&socket_control, e_get_server_addr(argv[1], atoi(argv[2])));

    char buffer[BUF_SIZE];
    struct request req;
    while (1) {
        // read and send one request to server
        try {
            printf("Please enter the message: ");
            bzero(buffer, BUF_SIZE);
            fgets(buffer, BUF_SIZE - 1, stdin);

            // Turn request_string --> request_struct and send to server
            t_read_request(&req, buffer);
            if (strcmp(req.command, "quit") == 0)
                break;
            e_write(socket_control, &req, sizeof(struct request));

            t_expect_read_code(socket_control, 0);
        }
        catch(err_no) {
            printf("err_no: %d syntax_error!\n", err_no);
            continue;
        }

        // when request accepted, create one data socket to transfer data
        int socket_data;
        create_data_socket(&socket_data, socket_control);

        if (strcmp(req.command, "put") == 0) {
            const char *open_mode = "rb";
            try {
                FILE *fp = fopen(req.args[0], open_mode);
                ASSERT(fp != NULL, ERR_OPEN_LOCAL_FILE);
                e_write_code(socket_control, 0);

                _for_in_fp (BUF_SIZE, buffer_send, read_cnt, fp) {
                    e_write(socket_data, buffer_send, read_cnt);
                }
                fclose(fp);
            }
            catch(err_no) {
                e_write_code(socket_control, err_no);
            }
        } else if (strcmp(req.command, "get") == 0){
            const char* open_mode = "wb";
            try {
                t_expect_read_code(socket_control, 0);

                FILE *fp = fopen(req.args[1], open_mode);
                ASSERT(fp != NULL, ERR_OPEN_LOCAL_FILE);
                e_write_code(socket_control, 0);

                ssize_t n;
                while ((n = e_read(socket_data, buffer, BUF_SIZE))) {
                    fwrite(buffer, n, sizeof(char), fp);
                }
                fclose(fp);
            }
            catch(err_no) {
                e_write_code(socket_control, err_no);
            }
        } else {
            bzero(buffer, BUF_SIZE);
            ssize_t n;
            while ((n = e_read(socket_data, buffer, BUF_SIZE - 1))) {
                buffer[n] = '\0';
                printf("%s\n", buffer);
            }
        }

        close(socket_data);

        int ret_code = e_read_code(socket_control);
        if (ret_code != 0) {
            // TODO: print err info
            printf("err_no: %d\n", ret_code);
        }

    }

    close(socket_control);
    return 0;
}

void create_data_socket(int *socket_data, int socket_control) {
    int socket_listen;
    e_start_listen(&socket_listen, CLIENT_PORT);

    int ready = 1;
    e_write(socket_control, (char*) &ready, sizeof(ready));

    e_socket_accept(socket_data, socket_listen);
    close(socket_listen);
}
