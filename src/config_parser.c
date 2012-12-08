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

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define parse_error \
    safe_free(config->motd_path); \
    safe_free(config->server_address); \
    safe_free(config->server_name); \
    safe_free(config); \
    return NULL;

#define TOKEN_SEP "\n"

config_t *parse_config(const char *path) {
    char *config_data, *config_token, *config_value;
    FILE *config_file;
    config_t *config;
    uint8_t missing_options = 0;
    uint16_t index, length;

    config_file = fopen(path, "r");
    if(config_file == NULL) {
        perror(path);
        return NULL;
    }

    config_data = fread_all(config_file, YOCTO_FILE_BUF_SIZE);
    if(config_data == NULL)
        return NULL;

    safe_malloc(config, sizeof(config_t));
    config->port = 0;
    config->timeout = 0;
    config->motd_path = NULL;
    config->server_address = NULL;
    config->server_name = NULL;
    config->chroot_folder = NULL;
    config->set_user = NULL;
    config->set_group = NULL;

    config_token = strtok(config_data, TOKEN_SEP);
    while(config_token != NULL) {
        if(config_token[0] == '\0' || config_token[0] == '#') {
            config_token = strtok(NULL, TOKEN_SEP);
            continue;
        }

        config_value = strchr(config_token, '=');
        if(config_value == NULL) {
            fprintf(stderr, "Malformed line: \"%s\"\n", config_token);
            parse_error;
        }
        else {
            /* This line is somewhat of a hack. config_value points to the =,
             * which should be part of neither the key nor the value. So, the =
             * is replaced with a NULL byte, and the config_value pointer is
             * incremented, so that it points to the first character of the
             * value. In effect, this line splits the key and value completely. */
            *config_value++ = '\0';

            if(strcmp(config_token, "Port") == 0) {
                config->port = atoi(config_value);
                if(config->port == 0) {
                    fprintf(stderr, "Invalid port: \"%s\"\n", config_value);
                    parse_error;
                }
            }

            else if(strcmp(config_token, "Timeout") == 0) {
                config->timeout = atoi(config_value);
                if(config->timeout == 0) {
                    fprintf(stderr, "Invalid timeout: \"%s\"\n", config_value);
                    parse_error;
                }
            }

            else if(strcmp(config_token, "MotdPath") == 0) {
                safe_malloc(config->motd_path, strlen(config_value)+1);
                strcpy(config->motd_path, config_value);
            }

            else if(strcmp(config_token, "ServerAddress") == 0) {
                index = 0;
                while(config_value[index] != '\0') {
                    if(config_value[index++] == ' ') {
                        fprintf(stderr, "Invalid server address: \"%s\" (spaces not allowed)\n", config_value);
                        parse_error;
                    }
                }
                length = strlen(config_value);
                safe_malloc(config->server_address, length+1);
                strcpy(config->server_address, config_value);
                config->server_address_length = length;
            }

            else if(strcmp(config_token, "ServerName") == 0) {
                length = strlen(config_value);
                safe_malloc(config->server_name, length+1);
                strcpy(config->server_name, config_value);
                config->server_name_length = length;
            }

            else if(strcmp(config_token, "ChrootFolder") == 0) {
                safe_malloc(config->chroot_folder, strlen(config_value)+1);
                strcpy(config->chroot_folder, config_value);
            }

            else if(strcmp(config_token, "SetUser") == 0) {
                safe_malloc(config->set_user, strlen(config_value)+1);
                strcpy(config->set_user, config_value);
            }

            else if(strcmp(config_token, "SetGroup") == 0) {
                safe_malloc(config->set_group, strlen(config_value)+1);
                strcpy(config->set_group, config_value);
            }

            else {
                fprintf(stderr, "Unknown configuration key: \"%s\"\n", config_value);
                parse_error;
            }
        }

        config_token = strtok(NULL, TOKEN_SEP);
    }

    if(config->port == 0) {
        fputs("Missing configuration option: Port\n", stderr);
        missing_options++;
    }
    if(config->timeout == 0) {
        fputs("Missing configuration option: Timeout\n", stderr);
        missing_options++;
    }
    if(config->motd_path == NULL) {
        fputs("Missing configuration option: MotdPath\n", stderr);
        missing_options++;
    }
    if(config->server_address == NULL) {
        fputs("Missing configuration option: ServerAddress\n", stderr);
        missing_options++;
    }
    if(config->server_name == NULL) {
        fputs("Missing configuration option: ServerName\n", stderr);
        missing_options++;
    }

    if(missing_options > 0) {
        if(missing_options > 1)
            fprintf(stderr, "%" PRIu8 " configuration options are missing\n", missing_options);
        parse_error;
    }

    return config;
}

