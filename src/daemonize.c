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

#include <assert.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

void daemonize() {
    int retval;

    assert(globals->devnull >= 0);

    /* If the parent process is PID 1 (init), we have already daemonized. */
    if(getppid() == 1)
        return;

    retval = fork();
    if(retval < 0)
        exit(EXIT_FAILURE);
    else if(retval > 0)
        /* The parent process should exit so the child process can go in the
         * background. */
        exit(EXIT_SUCCESS);

    setsid();
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    dup2(globals->devnull, STDIN_FILENO);
    dup2(globals->devnull, STDOUT_FILENO);
    dup2(globals->devnull, STDERR_FILENO);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}

