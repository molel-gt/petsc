/* $Id: petscda.h,v 1.54 2000/05/10 16:44:25 bsmith Exp bsmith $ */

/*
      Regular array object, for easy parallelism of simple grid 
   problems on regular distributed arrays.
*/
#if !defined(__PETSCDA_H)
#define __PETSCDA_H
#include "petscvec.h"
#include "petscao.h"

#define DA_COOKIE PETSC_COOKIE+14

typedef struct _p_DA* DA;
typedef enum { DA_STENCIL_STAR,DA_STENCIL_BOX } DAStencilType;
typedef enum { DA_NONPERIODIC,DA_XPERIODIC,DA_YPERIODIC,DA_XYPERIODIC,
               DA_XYZPERIODIC,DA_XZPERIODIC,DA_YZPERIODIC,DA_ZPERIODIC} 
               DAPeriodicType;
typedef enum { DA_X,DA_Y,DA_Z } DADirection;

EXTERN int   DACreate1d(MPI_Comm,DAPeriodicType,int,int,int,int*,DA *);
EXTERN int   DACreate2d(MPI_Comm,DAPeriodicType,DAStencilType,int,int,int,int,int,int,int*,int*,DA *);
EXTERN int   DACreate3d(MPI_Comm,DAPeriodicType,DAStencilType,
                        int,int,int,int,int,int,int,int,int *,int *,int *,DA *);
EXTERN int   DADestroy(DA);
EXTERN int   DAView(DA,Viewer);

EXTERN int   DAPrintHelp(DA);

EXTERN int   DAGlobalToLocalBegin(DA,Vec,InsertMode,Vec);
EXTERN int   DAGlobalToLocalEnd(DA,Vec,InsertMode,Vec);
EXTERN int   DAGlobalToNaturalBegin(DA,Vec,InsertMode,Vec);
EXTERN int   DAGlobalToNaturalEnd(DA,Vec,InsertMode,Vec);
EXTERN int   DANaturalToGlobalBegin(DA,Vec,InsertMode,Vec);
EXTERN int   DANaturalToGlobalEnd(DA,Vec,InsertMode,Vec);
EXTERN int   DALocalToLocalBegin(DA,Vec,InsertMode,Vec);
EXTERN int   DALocalToLocalEnd(DA,Vec,InsertMode,Vec);
EXTERN int   DALocalToGlobal(DA,Vec,InsertMode,Vec);
EXTERN int   DAGetOwnershipRange(DA,int **,int **,int **);
EXTERN int   DACreateGlobalVector(DA,Vec *);
EXTERN int   DACreateNaturalVector(DA,Vec *);
EXTERN int   DACreateLocalVector(DA,Vec *);
EXTERN int   DALoad(Viewer,int,int,int,DA *);
EXTERN int   DAGetCorners(DA,int*,int*,int*,int*,int*,int*);
EXTERN int   DAGetGhostCorners(DA,int*,int*,int*,int*,int*,int*);
EXTERN int   DAGetInfo(DA,int*,int*,int*,int*,int*,int*,int*,int*,int*,DAPeriodicType*,DAStencilType*);
EXTERN int   DAGetProcessorSubset(DA,DADirection,int,MPI_Comm*);
EXTERN int   DARefine(DA,DA*);

EXTERN int   DAGlobalToNaturalAllCreate(DA,VecScatter*);
EXTERN int   DANaturalAllToGlobalCreate(DA,VecScatter*);

EXTERN int   DAGetGlobalIndices(DA,int*,int**);
EXTERN int   DAGetISLocalToGlobalMapping(DA,ISLocalToGlobalMapping*);

EXTERN int   DAGetScatter(DA,VecScatter*,VecScatter*,VecScatter*);

EXTERN int   DAGetAO(DA,AO*);
EXTERN int   DASetCoordinates(DA,Vec); 
EXTERN int   DAGetCoordinates(DA,Vec *);
EXTERN int   DASetUniformCoordinates(DA,double,double,double,double,double,double);
EXTERN int   DASetFieldName(DA,int,const char[]);
EXTERN int   DAGetFieldName(DA,int,char **);

