CC ?= gcc
CFLAGS ?= -DNDEBUG -Os -pedantic -pipe -std=c99 -Wall

OBJS = src/child.o src/config_parser.o src/daemonize.o src/io.o src/memory.o src/motd_parser.o src/parent.o src/security.o src/signal_handlers.o src/split_lines.o src/main.o

.PHONY: all clean debug help release

all: release
debug: yoctoircd

help:
	@echo "Available targets:"
	@echo "    \"debug\" - Builds YoctoIRCd in debug mode"
	@echo "    \"release\" - Builds YoctoIRCd in release mode"
	@echo "    \"help\" - Prints this help and exits"
	@echo
	@echo "The default target (\"all\") builds YoctoIRCd in release mode."
	@echo
	@echo "The C compiler that will be used is \"$(CC)\""
	@echo "The arguments that will be used are \"$(CFLAGS)\""

release: yoctoircd
	@echo "  STRIP   $<"
	@strip -s $<

yoctoircd: $(OBJS)
	@echo "  LD      $@"
	@$(CC) $(CFLAGS) -o $@ $(OBJS)

.c.o: src/yoctoircd.h
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "  RM      yoctoircd src/*.o"
	@rm -f yoctoircd src/*.o

