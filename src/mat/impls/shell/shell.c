#ifndef lint
static char vcid[] = "$Id: shell.c,v 1.31 1996/04/03 16:33:31 curfman Exp curfman $";
#endif

/*
   This provides a simple shell for Fortran (and C programmers) to 
  create a very simple matrix class for use with KSP without coding 
  much of anything.
*/

#include "petsc.h"
#include "matimpl.h"        /*I "mat.h" I*/
#include "vec/vecimpl.h"  

typedef struct {
  int  m, n;                       /* rows, columns */
  int  (*destroy)(Mat);
  void *ctx;
} Mat_Shell;      

/*@
    MatShellGetContext - Returns the user-provided context associated with a shell matrix.

    Input Parameter:
.   mat - the matrix, should have been created with MatCreateShell()

    Output Parameter:
.   ctx - the user provided context

    Notes:
    This routine is intended for use within various shell matrix routines,
    as set with MatShellSetOperation().
    
.keywords: matrix, shell, get, context

.seealso: MatCreateShell(), MatShellSetOperation()
@*/
int MatShellGetContext(Mat mat,void **ctx)
{
  PetscValidHeaderSpecific(mat,MAT_COOKIE); 
  if (mat->type != MATSHELL) *ctx = 0; 
  else                       *ctx = ((Mat_Shell *) (mat->data))->ctx; 
  return 0;
}

static int MatGetSize_Shell(Mat mat,int *m,int *n)
{
  Mat_Shell *shell = (Mat_Shell *) mat->data;
  *m = shell->m; *n = shell->n;
  return 0;
}

static int MatDestroy_Shell(PetscObject obj)
{
  int       ierr;
  Mat       mat = (Mat) obj;
  Mat_Shell *shell;

  shell = (Mat_Shell *) mat->data;
  if (shell->destroy) {ierr = (*shell->destroy)(mat);CHKERRQ(ierr);}
  PetscFree(shell); 
  PLogObjectDestroy(mat);
  PetscHeaderDestroy(mat);
  return 0;
}
  
static struct _MatOps MatOps = {0,0,
       0, 
       0,0,0,0,
       0,0,0,0,
       0,0,
       0,
       0,
       0,0,0,
       0,
       0,0,0,
       0,0,
       0,
       0,0,0,0,
       0,0,MatGetSize_Shell,
       0,0,0,0,
       0,0,0,0 };

/*@C
   MatCreateShell - Creates a new matrix class for use with a user-defined
   private data storage format. 

   Input Parameters:
.  comm - MPI communicator
.  m - number of rows
.  n - number of columns
.  ctx - pointer to data needed by the shell matrix routines

   Output Parameter:
.  A - the matrix

   Notes:
   The shell matrix type is intended to provide a simple class to use
   with KSP (such as, for use with matrix-free methods). You should not
   use the shell type if you plan to define a complete matrix class.

   Usage:
$   MatCreateShell(m,n,ctx,&mat);
$   MatShellSetOperation(mat,MAT_MULT,mult);
$   [ Use matrix for operations that have been set ]
$   MatDestroy(mat);

.keywords: matrix, shell, create

.seealso: MatShellSetOperation(), MatHasOperation(), MatShellGetContext()
@*/
int MatCreateShell(MPI_Comm comm,int m,int n,void *ctx,Mat *A)
{
  Mat       B;
  Mat_Shell *b;

  PetscHeaderCreate(B,_Mat,MAT_COOKIE,MATSHELL,comm);
  PLogObjectCreate(B);
  B->factor    = 0;
  B->destroy   = MatDestroy_Shell;
  B->assembled = PETSC_TRUE;
  PetscMemcpy(&B->ops,&MatOps,sizeof(struct _MatOps));

  b          = PetscNew(Mat_Shell); CHKPTRQ(b);
  PetscMemzero(b,sizeof(Mat_Shell));
  B->data   = (void *) b;
  b->m = m;  B->m = 0; B->M = m;
  b->n = n;  B->n = 0; B->N = n;
  b->ctx     = ctx;
  *A = B;
  return 0;
}

/*@
    MatShellSetOperation - Allows user to set a matrix operation for
                           a shell matrix.

    Input Parameters:
.   mat - the shell matrix
.   op - the name of the operation
.   f - the function that provides the operation.

    Usage:
$      extern int usermult(Mat,Vec,Vec);
$      ierr = MatCreateShell(comm,m,m,ctx,&A);
$      ierr = MatShellSetOperation(A,MAT_MULT,usermult);

    Notes:
    See the file petsc/include/mat.h for a complete list of matrix
    operations, which all have the form MAT_<OPERATION>, where
    <OPERATION> is the name (in all capital letters) of the
    user interface routine (e.g., MatMult() -> MAT_MULT).

    All user-provided functions should have the same calling
    sequence as the usual matrix interface routines, since they
    are intended to be accessed via the usual matrix interface
    routines, e.g., 
$       MatMult(Mat,Vec,Vec) -> usermult(Mat,Vec,Vec)

    Within each user-defined routine, the user should call
    MatShellGetContext() to obtain the user-defined context that was
    set by MatCreateShell().

.keywords: matrix, shell, set, operation

.seealso: MatCreateShell(), MatShellGetContext()
@*/
int MatShellSetOperation(Mat mat,MatOperation op, void *f)
{
  PetscValidHeaderSpecific(mat,MAT_COOKIE);

  if (op == MAT_DESTROY) {
    if (mat->type == MATSHELL) {
       Mat_Shell *shell = (Mat_Shell *) mat->data;
       shell->destroy                                 = (int (*)(Mat)) f;
    } 
    else mat->destroy                                 = (int (*)(PetscObject)) f;
  } 
  else if (op == MAT_VIEW) mat->view                  = (int (*)(PetscObject,Viewer)) f;
  else                     (((void **)&mat->ops)[op]) = f;

  return 0;
}






