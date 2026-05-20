# LiteDir Makefile for OpenBSD 7.0 / bmake

CC      = cc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic
CFLAGS += -Ilib
CFLAGS += -D_BSD_SOURCE
LDFLAGS =

TARGET  = ldctl
SRCS    = src/main.c src/db.c src/users.c src/groups.c src/auth.c
OBJS    = $(SRCS:.c=.o)

.if defined(DEBUG)
CFLAGS += -g -O0
.else
CFLAGS += -O2
.endif

${TARGET}: ${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LDFLAGS}

.c.o:
	${CC} ${CFLAGS} -c -o $@ $<

clean:
	rm -f ${OBJS} ${TARGET} ldstore.db

test: ${TARGET}
	${CC} ${CFLAGS} -o run_tests tests/tests_users.c \
		src/db.c src/users.c src/auth.c src/groups.c ${LDFLAGS} \
		&& ./run_tests
	rm -f run_tests ldstore.db

size: ${TARGET}
	size ${TARGET}

.PHONY: clean test size
