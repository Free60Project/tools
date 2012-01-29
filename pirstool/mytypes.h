#ifndef MYTYPES_H
#define MYTYPES_H

#include <sys/types.h>

#ifdef __linux__
#include <unistd.h>
#else // !__linux__
#include <windows.h>
#ifndef __CYGWIN__
typedef unsigned __int16	u_int16_t;
typedef unsigned __int32	u_int32_t;
typedef unsigned __int64	u_int64_t;
#endif // __CYGWIN__
#endif // __linux__

#if	__GCC__ && __BYTE_ORDER == __BIG_ENDIAN
#define	bswap32(v)	(v)
#define	bswap16(v)	(v)
#else // little endian
#define bswap32(v) (((v >> 24) & 0xff) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) | ((v << 24) & 0xff000000))
#define bswap16(v) (((v >> 8) & 0xff) | ((v << 8) & 0xff00))
#endif

#endif // MYTYPES_H



