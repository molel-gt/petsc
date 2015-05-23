#if !defined(__PETSCVERSION_H)
#define __PETSCVERSION_H

#define PETSC_VERSION_RELEASE    0
#define PETSC_VERSION_MAJOR      3
#define PETSC_VERSION_MINOR      5
#define PETSC_VERSION_SUBMINOR   4
#define PETSC_VERSION_PATCH      0
#define PETSC_RELEASE_DATE       "Jun, 30, 2014"
#define PETSC_VERSION_DATE       "unknown"

#if !defined (PETSC_VERSION_GIT)
#define PETSC_VERSION_GIT        "unknown"
#endif

#if !defined(PETSC_VERSION_DATE_GIT)
#define PETSC_VERSION_DATE_GIT   "unknown"
#endif

#define PETSC_VERSION_(MAJOR,MINOR,SUBMINOR) \
  ((PETSC_VERSION_MAJOR == (MAJOR)) &&       \
   (PETSC_VERSION_MINOR == (MINOR)) &&       \
   (PETSC_VERSION_SUBMINOR == (SUBMINOR)) && \
   (PETSC_VERSION_RELEASE  == 1))

#define PETSC_VERSION_LT(MAJOR,MINOR,SUBMINOR)          \
  (PETSC_VERSION_RELEASE == 1 &&                        \
   (PETSC_VERSION_MAJOR < (MAJOR) ||                    \
    (PETSC_VERSION_MAJOR == (MAJOR) &&                  \
     (PETSC_VERSION_MINOR < (MINOR) ||                  \
      (PETSC_VERSION_MINOR == (MINOR) &&                \
       (PETSC_VERSION_SUBMINOR < (SUBMINOR)))))))

#define PETSC_VERSION_LE(MAJOR,MINOR,SUBMINOR) \
  (PETSC_VERSION_LT(MAJOR,MINOR,SUBMINOR) || \
   PETSC_VERSION_(MAJOR,MINOR,SUBMINOR))

#define PETSC_VERSION_GT(MAJOR,MINOR,SUBMINOR) \
  (0 == PETSC_VERSION_LE(MAJOR,MINOR,SUBMINOR))

#define PETSC_VERSION_GE(MAJOR,MINOR,SUBMINOR) \
  (0 == PETSC_VERSION_LT(MAJOR,MINOR,SUBMINOR))

#endif
