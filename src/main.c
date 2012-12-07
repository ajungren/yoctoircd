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

#define YOCTO_CONCRETE_EXTERNS
#include "yoctoircd.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv) {
    gid_t gid;
    struct sigaction parent_signal_action;
    struct sockaddr_in server_addr;
    uid_t uid;

    if(argc != 2) {
        fprintf(stderr, "Usage: %s CONFIG_FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        fprintf(stderr, "Usage: %s CONFIG_FILE\n" YOCTO_NAME " is a minimalist IRC server that only sends the MOTD and disconnects every client that connects to it.\n\n", argv[0]);
        fputs("CONFIG_FILE should be a configuration file in the format described at:\nhttps://github.com/dririan/yoctoircd/master/yoctoircd.cfg", stderr);
        return EXIT_SUCCESS;
    }

    if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
        fputs(YOCTO_NAME YOCTO_VERSION, stderr);
        fputs("Copyright (C) 2012 Ayron Jungren", stderr);
        fputs("Licensed under the Apache License 2.0 <https://www.apache.org/licenses/LICENSE-2.0>", stderr);
        fputs(YOCTO_NAME " has NO WARRANTY, to the extent permitted by law.", stderr);
    }

    safe_malloc(globals, sizeof(globals_t));
    globals->config_path = argv[1];

    config = parse_config(globals->config_path);
    if(config == NULL)
        return EXIT_FAILURE;

    puts("Using these settings:");
    printf("  Port:            %u\n", config->port);
    printf("  Timeout:         %u\n", config->timeout);
    printf("  MOTD file:       %s\n", config->motd_path);
    printf("  Server address:  %s\n", config->server_address);
    printf("  Server name:     %s\n", config->server_name);
    if(config->chroot_folder != NULL)
        printf("  Chroot folder:   %s\n", config->chroot_folder);
    if(config->set_user != NULL)
        printf("  User to run as:  %s\n", config->set_user);
    if(config->set_group != NULL)
        printf("  Group to run as: %s\n", config->set_group);

    /* Before chrooting, we need a file handle for /dev/null because it won't be
     * reachable after the chroot. */
    globals->devnull = open("/dev/null", O_RDWR);
    if(globals->devnull < 0) {
        perror("Could not open /dev/null");
        exit(EXIT_FAILURE);
    }

    /* Activate all of the security features the user requested, and bail if any
     * of them fail (the functions themselves will call exit).
     *
     * The ordering is VERY important; get_* must be first, because after the
     * chroot the user database will be unavailable. set_* must be after
     * do_chroot, because dropping permissions will prevent chrooting.
     * set_group must be before set_user, because dropping user permissions will
     * prevent setgid from working. */
    if(config->set_user != NULL)
        uid = get_user(config->set_user);
    if(config->set_group != NULL)
        gid = get_group(config->set_group);
    if(config->chroot_folder != NULL)
        do_chroot(config->chroot_folder);
    if(config->set_group != NULL)
        set_group(gid);
    if(config->set_user != NULL)
        set_user(uid);

    globals->motd = parse_motd(config->motd_path);
    if(globals->motd == NULL)
        return EXIT_FAILURE;

    /* And now the fun begins... */
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(config->port);

    globals->socket = socket(AF_INET, SOCK_STREAM, 0);
    if(globals->socket < 0) {
        perror("Could not create socket");
        return EXIT_FAILURE;
    }

    if(bind(globals->socket, (struct sockaddr *) &server_addr, sizeof(struct sockaddr)) < 0) {
        perror("Could not bind socket");
        return EXIT_FAILURE;
    }

    /* So that it doesn't have to be created each time a child is created, we
     * pre-initialize sigalrm_action here. */
    globals->child_signal_action.sa_handler = child_signal_handler;
    globals->child_signal_action.sa_flags = 0;
    sigfillset(&globals->child_signal_action.sa_mask);

    /* Install a signal handler to handle SIGINT and SIGTERM cleanly. */
    parent_signal_action.sa_handler = parent_signal_handler;
    parent_signal_action.sa_flags = 0;
    sigfillset(&parent_signal_action.sa_mask);
    sigaction(SIGINT, &parent_signal_action, NULL);
    sigaction(SIGTERM, &parent_signal_action, NULL);
    sigaction(SIGUSR1, &parent_signal_action, NULL);
    sigaction(SIGUSR2, &parent_signal_action, NULL);

    puts("Daemonizing");
    daemonize();

    start_server(globals->socket);

    close(globals->socket);
    return EXIT_SUCCESS;
}

