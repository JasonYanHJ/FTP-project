//
// Created by Ethan on 2022/11/18.
//

#ifndef PROJECT_TRANSFER_PACKET_H
#define PROJECT_TRANSFER_PACKET_H

#define BUF_SIZE 256

struct Header{
    unsigned int buffer_size;
    char ctrl_bits;
};

struct Packet{
    struct Header header;
    char buffer[(BUF_SIZE - sizeof(struct Header))];
};

#endif //PROJECT_TRANSFER_PACKET_H
