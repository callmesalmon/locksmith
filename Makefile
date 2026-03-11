SOURCE_FILES = $(shell find src/ -name '*.c') $(shell find src/ -name '*.h')
TARGET = locksmith

MAN_FILE = man/locksmith.1

ifeq (,$(shell which $(CC)))
$(error "$(CC) is not installed. Cannot compile.")
endif

CFLAGS = -std=c99 -Werror -Wall -Wextra -lsodium

all: build

build: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE_FILES)

install: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o /usr/local/bin/$(TARGET) $(SOURCE_FILES)
