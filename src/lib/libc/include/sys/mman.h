#ifndef _LIBC_SYS_MMAN_H
#define _LIBC_SYS_MMAN_H

#define PROT_NONE 0
#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4

#define MAP_SHARED 0
#define MAP_PRIVATE 1
#define MAP_ANONYMOUS 2
#define MAP_ANON MAP_ANONYMOUS

#define MAP_FAILED ((void*)-1)

#endif // ndef _LIBC_SYS_MMAN_H
