#ifdef PETSC_RCS_HEADER
"$Id: petscconf.h,v 1.22 1999/11/24 21:52:17 bsmith Exp $"
"Defines the configuration for this machine"
#endif

#if !defined(INCLUDED_PETSCCONF_H)
#define INCLUDED_PETSCCONF_H

#define PARCH_beos
#define PETSC_ARCH_NAME "beos"

#define PETSC_HAVE_SYS_WAIT_H
#define PETSC_HAVE_VPRINTF
#define PETSC_RETSIGTYPE void
#define PETSC_STDC_HEADERS
#define PETSC_SIZEOF_VOIDP 4
#define PETSC_SIZEOF_INT 4
#define PETSC_SIZEOF_DOUBLE 8
#define PETSC_HAVE_DRAND48
#define PETSC_HAVE_GETCWD
#define PETSC_HAVE_GETHOSTNAME
#define PETSC_HAVE_GETTIMEOFDAY
#define PETSC_HAVE_MEMMOVE
#define PETSC_HAVE_RAND
#define PETSC_HAVE_READLINK
#define PETSC_HAVE_SIGACTION
#define PETSC_HAVE_SIGNAL
#define PETSC_HAVE_SOCKET
#define PETSC_HAVE_STRSTR
#define PETSC_HAVE_UNAME
#define PETSC_HAVE_FCNTL_H
#define PETSC_HAVE_LIMITS_H
#define PETSC_HAVE_MALLOC_H
#define PETSC_HAVE_PWD_H 
#define PETSC_HAVE_STDLIB_H
#define PETSC_HAVE_STRING_H
#define PETSC_HAVE_SYS_RESOURCE_H
#define PETSC_HAVE_SYS_TIME_H 
#define PETSC_HAVE_UNISTD_H


#define PETSC_HAVE_FORTRAN_UNDERSCORE 
#define PETSC_HAVE_FORTRAN_UNDERSCORE_UNDERSCORE

#define PETSC_HAVE_DOUBLE_ALIGN_MALLOC
#define PETSC_CANNOT_START_DEBUGGER
#define PETSC_HAVE_NO_GETRUSAGE

#endif
