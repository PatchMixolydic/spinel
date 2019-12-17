#include <stdio.h>
#include <stdlib.h>
#include <kernel/panic.h>

__attribute__((__noreturn__))
void abort() {
	#ifdef __is_libk
		panic("abort() was called");
	#else
		// TODO: Abnormally terminate the process as if by SIGABRT.
		printf("abort()\n");
	#endif
	while (1) { }
	__builtin_unreachable();
}
