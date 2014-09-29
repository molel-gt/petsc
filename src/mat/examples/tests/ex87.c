
static char help[] = "Tests MatGetSubMatrices() for SBAIJ matrices\n\n";

#include <petscmat.h>

#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc,char **args)
{
  Mat            BAIJ,SBAIJ,*subBAIJ,*subSBAIJ;
  PetscViewer    viewer;
  char           file[PETSC_MAX_PATH_LEN];
  PetscBool      flg;
  PetscErrorCode ierr;
  PetscInt       n = 2,issize;
  PetscMPIInt    rank;
  IS             isrow,iscol,irow[2],icol[2];

  PetscInitialize(&argc,&args,(char*)0,help);
  ierr = PetscOptionsGetString(NULL,"-f",file,PETSC_MAX_PATH_LEN,&flg);CHKERRQ(ierr);
  ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,file,FILE_MODE_READ,&viewer);CHKERRQ(ierr);

  ierr = MatCreate(PETSC_COMM_WORLD,&BAIJ);CHKERRQ(ierr);
  ierr = MatSetType(BAIJ,MATMPIBAIJ);CHKERRQ(ierr);
  ierr = MatLoad(BAIJ,viewer);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(&viewer);CHKERRQ(ierr);

  ierr = PetscViewerBinaryOpen(PETSC_COMM_WORLD,file,FILE_MODE_READ,&viewer);CHKERRQ(ierr);
  ierr = MatCreate(PETSC_COMM_WORLD,&SBAIJ);CHKERRQ(ierr);
  ierr = MatSetType(SBAIJ,MATMPISBAIJ);CHKERRQ(ierr);
  ierr = MatLoad(SBAIJ,viewer);CHKERRQ(ierr);
  ierr = PetscViewerDestroy(&viewer);CHKERRQ(ierr);

  ierr   = MatGetSize(BAIJ,&issize,0);CHKERRQ(ierr);
  issize = 9;
  ierr   = ISCreateStride(PETSC_COMM_SELF,issize,0,1,&isrow);CHKERRQ(ierr);
  irow[0] = irow[1] = isrow;
  issize = 9;
  ierr   = ISCreateStride(PETSC_COMM_SELF,issize,0,1,&iscol);CHKERRQ(ierr);
  icol[0] = icol[1] = iscol;
  ierr   = MatGetSubMatrices(BAIJ,n,irow,icol,MAT_INITIAL_MATRIX,&subBAIJ);CHKERRQ(ierr);
  ierr   = MatGetSubMatrices(SBAIJ,n,irow,icol,MAT_INITIAL_MATRIX,&subSBAIJ);CHKERRQ(ierr);

  ierr = MPI_Comm_rank(PETSC_COMM_WORLD,&rank);CHKERRQ(ierr);
  if (!rank) {
    ierr = MatView(subBAIJ[0],PETSC_VIEWER_STDOUT_SELF);CHKERRQ(ierr);
    ierr = MatView(subSBAIJ[0],PETSC_VIEWER_STDOUT_SELF);CHKERRQ(ierr);
  }

  /* Free data structures */
  ierr = ISDestroy(&isrow);CHKERRQ(ierr);
  ierr = ISDestroy(&iscol);CHKERRQ(ierr);
  ierr = MatDestroyMatrices(n,&subBAIJ);CHKERRQ(ierr);
  ierr = MatDestroyMatrices(n,&subSBAIJ);CHKERRQ(ierr);
  ierr = MatDestroy(&BAIJ);CHKERRQ(ierr);
  ierr = MatDestroy(&SBAIJ);CHKERRQ(ierr);

  ierr = PetscFinalize();
  return 0;
}


