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

#ifndef __YOCTOIRCD_CONFIG_H
#define __YOCTOIRCD_CONFIG_H

/***** Compile-time Configuration *****/

/* Comment out this line if you don't want YoctoIRCd to daemonize after it loads
 * its configuration and MOTD file. */
#define YOCTO_DAEMONIZE

/* The starting size of the file buffer, and the amount is is increased by each
 * time it is resized up. */
#define YOCTO_FILE_BUF_SIZE 4096

/* The maximum size (in bytes) of the MOTD. If the MOTD is larger, it will be
 * truncated. It should be set to a multiple of YOCTO_FILE_BUF_SIZE. If it is
 * not, it will be rounded down to the next multiple of YOCTO_FILE_BUF_SIZE. */
#define YOCTO_MAX_MOTD_SIZE YOCTO_FILE_BUF_SIZE * 8

/* The maximum number of connections that can be in a "pending" state. In other
 * words, the number of connections YoctoIRCd can go without answering until new
 * connections are rejected until other connections have been answered. */
#define YOCTO_MAX_PENDING_CONNS 32

/* The size of the network buffer in bytes. */
#define YOCTO_NET_BUF_SIZE 4096

/***** Modules *****/

/* Comment out this line to disable the (extremely tiny) built-in HTTP server */
#define YOCTO_MOD_HTTP

/* Comment out this line to disable the IRC server (why would you want to do that?) */
#define YOCTO_MOD_IRC

#endif

