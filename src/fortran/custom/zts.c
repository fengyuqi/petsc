/*$Id: zts.c,v 1.33 2000/06/24 03:57:08 balay Exp balay $*/

#include "src/fortran/custom/zpetsc.h"
#include "petscts.h"

#ifdef PETSC_HAVE_FORTRAN_CAPS
#define tssetrhsfunction_                    TSSETRHSFUNCTION
#define tssetrhsmatrix_                      TSSETRHSMATRIX
#define tssetrhsjacobian_                    TSSETRHSJACOBIAN
#define tscreate_                            TSCREATE
#define tsgetsolution_                       TSGETSOLUTION
#define tsgetsnes_                           TSGETSNES
#define tsgetsles_                           TSGETSLES
#define tsgettype_                           TSGETTYPE
#define tsdestroy_                           TSDESTROY
#define tssetmonitor_                        TSSETMONITOR
#define tssettype_                           TSSETTYPE
#define tspvodegetiterations_                TSPVODEGETITERATIONS
#define tsdefaultcomputejacobian_            TSDEFAULTCOMPUTEJACOBIAN
#define tsdefaultcomputejacobiancolor_       TSDEFAULTCOMPUTEJACOBIANCOLOR
#define tsgetoptionsprefix_                  TSGETOPTIONSPREFIX
#define tsdefaultmonitor_                    TSDEFAULTMONITOR
#define tsview_                              TSVIEW
#define tsgetrhsjacobian_                    TSGETRHSJACOBIAN
#define tsgetrhsmatrix_                      TSGETRHSMATRIX
#define tssetrhsboundaryconditions_          TSSETRHSBOUNDARYCONDITIONS
#elif !defined(PETSC_HAVE_FORTRAN_UNDERSCORE)
#define tsdefaultcomputejacobian_            tsdefaultcomputejacobian
#define tsdefaultcomputejacobiancolor_       tsdefaultcomputejacobiancolor
#define tspvodegetiterations_                tspvodegetiterations
#define tssetrhsfunction_                    tssetrhsfunction
#define tssetrhsmatrix_                      tssetrhsmatrix
#define tssetrhsjacobian_                    tssetrhsjacobian
#define tscreate_                            tscreate
#define tsgetsolution_                       tsgetsolution
#define tsgetsnes_                           tsgetsnes
#define tsgetsles_                           tsgetsles
#define tsgettype_                           tsgettype
#define tsdestroy_                           tsdestroy
#define tssetmonitor_                        tssetmonitor
#define tssettype_                           tssettype
#define tsgetoptionsprefix_                  tsgetoptionsprefix
#define tsdefaultmonitor_                    tsdefaultmonitor
#define tsview_                              tsview
#define tsgetrhsjacobian_                    tsgetrhsjacobian
#define tsgetrhsmatrix_                      tsgetrhsmatrix
#define tssetrhsboundaryconditions_          tssetrhsboundaryconditions
#endif

EXTERN_C_BEGIN

static int ourtsbcfunction(TS ts,double d,Vec x,void *ctx)
{
  int ierr = 0;
  (*(void (PETSC_STDCALL *)(TS*,double*,Vec*,void*,int*))(((PetscObject)ts)->fortran_func_pointers[0]))(&ts,&d,&x,ctx,&ierr);
  return 0;
}

void PETSC_STDCALL tssetrhsboundaryconditions_(TS *ts,int (PETSC_STDCALL *f)(TS*,double*,Vec*,void*,int*),void *ctx,int *ierr)
{
  ((PetscObject)*ts)->fortran_func_pointers[0] = (void*)f;
  *ierr = TSSetRHSBoundaryConditions(*ts,ourtsbcfunction,ctx);
}

void PETSC_STDCALL tsgetrhsjacobian_(TS *ts,Mat *J,Mat *M,void **ctx,int *ierr)
{
  *ierr = TSGetRHSJacobian(*ts,J,M,ctx);
}

void PETSC_STDCALL tsgetrhsmatrix_(TS *ts,Mat *J,Mat *M,void **ctx,int *ierr)
{
  *ierr = TSGetRHSMatrix(*ts,J,M,ctx);
}

void PETSC_STDCALL tsview_(TS *ts,Viewer *viewer, int *ierr)
{
  Viewer v;
  PetscPatchDefaultViewers_Fortran(viewer,v);
  *ierr = TSView(*ts,v);
}

