all: streamrs232 writers232 writers232hex writebias dumprs232

clean:
	rm -f streamrs232 writers232 writers232hex writebias dumprs232 rs232.o

dumprs232 : dumprs232.c rs232.o makefile talk_listen.h
	cc -g -ansi -pipe -o dumprs232 dumprs232.c rs232.o

streamrs232 : streamrs232.c rs232.o makefile talk_listen.h
	cc -g -ansi -pipe -o streamrs232 streamrs232.c rs232.o

writers232 : writers232.c rs232.o makefile talk_listen.h
	cc -g -ansi -pipe -o writers232 writers232.c rs232.o

writers232hex : writers232hex.c rs232.o makefile talk_listen.h
	cc -g -ansi -pipe -o writers232hex writers232hex.c rs232.o

writebias : writebias.c rs232.o makefile talk_listen.h
	cc -g -ansi -pipe -o writebias writebias.c rs232.o

rs232.o : rs232.c rs232.h makefile
	gcc rs232.c -c -g -ansi -pipe
