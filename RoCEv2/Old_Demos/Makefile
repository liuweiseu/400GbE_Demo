CC = nvcc


TARGET_T = 400GbE_TDemo
TARGET_R = 400GbE_RDemo

SRC_T = 400GbE_Transmit_Demo.c
SRC_R = 400GbE_Receive_Demo.c

INC = packet.h

LIB = -libverbs -lcuda -arch=sm_86

all: ${TARGET_T} ${TARGET_R}

${TARGET_T}: ${SRC_T} ${INC}
	${CC} ${SRC_T} -o ${TARGET_T} ${LIB}

${TARGET_R}: ${SRC_R} ${INC}
	${CC} ${SRC_R} -o ${TARGET_R} ${LIB}

clean:
	rm ${TARGET_T}
	rm ${TARGET_R}