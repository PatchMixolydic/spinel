#ifndef SPINEL_VFS_H
#define SPINEL_VFS_H

#include <dirent.h>
#include <stdint.h>
#include <sys/types.h>

// Length of file*system* names, not filenames
#define FSNameLength 64

typedef enum {
    FileRead = 1,
    FileWrite = 2,
    FileReadWrite = FileRead | FileWrite,
    FileAppend = 4,
    FileTruncate = 8,
    FileCreateIfNeeded = 16,
    FileOnlyIfNonexistant = 32,
    FileIgnoreRefCount = 64
} FileFlags;

typedef enum {
    // Filesystems currently can't be mounted unreadable,
    // so a "writable" flag should suffice
    // (without it, the filesystem is read-only)
    FSWritable = 1,
    FSNoExecute = 2,
    FSNoAccessTime = 4,
    // Only updates access time if either of the
    // file modified or inode modified times are ahead
    FSRelativeAccessTime = 8,
} FileSystemFlags;

typedef enum {
    FileNormal,
    FileDirectory,
    FileBlockDev,
    // FileStreamDev
} FileType;

typedef enum {
    FileCanExecute = 1,
    FileCanWrite = 2,
    FileCanRead = 4,
    FileCanMove = 8,
    FileGroupCanExecute = FileCanExecute << 4,
    FileGroupCanWrite = FileCanWrite << 4,
    FileGroupCanRead = FileCanRead << 4,
    FileGroupCanMove = FileCanMove << 4,
    FileOwnerCanExecute = FileCanExecute << 8,
    FileOwnerCanWrite = FileCanWrite << 8,
    FileOwnerCanRead = FileCanRead << 8,
    FileOwnerCanMove = FileCanMove << 8,
} FilePermissions;

struct VNode;

// The callbacks tend to return:
// 0 if the operation is successful and has no data to report
// A positive integer if the operation is successful and has a size to report
// -EFAULT if a pointer argument is NULL when it shouldn't be
// -EIO if an I/O operation fails
// -ENOTDIR if the requested file should be a directory, but isn't
// -EISDIR if the requested file shouldn't be a directory, but is
// -EEXIST if the requested file exists, but it shouldn't
// -EACCES if the caller doesn't have permission to open this file
// -ENOENT if the file doesn't exist and it shouldn't be created
// -ENOTEMPTY if the requested directory has contents which render the
//            operation invalid
// -ELOOP if the request requires traversal of too many symlinks
// -ENOTSUP if the requested operation is not supported
// -ENOSPC if the requested operation is a write, but there isn't enough space
// -EROFS if the requested operation is a write, but the mounted filesystem
//        isn't writable
// -ENOLINK if the requested symlink is unresolvable
// -ENOEXEC if an executable is run, but the filesystem is mounted as
//          no execute

// Note that specific errors should be translated into the above errors for
// ease of handling. For instance, on a filesystem backed by the network,
// if the ethernet cord is yanked, FSOpenCallback might recieve -ENETUNREACH
// when trying to access a file. In this case, it should return -EIO, not
// -ENETUNREACH.

typedef ssize_t (*VNodeReadCallback)(struct VNode*, uint8_t*, size_t);
typedef ssize_t (*VNodeWriteCallback)(struct VNode*, uint8_t*, size_t);
typedef void (*VNodeOpenCallback)(struct VNode*, FileFlags);
typedef void (*VNodeCloseCallback)(struct VNode*);
// Called when refCount hits zero and the vnode is removed from the VFS
typedef void (*VNodeDestroyCallback)(struct VNode*);

// These paths are relative to /, not the device root
// Open a file with a given path, placing a pointer to the result in res
// (res must be heap allocated)
typedef int (*FSOpenCallback)(char* path, FileFlags, struct VNode** res);
// Create a hard link to this VNode at the specified path
// If the file doesn't exist on this filesystem, it is created as specified by
// the VNode
// Otherwise, the file's records are updated on the filesystem to create
// a new hard link using the VNode's fsINode
typedef int (*FSLinkCallback)(struct VNode*, char* path);
// Remove the hardlink at this path
typedef int (*FSUnlinkCallback)(char* path);

typedef struct {
    char name[FSNameLength];
    FSOpenCallback open;
    FSLinkCallback link;
    FSUnlinkCallback unlink;
} FSInfo;

typedef struct VMount {
    // TODO: constant
    char devicePath[4096];
    struct VNode* deviceVNode;
    char mountpointPath[4096];
    struct VNode* rootVNode;
    FileSystemFlags flags;
    FSInfo* filesystem;
} VMount;

typedef struct VNode {
    // The inode that can be use to index the VFS inode list
    ino_t vfsINode;
    off_t size;
    uid_t userId;
    gid_t groupId;
    nlink_t numHardLinks;

    // We might have to know the device
    // For instance, if we want to open a child directory in a directory,
    // or if this directory is a mountpoint
    VMount* device;
    // Filesystems with conflicting inodes may be mounted,
    // so this is provided to store the inode on the filesystem, if needed.
    // The inode for the VNode is probably different and is used for indexing
    // the VFS.
    ino_t fsINode;

    FileFlags flags;
    FileType type;

    // Spinel's permissions are different from most Unix-like systems
    // Most Unix-like systems use an octal quadruplet, representing:
    // the sticky bit, sgid bit, suid bit, user rwx, group rwx, and world rwx
    // Spinel instead uses a hex quadruplet: the sticky bit is now set
    // individually for user, group, and world, as the most significant bit.
    // So, then, -rwx -r-x tr-x would appear as 0x75D. The most significant hex
    // digit holds the sgid and suid bits, in that order. Note that Spinel
    // currently does not use the suid and sgid bits; these are just
    // provided for compatibility with filesystems that store them.
    // So if the program "su" from a Linux installation was
    // to be looked at in Spinel, its permissions, -u -rwx -r-x -r-x,
    // would be encoded as 0x1755.

    // Note that in filesystems that can only store one sticky bit (ie. most of
    // them), Spinel will place the sticky bit on the group and world nybbles.
    // If the filesystem supports some way to add extensions without breaking
    // compatibility (ex. compatible feature flags or extended attribute lists,
    // ext{2..4} is an example of both), however, Spinel will use those to
    // store the sticky bits (and the permissions field will have the sticky
    // bit set, if possible).
    mode_t permissions;

    time_t lastAccessedTime;
    time_t lastModifiedTime;
    time_t createdTime;

    uint32_t refCount;

    VNodeReadCallback read;
    VNodeWriteCallback write;
    VNodeOpenCallback open;
    VNodeCloseCallback close;
    VNodeDestroyCallback destroy;
} VNode;

void initVFS(void);
// Place a VNode into the vfs
// Returns the inode, or 0 on failure
ino_t vfsEmplace(VNode* vnode);
VNode* vfsOpen(ino_t inode, FileFlags flags);
// Returns 0 on success, or -EFAULT if inode is not found
int vfsClose(ino_t inode);
void vfsDestroy(VNode* vnode);
ssize_t vfsRead(ino_t inode, void* buf, size_t size);
ssize_t vfsWrite(ino_t inode, void* buf, size_t size);

#endif // ndef SPINEL_VFS_H
