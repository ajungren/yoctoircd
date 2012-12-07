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

#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

void start_child(int socket) {
    char buffer[YOCTO_NET_BUF_SIZE];
    int32_t index, retval;

    signal(SIGUSR1, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);

    memset(buffer, 0, YOCTO_NET_BUF_SIZE);

    /* Prepare the alarm handler that closes the connection when the timeout is
     * reached. */
    globals->socket = socket;
    sigaction(SIGALRM, &globals->child_signal_action, NULL);
    alarm(config->timeout);

    while(1) {
        retval = read(socket, buffer, YOCTO_NET_BUF_SIZE);
        if(retval < 0) {
            perror("Could not read from socket");
            close(socket);
            exit(EXIT_FAILURE);
        }

#ifdef YOCTO_MOD_HTTP
        /* This is a very basic (and NOT standards compliant) HTTP 1.1 server */
        if(retval >= 5 && memcmp(buffer, "GET /", 5) == 0) {
            write_wrapper(socket, "HTTP/1.1 200 OK\n", 16);
            write_wrapper(socket, "Connection: close\n", 18);
            write_wrapper(socket, "Content-Type: text/html\n\n", 25);
            write_wrapper(socket, "<html><head><title>", 19);
            write_wrapper(socket, config->server_name, config->server_name_length);
            write_wrapper(socket, "</title></head><body><h2>", 25);
            write_wrapper(socket, config->server_name, config->server_name_length);
            write_wrapper(socket, "</h2><h3>IRC server at ", 23);
            write_wrapper(socket, config->server_address, config->server_address_length);
            write_wrapper(socket, "</h2><pre>", 10);

            index = -1;
            while(globals->motd[++index] != NULL) {
                write_wrapper(socket, globals->motd[index], strlen(globals->motd[index]));
                write_wrapper(socket, "\n", 1);
            }

            write_wrapper(socket, "</pre></body></html>", 20);
            break;
        }
#endif

#ifdef YOCTO_MOD_IRC
        if(retval >= 5 && (memcmp(buffer, "NICK ", 5) == 0 || memcmp(buffer, "USER ", 5) == 0)) {
            write_wrapper(socket, ":", 1);
            write_wrapper(socket, config->server_address, config->server_address_length);
            write_wrapper(socket, " 375 :- ", 8);
            write_wrapper(socket, config->server_name, config->server_name_length);
            write_wrapper(socket, " Message of the Day:\r\n", 22);

            index = -1;
            while(globals->motd[++index] != NULL) {
                write_wrapper(socket, ":", 1);
                write_wrapper(socket, config->server_address, config->server_address_length);
                write_wrapper(socket, " 372 :- ", 8);
                write_wrapper(socket, globals->motd[index], strlen(globals->motd[index]));
                write_wrapper(socket, "\r\n", 2);
            }

            write_wrapper(socket, ":", 1);
            write_wrapper(socket, config->server_address, config->server_address_length);
            write_wrapper(socket, " 376 :- End of MOTD\r\n", 21);
            write_wrapper(socket, ":", 1);
            write_wrapper(socket, config->server_address, config->server_address_length);
            write_wrapper(socket, " 465 :- Goodbye\r\n", 17);
            break;
        }
#endif
    }

    shutdown(socket, SHUT_RDWR);
    close(socket);
}

