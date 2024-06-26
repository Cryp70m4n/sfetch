# Makefile for sfetch

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -lX11 -DDEFAULT_RESOURCE_PATH=\"$(HOME)/.local/share/sfetch/resources\"

# Directories
DESTDIR = /usr/bin
RESOURCE_PATH = $(HOME)/.local/share/sfetch/resources

# Files to compile
SRCS = src/SystemInformation.c src/main.c
OBJS = $(SRCS:.c=.o)
TARGET = sfetch

# Default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Rule for compiling .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Install target
install: $(TARGET)
	install -d $(DESTDIR)
	install -m 755 $(TARGET) $(DESTDIR)
	install -d $(RESOURCE_PATH)
	cp -r resources $(RESOURCE_PATH)

# Clean target
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all install clean

