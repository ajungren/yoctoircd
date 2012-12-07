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

#include <arpa/inet.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>

void start_server(int socket) {
    int incoming_socket;
    int32_t pid;
    socklen_t sockaddr_size = sizeof(struct sockaddr_in);
    struct sockaddr_in remote_addr;
    uint8_t errors = 0;

    listen(socket, YOCTO_MAX_PENDING_CONNS);
    while((incoming_socket = accept(socket, (struct sockaddr *) &remote_addr, &sockaddr_size))) {
        if(incoming_socket < 0) {
            if(errno != EINTR) {
                perror("Could not accept incoming connection");

                if(++errors > 5)
                    exit(EXIT_FAILURE);
            }

            continue;
        }
        else
            errors = 0;

        printf("Incoming connection from %s:%hu\n", inet_ntoa(remote_addr.sin_addr), remote_addr.sin_port);
        pid = fork();

        /* If forking was unsuccessful, exit. Because fork() not working is an
         * Extremely Bad Sign(tm), don't keep trying, just exit. */
        if(pid < 0) {
            perror("Could not create new process");
            exit(EXIT_FAILURE);
        }

        /* fork() returns 0 in the child process */
        if(pid == 0) {
            close(socket);
            start_child(incoming_socket);
            exit(EXIT_SUCCESS);
        }
        else
            close(incoming_socket);
    }
}

