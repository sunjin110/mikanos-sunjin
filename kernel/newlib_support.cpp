#include <sys/types.h>
#include <errno.h>

extern "C" caddr_t sbrk(int incr) {
    errno = ENOMEM;
    return (caddr_t)-1;
}
