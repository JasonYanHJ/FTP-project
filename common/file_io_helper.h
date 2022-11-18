//
// Created by Ethan on 2022/11/18.
//

#ifndef PROJECT_FILE_IO_HELPER_H
#define PROJECT_FILE_IO_HELPER_H

#define _for_in_fp(N, buffer, read_cnt, fp) char buffer[N]; \
 for(size_t read_cnt = 0; (read_cnt = fread(&buffer, sizeof(char), N, fp))!=0;)


#endif //PROJECT_FILE_IO_HELPER_H
