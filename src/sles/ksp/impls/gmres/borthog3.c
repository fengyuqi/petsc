#ifdef PETSC_RCS_HEADER
static char vcid[] = "$Id: borthog3.c,v 1.9 1998/08/18 22:28:03 bsmith Exp bsmith $";
#endif
/*
    Routines used for the orthogonalization of the Hessenberg matrix.

    Note that for the complex numbers version, the VecDot() and
    VecMDot() arguments within the code MUST remain in the order
    given for correct computation of inner products.
*/
#include "src/ksp/impls/gmres/gmresp.h"
#include <math.h>

/*
  This version uses iterative refinement of UNMODIFIED Gram-Schmidt.  
  It can give better performance when running in a parallel 
  environment and in some cases even in a sequential environment (because
  MAXPY has more data reuse).

  Care is taken to accumulate the updated HH/HES values.
 */
#undef __FUNC__  
#define __FUNC__ "KSPGMRESIROrthogonalization"
int KSPGMRESIROrthogonalization(KSP  ksp,int it )
{
  KSP_GMRES *gmres = (KSP_GMRES *)(ksp->data);
  int       j,ncnt;
  Scalar    *hh, *hes,shh[100], *lhh;
  double    dnorm;

  PetscFunctionBegin;
  PLogEventBegin(KSP_GMRESOrthogonalization,ksp,0,0,0);
  /* Don't allocate small arrays */
  if (it < 100) lhh = shh;
  else {
    lhh = (Scalar *)PetscMalloc((it+1) * sizeof(Scalar)); CHKPTRQ(lhh);
  }
  
  /* update Hessenberg matrix and do unmodified Gram-Schmidt */
  hh  = HH(0,it);
  hes = HES(0,it);

  /* Clear hh and hes since we will accumulate values into them */
  for (j=0; j<=it; j++) {
    hh[j]  = 0.0;
    hes[j] = 0.0;
  }

  ncnt = 0;
  do {
    /* 
	 This is really a matrix-vector product, with the matrix stored
	 as pointer to rows 
    */
    VecMDot( it+1, VEC_VV(it+1), &(VEC_VV(0)), lhh ); /* <v,vnew> */

    /*
	 This is really a matrix vector product: 
	 [h[0],h[1],...]*[ v[0]; v[1]; ...] subtracted from v[it+1].
    */
    for (j=0; j<=it; j++) lhh[j] = - lhh[j];
    VecMAXPY(it+1, lhh, VEC_VV(it+1),&VEC_VV(0) );
    for (j=0; j<=it; j++) {
      hh[j]  -= lhh[j];     /* hh += <v,vnew> */
      hes[j] += lhh[j];     /* hes += - <v,vnew> */
    }

    /* Note that dnorm = (norm(d))**2 */
    dnorm = 0.0;
#if defined(USE_PETSC_COMPLEX)
    for (j=0; j<=it; j++) dnorm += PetscReal(lhh[j] * PetscConj(lhh[j]));
#else
    for (j=0; j<=it; j++) dnorm += lhh[j] * lhh[j];
#endif

    /* Continue until either we have only small corrections or we've done
	 as much work as a full orthogonalization (in terms of Mdots) */
  } while (dnorm > 1.0e-16 && ncnt++ < 2);

  /* It would be nice to put ncnt somewhere.... */
  PLogInfo(ksp,"KSPGMRESIROrthogonalization: Number of iterative refinement steps %d dnorm %g\n",ncnt,sqrt(dnorm));

  if (it >= 100) PetscFree( lhh );
  PLogEventEnd(KSP_GMRESOrthogonalization,ksp,0,0,0);
  PetscFunctionReturn(0);
}



