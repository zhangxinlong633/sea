SHELL=/bin/sh
# Where you want it installed when you do 'make install'
PREFIX=/usr/local/

CFLAGS = -I/usr/local/include #-O3 -march=native -mfpmath=sse

LDFLAGS += -L/usr/local/lib


TEST_LDFLAGS += -L/usr/local/lib  -l sea_block

all: libblock block_test 

LIB_OBJS = sea_block_index.o sea_block_data.o sea_block_record.o sea_block_offset.o sea_block.o utils.o

TEST_OBJS = test.o

libblock: $(LIB_OBJS)
	${CC} ${CFLAGS} ${LIB_OBJS} -shared -o libsea_block.so ${LDFLAGS} 

block_test: $(TEST_OBJS)
	${CC} ${CFLAGS} ${TEST_OBJS} -o block_test ${TEST_LDFLAGS} 

distclean: clean
clean:
	rm -f *.o *.core libsea_block.so block_test

