#ifndef SPINEL_VFS_H
#define SPINEL_VFS_H

#include <stdint.h>
#include <sys/types.h>

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

typedef ssize_t (*ReadCallback)(struct VNode*, uint8_t*, size_t);
typedef ssize_t (*WriteCallback)(struct VNode*, uint8_t*, size_t);
typedef void (*OpenCallback)(struct VNode*, FileFlags);
typedef void (*CloseCallback)(struct VNode*);
// Called when refCount hits zero and the vnode is removed from the VFS
typedef void (*DestroyCallback)(struct VNode*);

typedef struct VNode {
    ino_t inode;
    off_t size;
    uid_t userId;
    gid_t groupId;
    nlink_t numHardLinks;

    // We might have to know the device
    // For instance, if we want to open a child directory in a directory,
    // or if this directory is a mountpoint
    void* device;
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
    // So if the program "su" were to be looked at in Spinel, its permissions,
    // -u -rwx -r-x -r-x, would be encoded as 0x1755.

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

    ReadCallback read;
    WriteCallback write;
    OpenCallback open;
    CloseCallback close;
    DestroyCallback destroy;
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
