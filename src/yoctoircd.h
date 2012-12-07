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

#ifndef __YOCTOIRCD_H
#define __YOCTOIRCD_H

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>

#include "../config.h"

#if !defined YOCTO_MOD_HTTP && !defined YOCTO_MOD_IRC
#error You have not enabled any modules; please edit config.h and enable at least one
#endif

/***** Program Information *****/

#define YOCTO_NAME "YoctoIRCd"
#define YOCTO_VERSION "0.1-git"

/***** Type definitions *****/

typedef struct {
    char *motd_path, *server_address, *server_name, *chroot_folder, *set_user, *set_group;
    uint16_t port, server_address_length, server_name_length, timeout;
} config_t;

typedef struct {
    char *config_path, **motd;
    int devnull, socket;
    struct sigaction child_signal_action;
} globals_t;

/***** Global variables *****/

#ifdef YOCTO_CONCRETE_EXTERNS
config_t *config;
globals_t *globals;
#else
extern config_t *config;
extern globals_t *globals;
#endif

/***** Function prototypes *****/

void child_signal_handler(int signum);
void daemonize();
void do_chroot(const char *path);
char *fread_all(FILE *handle, uint64_t maximum_length);
void free_config(config_t *config);
void free_motd(char **motd);
gid_t get_group(const char *group);
uid_t get_user(const char *user);
void parent_signal_handler(int signum);
config_t *parse_config(const char *path);
char **parse_motd(const char *path);
void set_group(gid_t gid);
void set_user(uid_t uid);
char **split_lines(char *string);
void start_child(int socket);
void start_server(int socket);
void write_wrapper(int handle, const char *buffer, size_t length);

/***** Macros *****/

#define safe_free(pointer) \
    if(pointer != NULL) { \
        free(pointer); \
        pointer = NULL; \
    }

#define safe_malloc(pointer, size) \
    pointer = malloc(size); \
    if(pointer == NULL) { \
        perror("Could not allocate memory"); \
        exit(EXIT_FAILURE); \
    }

#define safe_realloc(pointer, size) \
    pointer = realloc(pointer, size); \
    if(pointer == NULL) { \
        perror("Could not reallocate memory"); \
        exit(EXIT_FAILURE); \
    }

#endif

