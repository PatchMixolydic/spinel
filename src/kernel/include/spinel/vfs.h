#ifndef SPINEL_VFS_H
#define SPINEL_VFS_H

#include <stdint.h>

#define VFSFilenameLength 256

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

typedef size_t (*ReadCallback)(struct VNode*, uint8_t*, size_t);
typedef size_t (*WriteCallback)(struct VNode*, uint8_t*, size_t);
typedef void (*OpenCallback)(struct VNode*, FileFlags);
typedef void (*CloseCallback)(struct VNode*);

typedef struct VNode {
    char name[VFSFilenameLength];
    // 4 EiB ought to be enough for anybody :p
    // TODO: types
    uint64_t size;
    // Likewise, 4 million users ought to be enough
    uint32_t userId;
    uint32_t groupId;
    uint64_t inode;

    // We might have to know the device
    // For instance, if this is a directory and we want to look at its children
    void* device;

    FileFlags flags;
    FileType type;
    FilePermissions permissions;

    uint64_t lastAccessedTime;
    uint64_t lastModifiedTime;
    uint64_t createdTime;

    uint32_t refCount;

    ReadCallback readCallback;
    WriteCallback writeCallback;
    OpenCallback openCallback;
    CloseCallback closeCallback;
} VNode;

void initVFS(void);
void vfsEmplace(const char parentDir[], VNode* vnode);

#endif // ndef SPINEL_VFS_H
