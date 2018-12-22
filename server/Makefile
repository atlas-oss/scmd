CC=gcc

WARN= -Wall -Wextra -Wno-unused-parameter -Wno-deprecated-declarations -Wformat-security -Wformat -Werror=format-security
SEC= -march=native -fstack-protector-all --param ssp-buffer-size=4 -fpie -ftrapv -D_FORTIFY_SOURCE=2

CFLAGS= ${SEC} ${WARN} -std=c99 -O2 -DVERBOSE
LDFLAGS= -Wl,-z,relro,-z,now -pie -ldl

SRC = main.c mod.c def.c network.c
OBJECT = main.o mod.o def.o network.o

all: _scmd_
	@echo CC -o scmd_serv
	@${CC} ${OBJECT} -o scmd_serv ${LDFLAGS}

_scmd_:
	@echo CC ${SRC}
	@${CC} -c ${CFLAGS} ${SRC}

.PHONY: clean install uninstall

clean:
	@echo cleaning
	@rm -rf scmd_serv *.o *~

install: all
	@echo installing executable file to /usr/bin
	@mkdir -p /usr/bin
	@cp -f scmd_serv /usr/bin
	@chmod 755 /usr/bin/scmd_serv

uninstall:
	@echo removing executable file from /usr/bin
	@rm -f /usr/bin/scmd_serv