/* function */
void tsdefaultcomputejacobian_(TS *ts,double *t,Vec *xx1,Mat *J,Mat *B,MatStructure *flag,void *ctx,int *ierr)
{
  *ierr = TSDefaultComputeJacobian(*ts,*t,*xx1,J,B,flag,ctx);
}

/* function */
void tsdefaultcomputejacobiancolor_(TS *ts,double *t,Vec *xx1,Mat *J,Mat *B,MatStructure *flag,void *ctx,int *ierr)
{
  *ierr = TSDefaultComputeJacobianColor(*ts,*t,*xx1,J,B,flag,*(MatFDColoring*)ctx);
}

void PETSC_STDCALL tssettype_(TS *ts,CHAR type PETSC_MIXED_LEN(len),int *ierr PETSC_END_LEN(len))
{
  char *t;

  FIXCHAR(type,len,t);
  *ierr = TSSetType(*ts,t);
  FREECHAR(type,t);
}

static int ourtsfunction(TS ts,double d,Vec x,Vec f,void *ctx)
{
  int ierr = 0;
  (*(void (PETSC_STDCALL *)(TS*,double*,Vec*,Vec*,void*,int*))(((PetscObject)ts)->fortran_func_pointers[1]))(&ts,&d,&x,&f,ctx,&ierr);
  return 0;
}

void PETSC_STDCALL tssetrhsfunction_(TS *ts,int (PETSC_STDCALL *f)(TS*,double*,Vec*,Vec*,void*,int*),void*fP,int *ierr)
{
  ((PetscObject)*ts)->fortran_func_pointers[1] = (void*)f;
  *ierr = TSSetRHSFunction(*ts,ourtsfunction,fP);
}


/* ---------------------------------------------------------*/
static int ourtsmatrix(TS ts,double d,Mat* m,Mat* p,MatStructure* type,void*ctx)
{
  int ierr = 0;
  (*(void (PETSC_STDCALL *)(TS*,double*,Mat*,Mat*,MatStructure*,void*,int*))(((PetscObject)ts)->fortran_func_pointers[2]))(&ts,&d,m,p,type,ctx,&ierr);
  return 0;
}

void PETSC_STDCALL tssetrhsmatrix_(TS *ts,Mat *A,Mat *B,int (PETSC_STDCALL *f)(TS*,double*,Mat*,Mat*,MatStructure*,
                                                   void*,int *),void*fP,int *ierr)
{
  if (FORTRANNULLFUNCTION(f)) {
    *ierr = TSSetRHSMatrix(*ts,*A,*B,PETSC_NULL,fP);
  } else {
    ((PetscObject)*ts)->fortran_func_pointers[2] = (void*)f;
    *ierr = TSSetRHSMatrix(*ts,*A,*B,ourtsmatrix,fP);
  }
}

/* ---------------------------------------------------------*/
static int ourtsjacobian(TS ts,double d,Vec x,Mat* m,Mat* p,MatStructure* type,void*ctx)
{
  int ierr = 0;
  (*(void (PETSC_STDCALL *)(TS*,double*,Vec*,Mat*,Mat*,MatStructure*,void*,int*))(((PetscObject)ts)->fortran_func_pointers[3]))(&ts,&d,&x,m,p,type,ctx,&ierr);
  return 0;
}

void PETSC_STDCALL tssetrhsjacobian_(TS *ts,Mat *A,Mat *B,void (PETSC_STDCALL *f)(TS*,double*,Vec*,Mat*,Mat*,MatStructure*,
               void*,int*),void*fP,int *ierr)
{
  if (FORTRANNULLFUNCTION(f)) {
    *ierr = TSSetRHSJacobian(*ts,*A,*B,PETSC_NULL,fP);
  } else if ((void*)f == (void*)tsdefaultcomputejacobian_) {
    *ierr = TSSetRHSJacobian(*ts,*A,*B,TSDefaultComputeJacobian,fP);
  } else if ((void*)f == (void*)tsdefaultcomputejacobiancolor_) {
    *ierr = TSSetRHSJacobian(*ts,*A,*B,TSDefaultComputeJacobianColor,*(MatFDColoring*)fP);
  } else {
  ((PetscObject)*ts)->fortran_func_pointers[3] = (void*)f;
    *ierr = TSSetRHSJacobian(*ts,*A,*B,ourtsjacobian,fP);
  }
}

void PETSC_STDCALL tsgetsolution_(TS *ts,Vec *v,int *ierr)
{
  *ierr = TSGetSolution(*ts,v);
}

