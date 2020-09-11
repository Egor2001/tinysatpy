#ifndef TINYSATPY_SOLVER_H_
#define TINYSATPY_SOLVER_H_

#ifndef PY_SSIZE_T_CLEAN
    #define PY_SSIZE_T_CLEAN
#endif

#include <Python.h>
#include "TspCommon.h"
#include "tinySAT/include/CDpllSolver.hpp"

/// Adds TspSolver type to module's dictionary
extern "C" PyObject* TspModuleAddSolverType(PyObject* self);

/// Data structure for TspSolver
struct TspSolverObject
{
    PyObject_HEAD
    CDpllSolver* ob_solver; ///< tinySAT solver
};

#endif // TINYSATPY_SOLVER_H_
