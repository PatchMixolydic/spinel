#include <errno.h>

static char __ErrorMessages[][36] = {
//  "12345678901234567890123456789012345"
    "No recorded error",

    "Argument out of domain", // EDOM
    "Illegal character sequence", // EILSEQ
    "Result out of range", // ERANGE

    "Argument list too big", // E2BIG
    "Access denied", // EACCES
    "Address in use", // EADDRINUSE
    "Address not available", // EADDRNOTAVAIL
    "Address family not supported", // EAFNOSUPPORT
    "Resource temporarily unavailable", // EAGAIN, EWOULDBLOCK
    "Operation already in progress", // EALREADY
    "Bad file", // EBADF
    "Bad message", // EBADMSG
    "Resource busy", // EBUSY, ETXTBUSY
    "Operation cancelled", // ECANCELED
    "No children", // ECHILD
    "Connection aborted", // ECONNABORTED
    "Connection refused", // ECONNREFUSED
    "Connection reset", // ECONNRESET
    "Deadlock would occur", // EDEADLK
    "Destination address is required", // EDESTADDRREQ
    "Disk quota exceeded", // EDQUOT
    "Item already exists", // EEXIST
    "Bad Address!!", // EFAULT
    "File too large", // EFBIG
    "Host unreachable", // EHOSTUNREACH
    "Identifier removed", // EIDRM
    "Operation is in progress", // EINPROGRESS
    "Interrupted syscall", // EINTR
    "Invalid argument", // EINVAL
    "I/O error", // EIO
    "Socket is connected", // EISCONN
    "Is a directory", // EISDIR
    "Too many layers of symlinks", // ELOOP
    "Too many files", // EMFILE
    "Too many links", // EMLINK
    "Message too large", // EMSGSIZE
    "Multihop attempted", // EMULTIHOP
    "Filename too long", // ENAMETOOLONG
    "Network is down", // ENETDOWN
    "Network connection was reset", // ENETRESET
    "Network unreachable", // ENETUNREACH
    "Too many open files", // ENFILE
    "No buffer space available", // ENOBUFS
    "No data", // ENODATA
    "No such device", // ENODEV
    "No such file or directory", // ENOENT
    "Bad executable", // ENOEXEC
    "No locks available", // ENOLCK
    "Link was severed", // ENOLINK
    "Out of memory", // ENOMEM
    "No message of the desired type", // ENOMSG
    "Protocol not available", // ENOPROTOOPT
    "Not enough space on device", // ENOSPC
    "No STREAM resources available", // ENOSR
    "Not a STREAM", // ENOSTR
    "Syscall not implemented", // ENOSYS
    "Socket not connected", // ENOTCONN
    "Not a directory or a symlink to one", // ENOTDIR
    "Directory not empty", // ENOTEMPTY
    "Unrecoverable error", // ENOTRECOVERABLE
    "Not a socket", // ENOTSOCK
    "Operation not supported", // ENOTSUP, EOPTNOTSUPP
    "Invalid I/O operation for device", // ENOTTY
    "No such device or address", // ENXIO
    "Value too large for data type", // EOVERFLOW
    "Owner is dead", // EOWNERDEAD
    "No permission", // EPERM
    "Pipe broken, call plumber", // EPIPE
    "Protocol error", // EPROTO
    "Protocol not supported", // EPROTONOSUPPORT
    "Wrong protocol type for socket", // EPROTOTYPE
    "Read-only filesystem", // EROFS
    "Bad seek for pipe", // ESPIPE
    "Search for process failed", // ESRCH
    "Stale file", // ESTALE
    "Stream I/O control timeout", // ETIME
    "Connection timed out", // ETIMEDOUT
    "Bad cross-device link", // EXDEV

    "No such filesystem", // ENOFS
};

static char __UnknownError[] = "Unknown error";

char* strerror(int error) {
    if (error < 0 || error > sizeof(__ErrorMessages) / sizeof(char*)) {
        errno = EINVAL;
        return __UnknownError;
    } else {
        return __ErrorMessages[error];
    }
}
