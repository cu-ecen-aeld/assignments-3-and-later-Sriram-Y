# Get CROSS_COMPILE value from user
CROSS_COMPILE ?= 

# Compiler and flags
CC = $(CROSS_COMPILE)gcc
CFLAGS = -g -Wall -Werror
LDFLAGS = -I/

# Target binary
TARGET = writer

# Source files and object files
SRC = $(TARGET).c
OBJ = $(SRC:.c=.o)

# Default
all: $(TARGET)

# Build executable from object files
$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJ)

# Compile .c files into .o files
%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f *.o $(TARGET) *.elf *.map

# Phony targets
.PHONY: 
	all clean
