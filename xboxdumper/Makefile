
OBJS=main.o util.o fatx.o file64.o utility.o

CFLAGS=-O2 -DXBOX360=1 -DMEMCARD=0 -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D__USE_LARGEFILE64

all: $(OBJS)
	gcc -o xboxdumper $(OBJS)

clean:
	rm -fr $(OBJS) xboxdumper xboxdumper.exe *# *~

.PHONY: clean
