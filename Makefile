SOURCE_FILES = $(shell find src/ -name '*.c') $(shell find src/ -name '*.h')

CC     = cc
CFLAGS = -Werror -lsodium

all: build

build: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o locksmith $(SOURCE_FILES)

install: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o /usr/local/bin/locksmith $(SOURCE_FILES)
