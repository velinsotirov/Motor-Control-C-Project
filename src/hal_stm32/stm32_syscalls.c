#include <errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

/* Newlib stubs for bare-metal ARM development */

/* _exit is called when the program terminates */
void _exit(int status) {
    /* Hang in an infinite loop */
    while (1);
}

/* _sbrk is used for dynamic memory allocation (heap) */
caddr_t _sbrk(int incr) {
    extern char _end;
    static char *heap_end = 0;
    char *prev_heap_end;

    if (heap_end == 0) {
        heap_end = &_end;
    }
    prev_heap_end = heap_end;
    heap_end += incr;
    return (caddr_t)prev_heap_end;
}

/* _write is used by printf and other write functions */
int _write(int file, char *ptr, int len) {
    /* For now, do nothing - can be implemented for debugging output */
    return len;
}

/* _read is used by scanf and other read functions */
int _read(int file, char *ptr, int len) {
    /* For now, do nothing */
    return 0;
}

/* _close stub */
int _close(int file) {
    return -1;
}

/* _fstat stub */
int _fstat(int file, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

/* _isatty stub */
int _isatty(int file) {
    return 1;
}

/* _lseek stub */
int _lseek(int file, int ptr, int dir) {
    return 0;
}
