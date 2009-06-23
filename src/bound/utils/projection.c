#include "taosolver.h"

#undef __FUNCT__  
#define __FUNCT__ "VecBoundProjection"
PetscErrorCode TAOSOLVER_DLLEXPORT VecBoundGradientProjection(Vec G, Vec X, Vec XL, Vec XU, Vec GP){

  PetscErrorCode ierr;
  PetscInt n,i;
  PetscReal *xptr,*xlptr,*xuptr,*gptr,*gpptr;
  PetscReal xval,gpval;

  /* Project variables at the lower and upper bound */

  PetscFunctionBegin;
  PetscValidHeaderSpecific(G,VEC_COOKIE,1);
  PetscValidHeaderSpecific(X,VEC_COOKIE,2);
  PetscValidHeaderSpecific(XL,VEC_COOKIE,3);
  PetscValidHeaderSpecific(XU,VEC_COOKIE,4);
  PetscValidHeaderSpecific(GP,VEC_COOKIE,5);

  ierr = VecGetLocalSize(X,&n); CHKERRQ(ierr);

  ierr=VecGetArray(X,&xptr); CHKERRQ(ierr);
  ierr=VecGetArray(XL,&xlptr); CHKERRQ(ierr);
  ierr=VecGetArray(XU,&xuptr); CHKERRQ(ierr);
  ierr=VecGetArray(G,&gptr); CHKERRQ(ierr);
  if (G!=GP){
    ierr=VecGetArray(GP,&gpptr); CHKERRQ(ierr);
  } else { gpptr=gptr; }

  for (i=0; i<n; ++i){
    gpval = gptr[i]; xval = xptr[i]; 

    if (gpval>0 && xval<=xlptr[i]){
      gpval = 0;
    } else if (gpval<0 && xval>=xuptr[i]){
      gpval = 0;
    }
    gpptr[i] = gpval;
  }

  ierr=VecRestoreArray(X,&xptr); CHKERRQ(ierr);
  ierr=VecRestoreArray(XL,&xlptr); CHKERRQ(ierr);
  ierr=VecRestoreArray(XU,&xuptr); CHKERRQ(ierr);
  ierr=VecRestoreArray(G,&gptr); CHKERRQ(ierr);
  if (G!=GP){
    ierr=VecRestoreArray(GP,&gpptr); CHKERRQ(ierr);
  }
  PetscFunctionReturn(0);
}

#undef __FUNCT__
#define __FUNCT__ "VecStepMax2"
PetscErrorCode TAOSOLVER_DLLEXPORT VecStepMaxBounded(Vec X, Vec DX, Vec XL, Vec XU, PetscReal *stepmax){

  PetscErrorCode ierr;
  PetscInt i,nn;
  PetscReal *xx,*dx,*xl,*xu;
  PetscReal localmax=0;

  PetscFunctionBegin;
  PetscValidHeaderSpecific(X,VEC_COOKIE,2);
  PetscValidHeaderSpecific(DX,VEC_COOKIE,5);
  PetscValidHeaderSpecific(XL,VEC_COOKIE,3);
  PetscValidHeaderSpecific(XU,VEC_COOKIE,4);

  ierr = VecGetArray(X,&xx);CHKERRQ(ierr);
  ierr = VecGetArray(XL,&xl);CHKERRQ(ierr);
  ierr = VecGetArray(XU,&xu);CHKERRQ(ierr);
  ierr = VecGetArray(DX,&dx);CHKERRQ(ierr);
  ierr = VecGetLocalSize(X,&nn);CHKERRQ(ierr);

  for (i=0;i<nn;i++){
    if (dx[i] > 0){
      localmax=PetscMax(localmax,(xu[i]-xx[i])/dx[i]);      
    } else if (dx[i]<0){ 
      localmax=PetscMax(localmax,(xl[i]-xx[i])/dx[i]);
    }
  }
  ierr = VecRestoreArray(X,&xx);CHKERRQ(ierr);
  ierr = VecRestoreArray(XL,&xl);CHKERRQ(ierr);
  ierr = VecRestoreArray(XU,&xu);CHKERRQ(ierr);
  ierr = VecRestoreArray(DX,&dx);CHKERRQ(ierr);

  ierr = MPI_Allreduce(&localmax,stepmax,1,MPIU_REAL,MPI_MAX,((PetscObject)X)->comm);
  CHKERRQ(ierr);


  PetscFunctionReturn(0);
}
#undef __FUNCT__
#define __FUNCT__ "VecBoundStepInfo"
PetscErrorCode TAOSOLVER_DLLEXPORT VecStepBoundInfo(Vec X, Vec XL, Vec XU, Vec DX, PetscReal *boundmin, PetscReal *wolfemin, PetscReal *boundmax){

  PetscErrorCode ierr;
  PetscInt n,i;
  PetscReal *x,*xl,*xu,*dx;
  PetscReal t;
  PetscReal localmin=1.0e300,localwolfemin=1.0e300,localmax=0;
  MPI_Comm comm;
  
  PetscFunctionBegin;
  PetscValidHeaderSpecific(X,VEC_COOKIE,2);
  PetscValidHeaderSpecific(XL,VEC_COOKIE,3);
  PetscValidHeaderSpecific(XU,VEC_COOKIE,4);
  PetscValidHeaderSpecific(DX,VEC_COOKIE,5);

  ierr=VecGetArray(X,&x);CHKERRQ(ierr);
  ierr=VecGetArray(XL,&xl);CHKERRQ(ierr);
  ierr=VecGetArray(XU,&xu);CHKERRQ(ierr);
  ierr=VecGetArray(DX,&dx);CHKERRQ(ierr);
  ierr = VecGetLocalSize(X,&n);CHKERRQ(ierr);
  for (i=0;i<n;i++){
    if (dx[i]>0){
      t=(xu[i]-x[i])/dx[i];
      localmin=PetscMin(t,localmin);
      if (localmin>0){
	  localwolfemin = PetscMin(t,localwolfemin);
      }
      localmax = PetscMax(t,localmax);
    } else if (dx[i]<0){
      t=(xl[i]-x[i])/dx[i];
      localmin = PetscMin(t,localmin);
      if (localmin>0){
	localwolfemin = PetscMin(t,localwolfemin);
      }
      localmax = PetscMax(t,localmax);
    }
  }
  ierr=VecRestoreArray(X,&x);CHKERRQ(ierr);
  ierr=VecRestoreArray(XL,&xl);CHKERRQ(ierr);
  ierr=VecRestoreArray(XU,&xu);CHKERRQ(ierr);
  ierr=VecRestoreArray(DX,&dx);CHKERRQ(ierr);
  ierr=PetscObjectGetComm((PetscObject)X,&comm);CHKERRQ(ierr);
  
  if (boundmin){ ierr = MPI_Allreduce(&localmin,boundmin,1,MPIU_REAL,MPI_MIN,comm);CHKERRQ(ierr);}
  if (wolfemin){ ierr = MPI_Allreduce(&localwolfemin,wolfemin,1,MPIU_REAL,MPI_MIN,comm);CHKERRQ(ierr);}
  if (boundmax) { ierr = MPI_Allreduce(&localmax,boundmax,1,MPIU_REAL,MPI_MAX,comm);CHKERRQ(ierr);}

  ierr = PetscInfo3(X,"Step Bound Info: Closest Bound: %6.4e, Wolfe: %6.4e, Max: %6.4e \n",*boundmin,*wolfemin,*boundmax); CHKERRQ(ierr);
  PetscFunctionReturn(0);  
}

