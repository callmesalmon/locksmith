SOURCE_FILES = $(shell find src/ -name '*.c') $(shell find src/ -name '*.h')

CC     = cc
CFLAGS = -Werror -lsodium

install: $(SOURCE_FILES)
	$(CC) $(CFLAGS) -o locksmith $(SOURCE_FILES)
