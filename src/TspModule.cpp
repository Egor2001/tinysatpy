#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "TspCommon.h"

#include "tinySAT/include/core/SFormula.hpp"
#include "tinySAT/include/core/SMatch.hpp"

/**
 * @file
 * @brief tinySAT module for CPython
 * @author geome_try
 * @date 2020
 */

/// Returns pointer to TSP_MODULE state
#define TSP_GET_STATE(TSP_MODULE) \
    ((TspModuleState*) PyModule_GetState(TSP_MODULE))

/// Global module state
struct TspModuleState
{
    PyObject* error = NULL; ///< Module exception
    std::unique_ptr<CDpllSolver> solver; ///< Module solver
};

/// Module's state initializer
static TspModuleState* tsp_module_init_state(PyObject* self);
/// Module's types initializer
static TspModuleState* tsp_module_init_types(PyObject* self);

PyDoc_STRVAR(TSPSolveDocStr, "solves SAT formula");
static PyObject* tsp_solve(PyObject* self, PyObject* args, PyObject* kwargs);

PyDoc_STRVAR(TSPMatchDocStr, "checks if solution is correct");
static PyObject* tsp_match(PyObject* self, PyObject* args, PyObject* kwargs);

static PyMethodDef TSPMethods[] = {
    { "solve", (PyCFunction) tsp_solve, 
      METH_VARARGS | METH_KEYWORDS, TSPSolveDocStr },
    { "match", (PyCFunction) tsp_match, 
      METH_VARARGS | METH_KEYWORDS, TSPMatchDocStr },
    { NULL, NULL, 0, NULL }
};

static struct PyModuleDef TSPModule = {
    .m_base = PyModuleDef_HEAD_INIT,
    .m_name = "tinysatpy",
    .m_doc = "brings tinySAT functionality to CPython",
    .m_size = -1, // sizeof(TspModuleState),
    .m_methods = TSPMethods /*,
    .m_traverse = ,
    .m_clear = ,
    .m_free =
    */ 
};

// PyMODINIT_FUNC <=> extern "C" PyObject*
PyMODINIT_FUNC PyInit_tinysatpy(void)
{
    // Creating module
    PyObject* self = PyModule_Create(&TSPModule);
    if (self == NULL) 
        return NULL;

    // Initializing module state
    if (tsp_module_init_state(self) < 0)
    {
        Py_DECREF(self);
        return NULL;
    }

    // Initializing module types
    if (tsp_module_init_types(self) < 0)
    {
        Py_DECREF(self);
        return NULL;
    }

    return self;
}

static int tsp_module_init_state(PyObject* self)
{
    // Checking if module state chunk is allocated
    void* raw_state_ptr = PyModule_GetState(self);
    if (raw_state_ptr == NULL)
        return -1;

    // Constructing module state on the allocated memory chunk
    TspModuleState* state = (TspModuleState*)
    new (raw_state_ptr) TspModuleState {
        .error = PyErr_NewException("tinysatpy.error", NULL, NULL),
        .logger = NULL,
        .solver = NULL
    };

    // Adding error object to the module
    Py_INCREF(state->error);
    if (PyModule_AddObject(self, "error", state->error) < 0) 
    {
        Py_XDECREF(state->error);
        Py_CLEAR(state->error);

        return -1;
    }

    return 0;
}

static int tsp_module_init_types(PyObject* self)
{
    /*
    // TODO: to call extern function to init types
    if (PyType_Ready(&TSPSolverType) < 0)
    {
        Py_DECREF(self);
        return -1;
    }

    Py_INCREF(&TSPSolverType);
    if (PyModule_AddObject(self, "Solver", (PyObject*) &TSPSolverType))
    {
        Py_DECREF(&TSPSolverType);
        Py_DECREF(self);

        return -1;
    }
    */

    return 0;
}

static PyObject* tsp_solve(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* const kwlist[] = { "formula", NULL };

    SFormula formula = {};
    SMatch match = {};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", (char**) kwlist, 
                tsp_lst2formula, &formula))
        return NULL;

    TspModuleState* state = TSP_GET_STATE(self);
    if (state == NULL)
        return NULL;

    state->solver = std::make_unique<CDpllSolver(std::move(formula), nullptr);
    if (auto context = state->solver->context(); context)
        match = context->match();

    return tsp_match2lst(&match);
}

static PyObject* tsp_match(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* const kwlist[] = { "formula", "match", NULL };

    SFormula formula = {};
    SMatch match = {};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&O&", (char**) kwlist, 
                lst2formula, &formula, lst2match, &match))
        return NULL;

    return PyBool_FromLong(formula.is_match(match) ? 1l : 0l);
}
