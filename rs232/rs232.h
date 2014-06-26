#ifndef RS232_H
#define RS232_H

#include <termios.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

/* how many times befor giving up */
#define TIMEOUT 100

/* open one serial Port at PORTNAME with BOUDRATE bits/sec with input and
   output channel number returned */
int initrs232(const char * const PORTNAME,const speed_t BOUDRATE);
             
/* read an array of char terminated by '0' with maximum length of Length chars
   from serial input */
int readsrs232(char * const PTR_TO_CHAR,const int MAXLEN);

/* write an array of char terminated by '0' to serial output */
int writesrs232(const char * const STRING);

/* read a char from serial input */
int readcrs232(char * const PTR_TO_CHAR);

/* write a char to serial output */
int writecrs232(const char * const STRING);

/* close a serial io */
int closers232(void);

#endif
