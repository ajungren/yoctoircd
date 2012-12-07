# YoctoIRCd

**YoctoIRCd** is an extremely minimalistic IRC server with only one purpose:
send the MOTD to clients that connect to it and disconnect them immediately. Its
intended use is for servers that listen on SSL-only; YoctoIRCd can provide a
(non-SSL) message informing users trying to connect on the default port that
they'll have to use a different port with SSL enabled. It also makes a good
fallback server that can relay messages about downtime, etc. while the main IRC
server is down. It also has an extremely simple (read: non-standard-compliant)
HTTP server that just sends the MOTD and disconnects, as well. The HTTP server
can be disabled, in which case it won't be compiled at all.

## Requirements

YoctoIRCd requires a C compiler to build, and nothing but the C standard library
and a POSIX environment to run. YoctoIRCd officially supports FreeBSD and Linux,
but should run in any other POSIX-ish environment. Windows is *not* supported.

## Compiling

To compile YoctoIRCd, download it and run `make` in the folder where you have
the source. By default, `yoctoircd` will be built in release mode. See the
"Debugging" section for information about creating a debug build.

For advanced users, you can edit some compile-time configuration in `config.h`.
This is where you can disable the HTTP server as well as changing buffer sizes
and tweak how other internals work.

If for whatever reason you do not have `make` on the system where you're trying
to compile YoctoIRCd, you can compile it with something similar to `cc -Os -o
yoctoircd src/*.c` in the folder where the source is.

## Configuring

A sample configuration for YoctoIRCd is provided as `yoctoircd.cfg`. You should
copy it with a different name (so it doesn't get overwritten on updates) before
editing it. The sample configuration file is documented, but for convenience,
here are the configuration options available:

- `Port`: The TCP port to listen on
- `Timeout`: The maximum amount of time a client can be connected (in seconds)
- `MotdPath`: The path to the file containing the MOTD
- `ServerAddress`: The address of the server (no spaces)
- `ServerName`: The nickname of the server
- `ChrootFolder`: The folder to `chroot` into (*optional*, requires root)
- `SetUser`: The user to `setuid` to (*optional*, requires root)
- `SetGroup`: The group to `setgid` to (*optional*, requires root)

**NOTE**: The configuration parser is quite strict; comments (beginning with
`#`) *must* start at the beginning of the line and any unrecognized option will
cause an error.

## Running

Just run `./yoctoircd $PATH_TO_CONFIG`, replacing `$PATH_TO_CONFIG` with the
path to the configuration file you created.

## Updating the MOTD

You can update the MOTD without stopping YoctoIRCd. To do so, update the MOTD
file that your configuration file points to, then send a `SIGUSR1` signal to
YoctoIRCd. To do so, you can use the one-liner `killall -USR1 yoctoircd`. The
child processes will ignore `SIGUSR1`, but the parent process will reload the
MOTD when it recieves the signal.

## Updating the Configuration File

You can update the configuration file without stopping YoctoIRCd, just like
updating the MOTD. Instead of sending `SIGUSR1`, send `SIGUSR2` in the exact
same manner.

**NOTE**: You cannot update the port that YoctoIRCd listens on or any security
options (`ChrootFolder`, `SetUser`, and `SetGroup`) with `SIGUSR2`. You *must*
restart YoctoIRCd. You can, however, change the path to the MOTD file. YoctoIRCd
will update the MOTD automatically, as well.

## Security

Though I've done my best to eliminate bugs, it's impossible to get rid of all of
them. That being said, it's possible that some of the bugs may cause security
flaws. I've been extra careful to avoid those kinds of bugs, but it's still best
to err on the side of caution.

YoctoIRCd supports `chroot`, `setuid`, and `setgid` through configuration
options. For the best security, you should create a user and group just for
YoctoIRCd, create a folder with the configuration file and the MOTD (and nothing
else), set the folder and files read-only for YoctoIRCd, then set `ChrootFolder`
to that folder, and `SetUser` and `SetGroup` to that user and group. For those
options to work, however, you will need to run `yoctoircd` as root. If you do
so, it will drop to the specified user and group as soon as possible. If you
have set those security options, but don't run `yoctoircd` as root, it will
cause an error and refuse to run because it will be unable to use `chroot`,
`setuid`, and `setgid`.

## How It Works

YoctoIRCd forks a child process for each connection. Each child process sets
an alarm to go off after the timeout has been reached, which will close the
connection and kill the process. Because the child process does not write to
that much memory, the forking should be fairly inexpensive on COW systems, such
as Linux. Even on systems that don't have COW forking, YoctoIRCd's memory usage
is low enough that forking shouldn't have an excessively large overhead. A
threaded mode may be added in the future.

## Debugging

To produce a debug build, first set your `CFLAGS` to enable debug information.
For example, if you're using `bash` and `gcc`, run `export CFLAGS="-ggdb -O0"`
then `make debug`. It's important to both add debug flags to `CFLAGS` as well as
using the `debug` target instead of the default `release` target. The `release`
target runs `strip` over the built binary which strips all the debug information
that the C compiler added. Also, setting your `CFLAGS` will remove the
`-DNDEBUG` from the default `CFLAGS`, which will enable assertions.

## License

As noted in the source files, YoctoIRCd is licensed under the [Apache License,
Version 2.0](http://www.apache.org/licenses/LICENSE-2.0). A copy of the license
is also available in the source distribution, named `LICENSE.md`. `LICENSE.md`
is the plain-text version ported to Markdown, using the HTML version as a
guide.

## Donate

If you use YoctoIRCd and find it useful, it would be greatly appreciated if you
donated to the project. Donations are primarily accepted through
[Flattr](http://flattr.com/thing/1048781/) although other arrangements can be
made by [contacting me through GitHub](https://github.com/dririan) if you don't
want to use Flattr.

[![Flattr this](http://api.flattr.com/button/flattr-badge-large.png)](http://flattr.com/thing/1048781/)

