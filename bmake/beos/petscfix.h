/*$Id: petscfix.h,v 1.100 2000/01/11 20:57:57 bsmith Exp $*/

/*
    This fixes various things in system files that are incomplete, for 
  instance many systems don't properly prototype all system functions.
  It is not intended to DUPLICATE anything in the system include files;
  if the compiler reports a conflict between a prototye in a system file
  and this file then the prototype in this file should be removed.

    This is included by files in src/sys/src
*/

#if !defined(_PETSCFIX_H)
#define _PETSCFIX_H

#include "petsc.h"

/* -----------------------beos ------------------------------------------*/
#if defined(__cplusplus)
extern "C" {
}
#endif
#endif
