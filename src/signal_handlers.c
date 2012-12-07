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

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void child_signal_handler(int signum) {
    close(globals->socket);
    exit(EXIT_FAILURE);
}

void parent_signal_handler(int signum) {
    void *buffer;

    switch(signum) {
        case SIGINT:
            fputs("Exiting on SIGINT\n", stderr);
            break;
        case SIGTERM:
            fputs("Exiting on SIGTERM\n", stderr);
            break;
        case SIGUSR1:
            fputs("Reloading MOTD\n", stderr);
            buffer = (void *) parse_motd(config->motd_path);
            if(buffer != NULL) {
                free_motd(globals->motd);
                globals->motd = (char **) buffer;
            }
            return;
        case SIGUSR2:
            fputs("Reloading configuration\n", stderr);
            buffer = (void *) parse_config(globals->config_path);
            if(buffer != NULL) {
                free_config(config);
                config = (config_t *) buffer;
                buffer = (void *) parse_motd(config->motd_path);
                if(buffer != NULL) {
                    free_motd(globals->motd);
                    globals->motd = (char **) buffer;
                }
            }
            return;
        default:
            fputs("Exiting on unknown signal\n", stderr);
    }

    close(globals->socket);
    exit(EXIT_SUCCESS);
}

