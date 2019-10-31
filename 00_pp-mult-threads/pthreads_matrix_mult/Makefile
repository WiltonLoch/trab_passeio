# GNU Makefile

CC = gcc 
CCFLAGS = -Wall
LDFLAGS = -pthread
TARGET = pthreads_matrix_mult 

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

%: %.o
	$(CC) $(LDFLAGS) $^ -o $@ 

all: $(TARGET)

# Dependencias

pthreads_matrix_mult: matrix.o pthreads_matrix_mult.c
matrix.o: matrix.c matrix.h

clean:
	rm -f *.o *~ $(TARGET)
