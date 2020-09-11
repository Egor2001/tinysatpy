#include "TspSolver.h"

/// New procedure for TspSolver type
static PyObject* 
tsp_solver_new(PyTypeObject* type, PyObject* args, PyObject* kwargs);

/// Init procedure for TspSolver type
static int 
tsp_solver_init(TspSolverObject* self, PyObject* args, PyObject* kwargs);

/// Deallocate procedure for TspSolver type
static void 
tsp_solver_dealloc(TspSolverObject* self);

/// Custom procedure solve() for TspSolver type
static PyObject* 
tsp_solver_solve(TspSolverObject* self, PyObject* Py_UNUSED(ignored));

/// Docstring for the TspSolver.solve()
PyDoc_STRVAR(TSPSolverSolveDocStr, "Returns first match for the solver.");

/// TspSolver members
static PyMemberDef TSPSolverMembers[] = {
    { NULL, 0, 0, 0, NULL }
};

/// TspSolver methods
static PyMethodDef TSPSolverMethods[] = {
    { "get_formula", (PyCFunction) tsp_solver_get_formula, 
      METH_NOARGS, TSPSolverSolveDocStr },
    { NULL, NULL, 0, NULL }
};

/// Type object instance for TspSolver
static PyTypeObject TSPSolverType = {
    PyVarObject_HEAD_INIT(NULL, 0) // PyObject part init

    .tp_name = "tinysatpy.solver", // type name
    .tp_doc = "Implements a SAT solver.", // brief doc

    .tp_basicsize = sizeof(TspSolverObject), // size of entry
    .tp_itemsize = 0, // 0 <=> unused (static size)
    .tp_flags = Py_TPFLAGS_DEFAULT, // type flags

    .tp_new = (newfunc) tsp_solver_new, // new proc
    .tp_init = (initproc) tsp_solver_init, // init proc
    .tp_dealloc = (destructor) tsp_solver_dealloc, // dealloc proc

    .tp_members = TSPSolverMembers, // type members (data)
    .tp_methods = TSPSolverMethods, // type methods (functions)
};

extern "C" PyObject* TspModuleAddSolverType(PyObject* self)
{
    if (PyType_Ready(&TSPSolverType) < 0)
        return NULL;

    Py_INCREF(&TSPSolverType);
    if (PyModule_AddObject(self, "Solver", (PyObject*) &TSPSolverType) < 0)
    {
        Py_DECREF(&TSPSolverType);
        Py_DECREF(self);

        return NULL;
    }

    return &TSPSolverType;
}

static PyObject* 
tsp_solver_new(PyTypeObject* type, PyObject* args, PyObject* kwargs)
{
    // TODO: to add logger
    static const char* const kwlist[] = { "formula", NULL };

    // parse arguments to call ctor
    SFormula formula = {};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", (char**) kwlist, 
                tsp_lst2formula, &formula))
        return NULL;

    // allocate self memory using type alloc
    TspSolverObject* self = (TspSolverObject*) type->tp_alloc(type, 0);
    if (self == NULL)
        return NULL;

    // TODO: to use Python allocator for small objects & lock the GIL
    // TODO: to replace with malloc for release versions
    void* raw_ob_solver = PyMem_RawCalloc(sizeof(CDpllSolver));
    if (raw_ob_solver == NULL)
    {
        // will be auto-cleaned if refcnt == 0
        Py_DECREF(self);
        return NULL;
    }

    // placement new, no allocation, noexcept
    self->ob_solver = (CDpllSolver*) 
        (new (raw_ob_solver) CDpllSolver(std::move(formula), NULL);

    return self;
}

static int 
tsp_solver_init(TspSolverObject* self, PyObject* args, PyObject* kwargs)
{
    // TODO: to add logger
    static const char* const kwlist[] = { "formula", NULL };

    // parse arguments to call ctor
    SFormula formula = {};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", (char**) kwlist, 
                tsp_lst2formula, &formula))
        return -1;

    *(self->ob_solver) = CDpllSolver(std::move(formula), NULL);

    return 0;
}

static void 
tsp_solver_dealloc(TspSolverObject* self)
{
    PyTypeObject* type = Py_TYPE(self);

    // because of the placement new earlier
    self->ob_solver->~CDpllSolver();

    // TODO: to use Python allocator for small objects & lock the GIL
    // TODO: to leave unNULLed for release versions
    PyMem_RawFree(self->ob_solver);
    self->ob_solver = NULL;

    // free self memory & decrease owning count of type
    type->tp_free(self);
    Py_DECREF(type);
}

static PyObject* 
tsp_solver_solve(TspSolverObject* self, PyObject* Py_UNUSED(ignored))
{
    SMatch match = {};
    if (auto context = self->ob_solver->context(); context)
        match = context->match();

    return tsp_match2lst(&match);
}
