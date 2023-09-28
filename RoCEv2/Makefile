CC = gcc

TARGET = 400GbE_TDemo
SRC = 400GbE_Transmit_Demo.c
INC = packet.h
LIB = -libverbs

all: ${TARGET}

${TARGET}: ${SRC} ${INC}
	${CC} ${SRC} -o ${TARGET} ${LIB}

clean:
	rm ${TARGET}