# Get CROSS_COMPILE value from user
CROSS_COMPILE ?= 

# Compiler and flags
CC = $(CROSS_COMPILE)gcc
CFLAGS = -g -Wall -Werror

# Target binary
TARGET = aesdsocket

# Source files and object files
SRC = $(TARGET).c
OBJ = $(SRC:.c=.o)

# Default
all: $(TARGET)
default: $(TARGET)

# Build executable from object files
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $^

# Clean
clean:
	rm -f *.o $(TARGET) *.elf *.map

# Phony targets
.PHONY: 
	all clean
