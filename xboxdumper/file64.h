#ifndef FILE64_H
#define FILE64_H

#ifndef MYTYPES_H
#include "mytypes.h"
#endif

#ifdef	__linux__

typedef	int	fd64_t;
//typedef _off64_t	off64_t;
typedef int	size64_t;

#define	open64	open
#define	close64	close
#define	read64	read
#define	write64	write

#define	STDIN_FD64	fileno(stdin)
#define	STDOUT_FD64	fileno(stdout)
#define	STDERR_FD64	fileno(stderr)

#define	IS_INVALID_FD64(f)	((f) < 0)

#else // !__linux__

typedef	u_int64_t	off64_t;
typedef	HANDLE	fd64_t;
typedef	int		size64_t;

#define	INVALID_FD64	((fd64_t)INVALID_HANDLE_VALUE)
#define	IS_INVALID_FD64(f)	((f) == INVALID_FD64)

#define	STDIN_FD64	((fd64_t)GetStdHandle(STD_INPUT_HANDLE))
#define	STDOUT_FD64	((fd64_t)GetStdHandle(STD_OUTPUT_HANDLE))
#define	STDERR_FD64	((fd64_t)GetStdHandle(STD_ERROR_HANDLE))

extern	off64_t lseek64(fd64_t fd, off64_t offset, int whence); 
extern	fd64_t open64(const char *path, int oflag,...); 

extern	size64_t	read64(fd64_t fd, void *buf, size64_t count);
extern	size64_t	write64(fd64_t fd, void *buf, size64_t count);

extern	int	close64(fd64_t fd);

#endif // __linux__


#endif // FILE64_H



