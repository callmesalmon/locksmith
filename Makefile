SOURCE_FILES = $(shell find src/ -name '*.c') $(shell find src/ -name '*.h')
TARGET = locksmith

CC     = cc
CFLAGS = -Werror -Wall -Wextra -Wno-unused-variable -lsodium

all: build

build: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE_FILES)

install: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o /usr/local/bin/$(TARGET) $(SOURCE_FILES)
