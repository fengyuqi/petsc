#requirespackage 'PETSC_HAVE_VIENNACL'
ALL: lib

CFLAGS   =
FFLAGS   =
SOURCECXX= mpiaijviennacl.cxx
SOURCEF  =
SOURCEH  =
LIBBASE  = libpetscmat
DIRS     =
MANSEC   = Mat
LOCDIR   = src/mat/impls/aij/mpi/mpiviennacl/

include ${PETSC_DIR}/lib/petsc-conf/variables
include ${PETSC_DIR}/lib/petsc-conf/rules
include ${PETSC_DIR}/lib/petsc-conf/test
