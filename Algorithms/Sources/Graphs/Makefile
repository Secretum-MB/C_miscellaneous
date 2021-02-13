#
# Make file for graphs library
#

CC = gcc
#using -DNDEBUG would deactivate all asserts() in optimized code
CFLAGS = -std=gnu99 -pedantic -Wall -Wno-trigraphs -O3
DBGFLAGS = -std=gnu99 -pedantic -Wall -Wno-trigraphs -Wsign-compare -Wwrite-strings -Wtype-limits -Wno-unused-function -ggdb3 -DDEBUG

HDR = headers#folder for header files.. dont need this when using make depend
BUILD = build#folder for objects and executables (can't get it working)
SRC = sources#folder for .c files (can't get it working)

LIB_DIR = -L /home/mathias/Documents/Projects/C_Programming/Libraries/
LIB = -l HashTable

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
DBGOBJS = $(SRCS:.c=.dbg.o)
DBGOBJS_PATH = $(patsubst %.o,$(BUILD)/%.o, $(DBGOBJS))#not currently used (see linking)

MAIN = Graph
DEBUG = Graph-debug


.PHONY: all launch clean depend 

# targets: instruction for linking object files
all: $(DEBUG)

#$(MAIN): $(OBJS)
#	gcc -o $@ -O3 $(OBJS)

$(DEBUG): $(DBGOBJS)
	$(CC) -O0 -o $@ -ggdb3 $(DBGOBJS) $(LIB_DIR) $(LIB)
#replacing last with above will update path but will make re-compile when not needed

# dependancies: instructions for compiling from .c
# building object files
%.dbg.o: %.c
	$(CC) $(DBGFLAGS) -c -o $@ $<
#adding $(BUILD) before $@ works but no use if linking dosent work well.

launch: $(DEBUG)
	./$(DEBUG)

# runs command below with "make clean"
clean:
	rm -f $(MAIN) $(DEBUG) *.o *.c~

# automates linking proper .h files
depend:
	makedepend $(SRCS)
	makedepend -a -o .dbg.o $(SRCS)
# DO NOT DELETE

breadthFirstSearch.o: /usr/include/stdlib.h /usr/include/stdio.h
breadthFirstSearch.o: ../../Headers/graph.h
breadthFirstSearch.o: /home/mathias/Documents/Projects/C_Programming/Libraries/hashTables.h
breadthFirstSearch.o: /usr/include/string.h
graph.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/string.h
graph.o: ../../Headers/graph.h

breadthFirstSearch.dbg.o: /usr/include/stdlib.h /usr/include/stdio.h
breadthFirstSearch.dbg.o: ../../Headers/graph.h
breadthFirstSearch.dbg.o: /home/mathias/Documents/Projects/C_Programming/Libraries/hashTables.h
breadthFirstSearch.dbg.o: /usr/include/string.h
graph.dbg.o: /usr/include/stdio.h /usr/include/stdlib.h /usr/include/string.h
graph.dbg.o: ../../Headers/graph.h
