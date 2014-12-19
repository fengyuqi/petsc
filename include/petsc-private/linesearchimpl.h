#ifndef __LINESEARCHIMPL_H
#define __LINESEARCHIMPL_H

#include <petscsnes.h>
#include <petsc-private/petscimpl.h>

PETSC_EXTERN PetscBool SNESLineSearchRegisterAllCalled;
PETSC_EXTERN PetscErrorCode SNESLineSearchRegisterAll(void);

typedef struct _LineSearchOps *LineSearchOps;

struct _LineSearchOps {
  PetscErrorCode (*view)(SNESLineSearch, PetscViewer);
  SNESLineSearchApplyFunc        apply;
  PetscErrorCode (*precheck)(SNESLineSearch,Vec,Vec,PetscBool*,void*);
  SNESLineSearchVIProjectFunc    viproject;
  SNESLineSearchVINormFunc       vinorm;
  PetscErrorCode (*postcheck)(SNESLineSearch,Vec,Vec,Vec,PetscBool *,PetscBool *,void*);
  PetscErrorCode (*setfromoptions)(SNESLineSearch);
  PetscErrorCode (*reset)(SNESLineSearch);
  PetscErrorCode (*destroy)(SNESLineSearch);
  PetscErrorCode (*setup)(SNESLineSearch);
  PetscErrorCode (*snesfunc)(SNES,Vec,Vec);
};

struct _p_LineSearch {
  PETSCHEADER(struct _LineSearchOps);

  SNES                snes;

  void                *data;

  PetscBool           setupcalled;

  Vec                 vec_sol;
  Vec                 vec_sol_new;
  Vec                 vec_func;
  Vec                 vec_func_new;
  Vec                 vec_update;

  PetscInt            nwork;
  Vec                 *work;

  PetscReal           lambda;

  PetscBool           norms;
  PetscReal           fnorm;
  PetscReal           ynorm;
  PetscReal           xnorm;
  PetscBool           success;
  PetscBool           keeplambda;

  PetscReal           damping;
  PetscReal           maxstep;
  PetscReal           steptol;
  PetscInt            max_its;
  PetscReal           rtol;
  PetscReal           atol;
  PetscReal           ltol;
  PetscInt            order;

  PetscReal           precheck_picard_angle;

  void *              precheckctx;
  void *              postcheckctx;

  PetscViewer         monitor;

};

#endif
