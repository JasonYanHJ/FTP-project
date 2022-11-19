#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../common/exitable_functions.h"
#include "../common/defines.h"
#include "../common/socket_helper.h"
#include "../common/try_catch.h"

void create_data_socket(int *socket_data, int socket_control);

int main(int argc, char *argv[])
{
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(1);
    }

    int socket_control;
    e_socket_connect(&socket_control, e_get_server_addr(argv[1], atoi(argv[2])));

    char buffer[256];
    while (1) {
        try {
            printf("Please enter the message: ");
            bzero(buffer, 256);
            fgets(buffer, 255, stdin);
            if (strncmp(buffer, "quit", 4) == 0)
                break;
            e_write(socket_control, buffer, strlen(buffer));

            int ret_code;
            e_read(socket_control, &ret_code, sizeof(ret_code));
            ASSERT(ret_code == 0, ret_code)

            int socket_data;
            create_data_socket(&socket_data, socket_control);

            bzero(buffer, BUF_SIZE);
            ssize_t n;
            while ((n = e_read(socket_data, buffer, BUF_SIZE - 1))) {
                buffer[n] = '\0';
                printf("%s(%lu)\n", buffer, n);
            }

            close(socket_data);
        }
        catch(err_no) {
            printf("err_no: %d\n", err_no);
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
