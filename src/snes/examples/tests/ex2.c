#ifndef lint
static char vcid[] = "$Id: ex2.c,v 1.2 1995/04/04 19:51:02 bsmith Exp bsmith $";
#endif

static char help[] = "Uses Newton method to solve a two variable system\n";


#include "snes.h"

int  FormJacobian(Vec,Mat*,void*),
     FormResidual(Vec,Vec,void*),
     FormInitialGuess(Vec,void*),
     Monitor(SNES,int, Vec,Vec,double,void *);

int main( int argc, char **argv )
{
  SNES         snes;
  SLES         sles;
  SNESMETHOD   method = SNES_NLS;  /* nonlinear solution method */
  Vec          x,r;
  Mat          J;
  int          ierr, its; 

  PetscInitialize( &argc, &argv, 0,0 );

  ierr = VecCreateSequential(2,&x); CHKERRA(ierr);
  ierr = VecCreate(x,&r); CHKERRA(ierr);
  ierr = MatCreateSequentialDense(2,2,&J); CHKERRA(ierr);

  ierr = SNESCreate(&snes); CHKERRA(ierr);
  ierr = SNESSetMethod(snes,method); CHKERRA(ierr);
  ierr = SNESSetMonitor(snes,Monitor,0);
  ierr = SNESSetFromOptions(snes); CHKERR(ierr);

  /* Set various routines */
  SNESSetSolution( snes, x,FormInitialGuess,0 );
  SNESSetResidual( snes, r,FormResidual,0, 0 );
  SNESSetJacobian( snes, J, FormJacobian,0 );	

  SNESGetSLES(snes,&sles);
  SLESSetFromOptions(sles);

  SNESSetUp( snes );				       

  /* Execute solution method */
  ierr = SNESSolve( snes,&its );				       
  printf( "number of Newton iterations = %d\n\n", its );

  VecDestroy(x);
  VecDestroy(r);
  MatDestroy(J);
  SNESDestroy( snes );				       
  PetscFinalize();

  return 0;
}
/* ------------------------------------------------ */
/*
    Evaluate residual F(x).
 */

int FormResidual(Vec x,Vec  f,void *dummy )
{
   Scalar *xx, *ff;
   VecGetArray(x,&xx); VecGetArray(f,&ff);
   ff[0] = xx[0]*xx[0] + xx[0]*xx[1] - 3.0;
   ff[1] = xx[0]*xx[1] + xx[1]*xx[1] - 6.0;
   return 0;
}
/* ------------------------------------------------ */
/*
    Form initial approximation.
 */
int FormInitialGuess(Vec x,void *dummy)
{
   Scalar pfive = .50;
   VecSet(&pfive,x);
   return 0;
}
/* ------------------------------------------------ */
/*
   Evaluate Jacobian matrix F'(x).
 */
int FormJacobian(Vec x,Mat *jac,void *dummy)
{
  Scalar *xx, A[4];
  int    idx[2] = {0,1};
  VecGetArray(x,&xx);
  A[0] = 2.0*xx[0] + xx[1]; A[1] = xx[0];
  A[2] = xx[1]; A[3] = xx[0] + 2.0*xx[1];
  MatSetValues(*jac,2,idx,2,idx,A,InsertValues);
  return 0;
}

int Monitor(SNES snes,int its, Vec x,Vec f,double fnorm,void *dummy)
{
  fprintf( stdout, "iter = %d, residual norm %g \n",its,fnorm);
  VecView(x,STDOUT_VIEWER);
  return 0;
}
