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
#include <grp.h>
#include <pwd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

void do_chroot(const char *path) {
    if(chroot(path) == -1) {
        perror("Could not chroot");
        exit(EXIT_FAILURE);
    }

    chdir("/");
}

gid_t get_group(const char *group) {
    struct group *group_entry;

    errno = 0;
    group_entry = getgrnam(group);
    if(group_entry == NULL) {
        perror("Could not find group");
        exit(EXIT_FAILURE);
    }

    return group_entry->gr_gid;
}

uid_t get_user(const char *user) {
    struct passwd *user_entry;

    errno = 0;
    user_entry = getpwnam(user);
    if(user_entry == NULL) {
        perror("Could not find user");
        exit(EXIT_FAILURE);
    }

    return user_entry->pw_uid;
}

void set_group(gid_t gid) {
    if(setgid(gid) == -1) {
        perror("Could not set group");
        exit(EXIT_FAILURE);
    }
}

void set_user(uid_t uid) {
    if(setuid(uid) == -1) {
        perror("Could not set user");
        exit(EXIT_FAILURE);
    }
}