EXTERN int   DAVecGetArray(DA,Vec,void **);
EXTERN int   DAVecRestoreArray(DA,Vec,void **);

#include "petscmat.h"
EXTERN int   DAGetColoring(DA,ISColoring *,Mat *);
EXTERN int   DAGetInterpolation(DA,DA,Mat*,Vec*);

#include "petscpf.h"
EXTERN int DACreatePF(DA,PF*);

/*
   The VecPack routines allow one to manage a nonlinear solver that works on a vector that consists
  of several distinct parts. This is mostly used for LNKS solvers, that is design optimization problems 
  that are written as a nonlinear system
*/
typedef struct _p_VecPack *VecPack;

EXTERN int VecPackCreate(MPI_Comm,VecPack*);
EXTERN int VecPackDestroy(VecPack);
EXTERN int VecPackAddArray(VecPack,int);
EXTERN int VecPackAddDA(VecPack,DA);
EXTERN int VecPackAddVecScatter(VecPack,VecScatter);
EXTERN int VecPackScatter(VecPack,Vec,...);
EXTERN int VecPackGather(VecPack,Vec,...);
EXTERN int VecPackCreateGlobalVector(VecPack,Vec*);
EXTERN int VecPackGetGlobalIndices(VecPack,...);

#include "petscsnes.h"

/*
     Data structure to easily manage multi-level linear solvers on regular grids managed by DA
*/
typedef struct _p_DAMG *DAMG;
struct _p_DAMG {
  DA         da;                    /* grid information for this level */
  Vec        x,b,r;                 /* global vectors used in multigrid preconditioner for this level*/
  Mat        J;                     /* matrix on this level */
  Mat        R;                     /* restriction to next coarser level (not defined on level 0) */
  int        ratiox,ratioy,ratioz;  /* grid spacing to next level finer level, usually 2 */
  int        nlevels;               /* number of levels above this one (total number of levels on level 0) */
  MPI_Comm   comm;
  int        (*solve)(DAMG*,int);
  void       *user;         

  /* SLES only */
  SLES       sles;             
  int        (*rhs)(DAMG,Vec);

  /* SNES only */
  Mat           B;
  Vec           Rscale;                /* scaling to restriction before computing Jacobian */
  Vec           localX,localF;         /* ghosted work vectors */
  int           (*computejacobian)(SNES,Vec,Mat*,Mat*,MatStructure*,void*);  
  int           (*computefunction)(SNES,Vec,Vec,void*);  
  MatFDColoring fdcoloring;            /* only used with finite difference coloring for Jacobian */  
  SNES          snes;                  
  int           Xsize;
};
EXTERN int DAMGCreate(MPI_Comm,int,void*,DAMG**);
EXTERN int DAMGDestroy(DAMG*);
EXTERN int DAMGSetCoarseDA(DAMG*,DA);
EXTERN int DAMGSetSLES(DAMG*,int (*)(DAMG,Vec),int (*)(DAMG,Mat));
EXTERN int DAMGSetSNES(DAMG*,int (*)(SNES,Vec,Vec,void*),int (*)(SNES,Vec,Mat*,Mat*,MatStructure*,void*));
EXTERN int DAMGView(DAMG*,Viewer);
EXTERN int DAMGSetUpLevel(DAMG*,SLES,int);
EXTERN int DAMGSolve(DAMG*);

#define DAMGGetb(ctx) (ctx)[(ctx)[0]->nlevels-1]->b
#define DAMGGetx(ctx) (ctx)[(ctx)[0]->nlevels-1]->x
#define DAMGGetJ(ctx) (ctx)[(ctx)[0]->nlevels-1]->J
#define DAMGGetB(ctx) (ctx)[(ctx)[0]->nlevels-1]->B
#define DAMGGetFine(ctx) (ctx)[(ctx)[0]->nlevels-1]
#define DAMGGetSLES(ctx) (ctx)[(ctx)[0]->nlevels-1]->sles
#define DAMGGetSNES(ctx) (ctx)[(ctx)[0]->nlevels-1]->snes

#endif


