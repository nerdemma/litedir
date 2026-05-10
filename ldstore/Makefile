CC = cc
CFLAGS = -std=c99 -Wall -Wextra -pedantic
CFLAGS = -D_POSIX_C_SOURCE=200809L
CFLAGS = -Iinclude

TARGET = ldctl
SRCS = src/main.c src/db.c src/users.c src/groups.c src/auth.c
OBJS = ${SRCS:.c=.o }

.if defined(DEBUG)
CFLAGS += -g -O0
.else
CFLAGS += -O2
.endif

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ ${OBJS}

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ${OBJS} ${TARGET} ldstore.db

test: ${TARGET}
	${CC} ${CFLAGS} -o run_test test/test_users.c
	src/db.c src/users.c src/auth.c src/groups.c && ./run_tests

size: ${TARGET}
	size ${TARGET}

.PHONY: clean test size
