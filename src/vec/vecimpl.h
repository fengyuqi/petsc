/* $Id: vecimpl.h,v 1.26 1996/03/19 21:22:48 bsmith Exp curfman $ */
/* 
   This private file should not be included in users' code.
*/

#ifndef __VECIMPL 
#define __VECIMPL
#include "vec.h"

/* vector operations */
struct _VeOps {
  int  (*duplicate)(Vec,Vec*),           /* get single vector */
       (*getvecs)(Vec,int,Vec**),        /* get array of vectors */
       (*destroyvecs)(Vec*,int),            /* free array of vectors */
       (*dot)(Vec,Vec,Scalar*),          /* z = x^H * y */
       (*mdot)(int,Vec,Vec*,Scalar*),    /* z[j] = x dot y[j] */
       (*norm)(Vec,NormType,double*),    /* z = sqrt(x^H * x) */
       (*tdot)(Vec,Vec,Scalar*),         /* x'*y */
       (*mtdot)(int,Vec,Vec*,Scalar*),   /* z[j] = x dot y[j] */
       (*scale)(Scalar*,Vec),            /* x = alpha * x   */
       (*copy)(Vec,Vec),                 /* y = x */
       (*set)(Scalar*,Vec),              /* y = alpha  */
       (*swap)(Vec,Vec),                 /* exchange x and y */
       (*axpy)(Scalar*,Vec,Vec),         /* y = y + alpha * x */
       (*axpby)(Scalar*,Scalar*,Vec,Vec),/* y = y + alpha * x + beta * y*/
       (*maxpy)(int,Scalar*,Vec,Vec*),   /* y = y + alpha[j] x[j] */
       (*aypx)(Scalar*,Vec,Vec),         /* y = x + alpha * y */
       (*waxpy)(Scalar*,Vec,Vec,Vec),    /* w = y + alpha * x */
       (*pointwisemult)(Vec,Vec,Vec),    /* w = x .* y */
       (*pointwisedivide)(Vec,Vec,Vec),  /* w = x ./ y */
       (*setvalues)(Vec,int,int*,Scalar*,InsertMode),
       (*assemblybegin)(Vec),            /* start global assembly */
       (*assemblyend)(Vec),              /* end global assembly */
       (*getarray)(Vec,Scalar**),        /* get data array */
       (*getsize)(Vec,int*),(*getlocalsize)(Vec,int*),
       (*getownershiprange)(Vec,int*,int*),
       (*restorearray)(Vec,Scalar**),    /* restore data array */
       (*max)(Vec,int*,double*),         /* z = max(x); idx=index of max(x) */
       (*min)(Vec,int*,double*),         /* z = min(x); idx=index of min(x) */
       (*setrandom)(PetscRandom,Vec);       /* set y[j] = random numbers */
};

struct _Vec {
  PETSCHEADER                            /* general PETSc header */
  struct _VeOps ops;                     /* vector operations */
  void          *data;                   /* implementation-specific data */
  int           N, n;                    /* global, local vector size */
};

typedef enum { VEC_SCATTER_GENERAL, VEC_SCATTER_STRIDE, VEC_SCATTER_MPI,
               VEC_SCATTER_MPITOALL} VecScatterType;

/* 
   These scatters are for the purely local case.
*/

typedef struct {
  VecScatterType type;
  int            n, *slots;             /* number of components and their locations */
} VecScatter_General;

typedef struct {
  VecScatterType type;
  int             n, first, step;           
} VecScatter_Stride;

/*
   This scatter is for a global vector copied (completely) to each processor
*/

typedef struct {
  VecScatterType type;
  int            *count;              /* elements of vector on each processor */
  Scalar         *work, *work2;        
} VecScatter_MPIToAll;

/*
   This is the parallel scatter
*/
typedef struct { 
  VecScatterType     type;
  int                n;        /* number of processors to send/receive */
  int                nbelow;   /* number with lower process id */
  int                nself;    /* number sending to self */
  int                *starts;  /* starting point in indices and values for each proc*/ 
  int                *indices; /* list of all components sent or received */
  int                *procs;   /* processors we are communicating with in scatter */
  MPI_Request        *requests;
  Scalar             *values;  /* buffer for all sends or receives */
                               /* note that we pack/unpack ourselves;
                                   we do not use MPI packing */
  VecScatter_General local;    /* any part that happens to be local */
  MPI_Status         *sstatus;
} VecScatter_MPI;

struct _VecScatter {
  PETSCHEADER
  int     inuse;           /* prevents corruption from mixing two scatters */
  int     (*scatterbegin)(Vec,Vec,InsertMode,int,VecScatter);
  int     (*scatterend)(Vec,Vec,InsertMode,int,VecScatter);
  int     (*pipelinebegin)(Vec,Vec,InsertMode,PipelineMode,VecScatter);
  int     (*pipelineend)(Vec,Vec,InsertMode,PipelineMode,VecScatter);
  int     (*copy)(VecScatter,VecScatter);
  void    *fromdata,*todata;
};

/* Default obtain and release vectors; can be used by any implementation */
extern int     Veiobtain_vectors(Vec, int, Vec **);
extern int     Veirelease_vectors(Vec *,int);

#endif
