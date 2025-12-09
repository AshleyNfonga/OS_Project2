# Compiler settings
CC=gcc
CFLAGS=-I. -pthread -std=c99

# Dependencies
DEPS = BENSCHILLIBOWL.h
OBJ = BENSCHILLIBOWL.o main.o 

# Compile object files
%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

# Link main executable
main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Optional: targets for Project 1 (shm_processes)
shm_proc: shm_processes.c
	$(CC) shm_processes.c -D_SVID_SOURCE -pthread -std=c99 -lpthread -o shm_proc

example: example.c
	$(CC) example.c -pthread -std=c99 -lpthread -o example
