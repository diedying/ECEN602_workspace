
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "readFast.h"

#define MAXDATASIZE 10

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXDATASIZE+1];

static ssize_t my_read(int fd, char *ptr)
{

    if (read_cnt <= 0) {
      again:
        if ((read_cnt = (int) read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return (-1);
        } else if (read_cnt == 0)
            return (0);
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return (1);
}

ssize_t readFast(int fd, void *vptr, size_t max_len)
{
    ssize_t n, rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < max_len; n++) {
        if ( (rc = my_read(fd, &c)) == 1) {
            *ptr++ = c;
            if (c  == '\n')
                break;          /* newline is stored, like fgets() */
        } else if (rc == 0) {
            *ptr = 0;
            return (n - 1);     /* EOF, n - 1 bytes were read */
        } else
            return (-1);        /* error, errno set by read() */
    }

    *ptr  = 0;                  /* null terminate like fgets() */
    return (n);
}


