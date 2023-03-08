CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic
LDFLAGS =
SRCS = C_files_to_header.c bin2c_bank.c pal2c.c sc5_to_pal.c
PROGS = $(SRCS:%.c=%.elf)

.PHONY: all clean

all: $(PROGS)

%.elf: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f $(PROGS)
