CC = cc
CFLAGS = -std=c99 -Wall -Wextra -pedantic -D_POSIX_C_SOURCE=200809L -Iinclude

TARGET = ldctl
SRCS = src/main.c src/db.c src/users.c src/groups.c src/auth.c
OBJS = ${SRCS:.c=.o}

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS += -g -O0
else
	CFLAGS += -O2
endif

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ ${OBJS}

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f ${OBJS} ${TARGET} ldstore.db run_test

test: ${TARGET}
	$(CC) $(CFLAGS) -o run_test test/test_users.c src/db.c src/users.c src/auth.c src/groups.c && ./run_test

size: ${TARGET}
	size ${TARGET}

.PHONY: clean test size
