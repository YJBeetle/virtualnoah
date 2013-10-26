ifeq ($(origin CC),default)
CC = gcc
endif

ALL_SOURCES=*.[ch] hashtable/*.[ch]

CFLAGS := $(CFLAGS) -Wall -O2 -I./lzo/include -I../../../include

LDFLAGS := $(LDFLAGS) -lz -llzo2 -lm -L./lzo/lib

TARGETS = mkfs.ubifs

all: $(TARGETS)

mkfs.ubifs: crc16.o crc32.o lpt.o compr.o hashtable/hashtable.o \
            hashtable/hashtable_itr.o devtable.o

clean:
	rm -f *.o $(TARGETS) cscope.*

cscope:
	@echo $(ALL_SOURCES) > cscope.files
	@cscope -bR
	@rm cscope.files
