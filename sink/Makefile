CC   = gcc
INC  = -I../../lib
LIBS =
CFLAGS = -ggdb $(INC) 

SRCS= \
	main.c 

OBJS=$(SRCS:.c=.o)
LIBS=-L../../lib -lpopup -lpthread

TARGET=log-sink

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBS)

dep:
	gccmakedep $(INC) $(SRCS)

clean:
	rm -rf $(OBJS) $(TARGET) core

install:
	cp $(TARGET) ../../bin
