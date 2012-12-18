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

#include <stdlib.h>
#include <string.h>

char **split_lines(char *string) {
    char **split, *token;
    int64_t index;
    size_t length;
    uint32_t lines = 0;

    length = strlen(string);
    for(index = 0; index < (int64_t) length; index++) {
        if(string[index] == '\n')
            lines++;
    }

    safe_malloc(split, sizeof(char *) * (lines + 1));
    split[lines] = NULL;

    index = 0;
    token = strtok(string, "\n");
    while(token != NULL) {
        split[index] = malloc(strlen(token) + 1);
        if(split[index] == NULL) {
            while(--index >= 0) {
                safe_free(split[index]);
            }
            safe_free(split);

            return NULL;
        }
        strcpy(split[index], token);

        index++;
        token = strtok(NULL, "\n");
    }

    return split;
}

