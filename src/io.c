/*
 * This file is part of YoctoIRCd.
 * Copyright (C) 2012 Ayron Jungren
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "yoctoircd.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

char *fread_all(FILE *handle, uint64_t maximum_length) {
    char *buffer;
    size_t bytes_read = 0, retval;
    uint64_t length = 0;

    safe_malloc(buffer, YOCTO_FILE_BUF_SIZE);

    while(!feof(handle)) {
        length += YOCTO_FILE_BUF_SIZE;
        if(length > maximum_length)
            break;

        safe_realloc(buffer, length);

        /* A few notes:
         * - fread is used instead of fgets because fread does not add a NULL to
         *   the end of buffer
         * - buffer+length-YOCTO_FILE_BUF_SIZE is the first byte that hasn't been written
         *   to yet
         * - sizeof(char) is 1 as per C specifications
         */
        retval = fread(buffer+length-YOCTO_FILE_BUF_SIZE, 1, YOCTO_FILE_BUF_SIZE, handle);
        if(retval == 0) {
            safe_free(buffer);
            perror("Could not read file");
            return NULL;
        }
        bytes_read += retval;
    }

    /* bytes_read+1 because a NULL byte needs to be added */
    safe_realloc(buffer, (size_t) bytes_read+1);

    buffer[bytes_read] = '\0';
    return buffer;
}

void write_wrapper(int handle, const char *buffer, size_t length) {
    ssize_t bytes_written;

    while(length > 0) {
        bytes_written = write(handle, buffer, length);
        if(bytes_written < (ssize_t) length && errno != EINTR) {
            perror("Could not write to socket");
            exit(EXIT_FAILURE);
        }

        buffer += bytes_written;
        length -= (size_t) bytes_written;
    }
}

