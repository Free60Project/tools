#ifndef __linux__
#include "mytypes.h"
#include "file64.h"
#include <stdio.h>
#include <fcntl.h>

#define	VERBOSE_DEBUG	0


fd64_t
open64(const char *path, int oflag,...)
{
	DWORD	desired_access = 0;
	DWORD	share_mode = 0;
	DWORD	create_disposition = 0;
	fd64_t	retval;

	if (oflag & O_CREAT) {
		if (oflag & O_TRUNC) {
			create_disposition = CREATE_ALWAYS;
		} else {
			create_disposition = CREATE_NEW;
		}
	} else {
		create_disposition = OPEN_EXISTING;
	}
	if (oflag & O_RDONLY) {
		desired_access = GENERIC_READ;
		share_mode = FILE_SHARE_READ;
	} else if (oflag & O_RDWR) {
		desired_access = GENERIC_READ | GENERIC_WRITE;
		share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}

	retval = CreateFile(path, desired_access, share_mode, (LPSECURITY_ATTRIBUTES)0,
		create_disposition, FILE_ATTRIBUTE_NORMAL, (HANDLE)0);


#if	VERBOSE_DEBUG
	fprintf(stderr, "open64('%s', 0x%x) = 0x%x\n", path, oflag, (unsigned int)(void *)retval);
#endif
	return retval;
}

int
close64(fd64_t fd)
{
	if (CloseHandle(fd)) {
		return 0;
	} else {
		return -1;
	}
}

off64_t
lseek64(fd64_t fd, off64_t offset, int whence)
{
	LARGE_INTEGER	li;
	LARGE_INTEGER	liret;
	DWORD	mm;
	off64_t	retval;

	li.QuadPart = offset;


	mm = FILE_BEGIN;
	if (whence == SEEK_SET) {
		mm = FILE_BEGIN;
	} else if (whence == SEEK_CUR) {
		mm = FILE_CURRENT;
	} else if (whence == SEEK_END) {
		mm = FILE_END;
	}


	if (SetFilePointerEx(fd, li, &liret, mm)) {
		retval = (off64_t)liret.QuadPart;
	} else {
		retval = (off64_t)-1L;
	}

#if	VERBOSE_DEBUG
	fprintf(stderr, "lseek64(fd=0x%x, offset=0x%08x%08x) = 0x%08x%08x\n", 
		(unsigned int)(void *)fd,
		(unsigned int)li.HighPart, (unsigned int)li.LowPart,
		(unsigned int)liret.HighPart, (unsigned int)liret.LowPart);
#endif

	return retval;
}


size64_t
read64(fd64_t fd, void *buf, size64_t count)
{
	DWORD	actual;
	size64_t	retval;

	if (ReadFile(fd, (LPVOID)buf, (DWORD)count, &actual, (LPOVERLAPPED)0)) {
		retval = (size64_t)actual;
	} else {
		retval = (size64_t)-1;
	}

#if	VERBOSE_DEBUG
	fprintf(stderr, "read64(fd=0x%x, count=0x%08x) = 0x%08x\n",
		(unsigned int)(void *)fd,
		(unsigned int)count, (unsigned int)actual);
#endif

	return retval;
}

size64_t
write64(fd64_t fd, void *buf, size64_t count)
{
	DWORD	actual;
	size64_t	retval;

	if (WriteFile(fd, (LPCVOID)buf, (DWORD)count, &actual, (LPOVERLAPPED)0)) {
		retval = (size64_t)actual;
	} else {
		retval =(size64_t)-1;
	}

#if	VERBOSE_DEBUG
	fprintf(stderr, "write64(fd=0x%x, count=0x%08x) = 0x%08x\n",
		(unsigned int)(void *)fd,
		(unsigned int)count, (unsigned int)actual);
#endif

	return retval;
}
#endif // !__linux__