void PETSC_STDCALL tscreate_(MPI_Comm *comm,TSProblemType *problemtype,TS *outts,int *ierr)
{
  *ierr = TSCreate((MPI_Comm)PetscToPointerComm(*comm),*problemtype,outts);
  ((PetscObject)*outts)->fortran_func_pointers = (void**)PetscMalloc(7*sizeof(void *));
  if (!((PetscObject)*outts)->fortran_func_pointers) {*ierr = 1; return;}
}

void PETSC_STDCALL tsgetsnes_(TS *ts,SNES *snes,int *ierr)
{
  *ierr = TSGetSNES(*ts,snes);
}

void PETSC_STDCALL tsgetsles_(TS *ts,SLES *sles,int *ierr)
{
  *ierr = TSGetSLES(*ts,sles);
}

void PETSC_STDCALL tsgettype_(TS *ts,CHAR name PETSC_MIXED_LEN(len),int *ierr PETSC_END_LEN(len))
{
  char *tname;

  *ierr = TSGetType(*ts,(TSType *)&tname);
#if defined(PETSC_USES_CPTOFCD)
  {
    char *t = _fcdtocp(name); int len1 = _fcdlen(name);
    *ierr = PetscStrncpy(t,tname,len1);
  }
#else
  *ierr = PetscStrncpy(name,tname,len);
#endif
}

#if defined(PETSC_HAVE_PVODE)  && !defined(__cplusplus)
void PETSC_STDCALL tspvodegetiterations_(TS *ts,int *nonlin,int *lin,int *ierr)
{
  if (FORTRANNULLINTEGER(nonlin)) nonlin = PETSC_NULL;
  if (FORTRANNULLINTEGER(lin))    lin    = PETSC_NULL;
  *ierr = TSPVodeGetIterations(*ts,nonlin,lin);
}
#endif

void PETSC_STDCALL tsdestroy_(TS *ts,int *ierr){
  *ierr = TSDestroy(*ts);
}

void PETSC_STDCALL tsdefaultmonitor_(TS *ts,int *step,double *dt,Vec *x,void *ctx,int *ierr)
{
  *ierr = TSDefaultMonitor(*ts,*step,*dt,*x,ctx);
}

/*
   Note ctx is the same as ts so we need to get the Fortran context out of the TS
*/
static int ourtsmonitor(TS ts,int i,double d,Vec v,void*ctx)
{
  int        ierr = 0;
  void       *mctx = ((PetscObject)ts)->fortran_func_pointers[6];
  (*(void (PETSC_STDCALL *)(TS*,int*,double*,Vec*,void*,int*))(((PetscObject)ts)->fortran_func_pointers[4]))(&ts,&i,&d,&v,mctx,&ierr);
  return 0;
}

static int ourtsdestroy(void *ctx)
{
  int         ierr = 0;
  TS          ts = (TS)ctx;
  void        *mctx = ((PetscObject)ts)->fortran_func_pointers[6];
  (*(void (PETSC_STDCALL *)(void*,int*))(((PetscObject)ts)->fortran_func_pointers[5]))(mctx,&ierr);
  return 0;
}

void PETSC_STDCALL tssetmonitor_(TS *ts,void (PETSC_STDCALL *func)(TS*,int*,double*,Vec*,void*,int*),void *mctx,void (PETSC_STDCALL *d)(void*,int*),int *ierr)
{
  if ((void*)func == (void*)tsdefaultmonitor_) {
    *ierr = TSSetMonitor(*ts,TSDefaultMonitor,0,0);
  } else {
    ((PetscObject)*ts)->fortran_func_pointers[4] = (void*)func;
    ((PetscObject)*ts)->fortran_func_pointers[5] = (void*)d;
    ((PetscObject)*ts)->fortran_func_pointers[6] = mctx;
    if (FORTRANNULLFUNCTION(d)) {
      *ierr = TSSetMonitor(*ts,ourtsmonitor,*ts,0);
    } else {
      *ierr = TSSetMonitor(*ts,ourtsmonitor,*ts,ourtsdestroy);
    }
  }
}

void PETSC_STDCALL tsgetoptionsprefix_(TS *ts,CHAR prefix PETSC_MIXED_LEN(len),int *ierr PETSC_END_LEN(len))
{
  char *tname;

  *ierr = TSGetOptionsPrefix(*ts,&tname);
#if defined(PETSC_USES_CPTOFCD)
  {
    char *t = _fcdtocp(prefix); int len1 = _fcdlen(prefix);
    *ierr = PetscStrncpy(t,tname,len1);
  }
#else
  *ierr = PetscStrncpy(prefix,tname,len);
#endif
}

EXTERN_C_END

