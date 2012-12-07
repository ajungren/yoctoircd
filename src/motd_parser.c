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

char **parse_motd(const char *path) {
    char **motd, *motd_data;
    FILE *motd_handle;

    motd_handle = fopen(path, "r");
    if(motd_handle == NULL) {
        perror(path);
        return NULL;
    }

    motd_data = fread_all(motd_handle, YOCTO_MAX_MOTD_SIZE);
    if(motd_data == NULL) {
        fclose(motd_handle);
        return NULL;
    }
    fclose(motd_handle);

    motd = split_lines(motd_data);
    safe_free(motd_data);

    return motd;
}
