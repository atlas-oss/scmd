CC=gcc

WARN= -Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -Wformat-security -Wformat -Werror=format-security -Wstack-protector
SEC= -march=native -fstack-protector-all --param ssp-buffer-size=4 -fpie -ftrapv -D_FORTIFY_SOURCE=2

CFLAGS= ${SEC} ${WARN} -std=c99 -pedantic -O2
LDFLAGS= -Wl,-z,relro,-z,now -pie

SRC = main.c network.c
OBJECT = main.c network.c

all: _swm_
	@echo CC -o scmd
	@${CC} ${OBJECT} -o scmd ${LDFLAGS}

_swm_:
	@echo CC ${SRC}
	@${CC} -c ${CFLAGS} ${SRC}

.PHONY: clean install uninstall

clean:
	@echo cleaning
	@rm -rf scmd *.o *~

install: all
	@echo installing executable file to /usr/bin
	@mkdir -p /usr/bin
	@cp -f scmd /usr/bin
	@chmod 755 /usr/bin/scmd

uninstall:
	@echo removing executable file from /usr/bin
	@rm -f /usr/bin/scmd
