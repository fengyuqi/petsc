#ifndef lint
static char vcid[] = "$Id: pcregis.c,v 1.7 1995/03/06 04:12:59 bsmith Exp bsmith $";
#endif


#include "petsc.h"
#include "pcimpl.h"

extern int PCiJacobiCreate(PC);
extern int PCiBJacobiCreate(PC);
extern int PCiNoneCreate(PC);
extern int PCiDirectCreate(PC);
extern int PCiSORCreate(PC);
extern int PCiShellCreate(PC);
extern int PCiMGCreate(PC);
extern int PCiESORCreate(PC);
extern int PCiILUCreate(PC);

/*@
   PCRegisterAll - Registers all the iterative methods
  in KSP.

  Notes:
  To prevent all the methods from being
  registered and thus save memory, copy this routine and
  register only those methods desired.
@*/
int PCRegisterAll()
{
  PCRegister(PCNONE         , "none",       PCiNoneCreate);
  PCRegister(PCJACOBI       , "jacobi",     PCiJacobiCreate);
  PCRegister(PCBJACOBI      , "bjacobi",    PCiBJacobiCreate);
  PCRegister(PCSOR          , "sor",        PCiSORCreate);
  PCRegister(PCDIRECT       , "direct",     PCiDirectCreate);
  PCRegister(PCSHELL        , "shell",      PCiShellCreate);
  PCRegister(PCMG           , "mg",         PCiMGCreate);
  PCRegister(PCESOR         , "eisenstat",  PCiESORCreate);
  PCRegister(PCILU          , "ilu",        PCiILUCreate);
  return 0;
}


