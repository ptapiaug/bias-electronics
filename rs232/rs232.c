#include "rs232.h"

static int fdin,fdout;

int initrs232 (const char * const portfile,const speed_t baud)
{
int ret;
struct termios tio;
FILE * fpin,fpout;

/*  Port fuer Eingabe oeffnen  */
    if (!(fdin=open(portfile,O_NOCTTY|O_RDONLY))) return (-1);
        if (fdin==-1) 
        {
            perror("open");
            return(-2);
        }
          
/*  Port fuer Ausgabe oeffnen  */
    if (!(fdout=open(portfile,O_NOCTTY|O_WRONLY))) return (-3);
        if (fdout==-1)
        {
            perror("open");
            close(fdin);
            return(-4);
        }

/*  Einstellungen fuer Eingabeport  */        
    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CLOCAL|CS7|PARODD|PARENB|CREAD|baud; /* 7O1 */
    tio.c_cflag=CLOCAL|CS8|CSTOPB|CREAD|baud; /* 8N2 */
    tio.c_cflag=CLOCAL|CS8|CREAD|baud; /* 8N1 */
    tio.c_lflag=0;
    cfsetispeed(&tio,baud);
    cfsetospeed(&tio,baud);
    if (tcsetattr(fdin,TCSANOW,&tio))
    {
        perror("tcsetattr");
        close(fdin);
        close(fdout);
        return(-5);
    }

/*  Einstellungen fuer Ausgabeport  */ 
/*    tio.c_iflag=0;
    tio.c_oflag=0;
    tio.c_cflag=CLOCAL|CS8|CREAD|baud;
    tio.c_lflag=0;
    cfsetispeed(&tio,baud);
    cfsetospeed(&tio,baud); */
    tio.c_cflag=CLOCAL|CS7|PARODD|PARENB|baud; /* 7O1 */
    tio.c_cflag=CLOCAL|CS8|CSTOPB|CREAD|baud; /* 8N2 */
    tio.c_cflag=CLOCAL|CS8|CREAD|baud; /* 8N1 */
    if (tcsetattr(fdout,TCSANOW,&tio))
    {
        perror("tcsetattr");
        close(fdin);
        close(fdout);
        return(-6);
    }
    tcflush(fdout,TCOFLUSH);
    tcflush(fdin,TCIFLUSH);
    return(0);
}

int writesrs232 (const char * const str)
{
int len=0;
int ret;
    if (str==NULL) return(-2);
    
/* laenge ermitteln */
    while (str[len]) len++;
    
    ret=write (fdout,str,len);
return(ret);
}

int readsrs232 (char * const str,const int maxchars)
{
int lauf=0;
int ret;
int chars=0;

if (maxchars==0) return (0);

/*  Read until enough or lf  */

    while (!(ret=read(fdin,str,1)));
    if (ret==-1) return (ret);
    while ( (chars<maxchars) && ((str[chars]) != '\r') && ( (ret>=0) || (errno==EAGAIN)) )
    {
        if (ret>0)
        {
            chars++;
        }
        while (!(ret=read(fdin,str+chars,1)));
    }
    if (ret==-1)
    {
        perror("read");
        return(ret);
    }
    str[chars]=0;
    return(chars);
}

int writecrs232 (const char * const str)
{
int len=0;
int ret;
    if (str==NULL) return(-2);
    ret=write (fdout,str,1);
return(ret);
}

int readcrs232 (char * const str)
{
int lauf=0;
int ret;
    ret=read(fdin,str,1);
    return (ret);
}

int closers232 (void)
{
    close(fdout);
    close(fdin);
    return(0);
}
