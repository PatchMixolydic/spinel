#ifndef _LIBC_ERRNO_H
#define _LIBC_ERRNO_H

extern unsigned errno;

enum {
    // C standard
    EDOM = 1, // Argument out of domain
    EILSEQ, // Illegal character sequence (go to jail)
    ERANGE, // Result out of range

    // POSIX
    E2BIG, // Argument list too big
    EACCES, // Access denied
    EADDRINUSE, // Address in use
    EADDRNOTAVAIL, // Address not available
    EAFNOSUPPORT, // Address family not supported
    EAGAIN, // Resource temporarily unavailable
    EALREADY, // Operation already in progress
    EBADF, // Bad file
    EBADMSG, // Bad message
    EBUSY, // Resource busy
    ECANCELED, // Operation cancelled
    ECHILD, // No children
    ECONNABORTED, // Connection aborted
    ECONNREFUSED, // Connection refused
    ECONNRESET, // Connection reset
    EDEADLK, // Deadlock would occur
    EDESTADDRREQ, // Destination address is required
    EDQUOT, // Disk quota exceeded (POSIX says "reserved"?)
    EEXIST, // Already exists
    EFAULT, // Bad Address!!
    EFBIG, // File too large
    EHOSTUNREACH, // Host unreachable
    EIDRM, // Identifier removed
    EINPROGRESS, // Operation is in progress
    EINTR, // Interrupted syscall
    EINVAL, // Invalid argument
    EIO, // I/O error
    EISCONN, // Socket is connected
    EISDIR, // Is a directory
    ELOOP, // Too many layers of symlinks
    EMFILE, // Too many files
    EMLINK, // Too many links
    EMSGSIZE, // Message too large
    EMULTIHOP, // Multihop attempted
    ENAMETOOLONG, // Filename too long
    ENETDOWN, // Network is down
    ENETRESET, // Network connection was reset
    ENETUNREACH, // Network unreachable
    ENFILE, // Too many open files
    ENOBUFS, // No buffer space available
    ENODATA, // No data
    ENODEV, // No such device
    ENOENT, // No such file or directory
    ENOEXEC, // Bad executable
    ENOLCK, // No locks available
    ENOLINK, // Link was severed
    ENOMEM, // Out of memory
    ENOMSG, // No message of the desired type
    ENOPROTOOPT, // Protocol not available
    ENOSPC, // No space remaining on device
    ENOSR, // No STREAM resources available
    ENOSTR, // Not a STREAM
    ENOSYS, // Function not implemented
    ENOTCONN, // Socket not connected
    ENOTDIR, // Not a directory or a symlink to a directory
    ENOTEMPTY, // Directory not empty
    ENOTRECOVERABLE, // Not recoverable
    ENOTSOCK, // Not a socket
    ENOTSUP, // Operation not supported
    ENOTTY, // Invalid I/O control operation for device
    ENXIO, // No such device or address
    EOVERFLOW, // Value too large for data type
    EOWNERDEAD, // Owner is dead
    EPERM, // No permission
    EPIPE, // Pipe broke, call plumber
    EPROTO, // Protocol error
    EPROTONOSUPPORT, // Protocol not supported
    EPROTOTYPE, // Wrong type for socket given protocol
    EROFS, // Read-only filesystem
    ESPIPE, // Bad seek for pipe
    ESRCH, // Search for process failed
    ESTALE, // Stale file
    ETIME, // Stream I/O control timeout(?)
    ETIMEDOUT, // Connection timed out
    EXDEV, // Bad cross-device link

    // Spinel

    // Aliases
    EOPTNOTSUPP = ENOTSUP,
    ETXTBUSY = EBUSY, // Text file busy???
    EWOULDBLOCK = EAGAIN, // Operation would block
};

#endif // ndef _LIBC_ERRNO_H
