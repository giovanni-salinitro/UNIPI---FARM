.PHONY: all test clean cleanall

CC = gcc
CFLAGS = -g -Wall -std=c99 -I./utils
OPTFLAGS = -O3
LIBS = -pthread
LDFLAGS = -L./utils
SRCS = main.c check_arguments.c ConcurrentQueue.c Collector.c Master.c Result.c Workers.c
OBJS = $(SRCS:.c=.o)

.PRECIOUS: farm generafile
.INTERMEDIATE: $(OBJS)

all: farm generafile

farm: $(OBJS)
	$(CC) $(CFLAGS) $(OPTFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

generafile: generafile.c
	$(CC) $(CFLAGS) $(OPTFLAGS) $(LDFLAGS) -o $@ $< $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

test: 
	./test.sh

clean:
	rm -f *.o

cleanall: clean
	rm -f farm generafile
