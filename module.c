#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <assert.h>
#include "data.h"

/**
 * @file
 * @brief tinySAT module for CPython
 * @author geome_try
 * @date 2020
 */

// Object structure for TspSolver
typedef struct {
    PyObject_HEAD // <=> PyObject ob_base;
    struct SFormula ob_formula;
} TspSolverObject;

// Type object instance for TspSolver
static PyTypeObject TSPSolverType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "tinysatpy.Solver",
    .tp_doc = "implements a SAT solver",
    .tp_basicsize = sizeof(TspSolverObject),
    .tp_itemsize = 0,
    .tp_flags = Py_TPFLAGS_DEFAULT,
    .tp_new = PyType_GenericNew
};

extern int ts_solve(const struct SFormula*, struct SMatch*);
extern int ts_match(const struct SFormula*, const struct SMatch*);

extern int destroy_match(struct SMatch*);
extern int destroy_clause(struct SClause*);
extern int destroy_formula(struct SFormula*);

int lst2formula(const PyObject* lst, struct SFormula* formula);
int lst2clause(const PyObject* lst, struct SClause* clause);
int lst2match(const PyObject* lst, struct SMatch* match);

PyObject* formula2lst(const struct SFormula* formula);
PyObject* clause2lst(const struct SClause* clause);
PyObject* match2lst(const struct SMatch* match);

static const char TSPSolveDocStr[] = "solves SAT formula";
static PyObject* tsp_solve(PyObject* self, PyObject* args, PyObject* kwargs);

static const char TSPMatchDocStr[] = "checks if solution is correct";
static PyObject* tsp_match(PyObject* self, PyObject* args, PyObject* kwargs);

// TODO: to replace with non-static state structure
static PyObject* TSPError = NULL;

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
    .m_size = -1,
    .m_methods = TSPMethods
};

// PyMODINIT_FUNC <=> extern "C" PyObject*
PyMODINIT_FUNC PyInit_tinysatpy(void)
{
    PyObject* self = PyModule_Create(&TSPModule);
    if (!self) return NULL;

    if (PyType_Ready(&TSPSolverType) < 0)
        return NULL;

    TSPError = PyErr_NewException("tinysatpy.error", NULL, NULL);

    Py_INCREF(&TSPSolverType);
    if (PyModule_AddObject(self, "Solver", (PyObject*) &TSPSolverType))
    {
        Py_DECREF(&TSPSolverType);
        Py_DECREF(self);

        return NULL;
    }

    Py_INCREF(TSPError);
    if (PyModule_AddObject(self, "error", TSPError) < 0) 
    {
        Py_XDECREF(TSPError);
        Py_CLEAR(TSPError);
        Py_DECREF(self);

        return NULL;
    }

    return self;
}

static PyObject* tsp_solve(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* const kwlist[] = { "formula", NULL };

    struct SFormula formula = {};
    struct SMatch match = {};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&", (char**) kwlist, 
                lst2formula, &formula))
    {
        // TSPError->... = ...;
        goto error;
    }

    if (!ts_solve(&formula, &match))
    {
        // TSPError->... = ...;
        goto error;
    }

    PyObject* result = match2lst(&match);

    destroy_match(&match);
    destroy_formula(&formula);

    return result;

error:
    destroy_formula(&formula);
    destroy_match(&match);

    return NULL;
}

static PyObject* tsp_match(PyObject* self, PyObject* args, PyObject* kwargs)
{
    static const char* const kwlist[] = { "formula", "match", NULL };

    int result = -1;

    struct SFormula formula = {};
    struct SMatch match = {};

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O&O&", (char**) kwlist, 
                lst2formula, &formula, lst2match, &match))
    {
        // TSPError->... = ...;
        goto error;
    }

    result = ts_match(&formula, &match);

    if (result < 0)
    {
        // TSPError->... = ...;
        goto error;
    }

    destroy_formula(&formula);
    destroy_match(&match);

    return PyBool_FromLong(!!result);

error:
    destroy_formula(&formula);
    destroy_match(&match);

    return NULL;
}

int lst2formula(const PyObject* lst, struct SFormula* formula)
{
    if (!PyList_Check((PyObject*) lst) || !formula)
        return 0;

    formula->clauses_cnt = PyList_Size((PyObject*) lst);
    formula->clauses_arr = (struct SClause*) 
        calloc(formula->clauses_cnt, sizeof(struct SClause));
    for (int cls_idx = 0; cls_idx < formula->clauses_cnt; ++cls_idx)
    {
        PyObject* cls = PyList_GetItem((PyObject*) lst, cls_idx);
        if (!PyList_Check(cls))
            return 0;

        lst2clause(cls, formula->clauses_arr + cls_idx);
    }

    return 1;
}

int lst2clause(const PyObject* lst, struct SClause* clause)
{
    if (!PyList_Check((PyObject*) lst) || !clause)
        return 0;

    clause->literals_cnt = PyList_Size((PyObject*) lst);
    clause->literals_arr = (int*) calloc(clause->literals_cnt, sizeof(int));
    for (int lit_idx = 0; lit_idx < clause->literals_cnt; ++lit_idx)
    {
        PyObject* lit = PyList_GetItem((PyObject*) lst, lit_idx);
        if (!PyLong_Check(lit))
            return 0;

        clause->literals_arr[lit_idx] = PyLong_AsLong(lit);
    }

    return 1;
}

int lst2match(const PyObject* lst, struct SMatch* match)
{
    if (!PyList_Check((PyObject*) lst) || !match)
        return 0;

    match->values_cnt = PyList_Size((PyObject*) lst);
    match->values_arr = (int*) calloc(match->values_cnt, sizeof(int));
    for (int val_idx = 0; val_idx < match->values_cnt; ++val_idx)
    {
        PyObject* val = PyList_GetItem((PyObject*) lst, val_idx);
        if (!PyBool_Check(val))
            return 0;

        match->values_arr[val_idx] = (val == Py_True);
    }

    return 1;
}

PyObject* formula2lst(const struct SFormula* formula)
{
    if (!formula) return NULL;

    PyObject* res = PyList_New(formula->clauses_cnt);
    if (!res) return NULL;

    for (int cls_idx = 0; cls_idx < formula->clauses_cnt; ++cls_idx)
    {
        if (PyList_SetItem(res, cls_idx, 
                    clause2lst(formula->clauses_arr + cls_idx)) < 0)
        {
            Py_DECREF(res);
            return NULL;
        }
    }

    return res;
}

PyObject* clause2lst(const struct SClause* clause)
{
    if (!clause) return NULL;

    PyObject* res = PyList_New(clause->literals_cnt);
    if (!res) return NULL;

    for (int lit_idx = 0; lit_idx < clause->literals_cnt; ++lit_idx)
    {
        if (PyList_SetItem(res, lit_idx, 
                    PyLong_FromLong(clause->literals_arr[lit_idx])) < 0)
        {
            Py_DECREF(res);
            return NULL;
        }
    }

    return res;
}

PyObject* match2lst(const struct SMatch* match)
{
    if (!match) return NULL;

    PyObject* res = PyList_New(match->values_cnt);
    if (!res) return NULL;

    for (int val_idx = 0; val_idx < match->values_cnt; ++val_idx)
    {
        if (PyList_SetItem(res, val_idx, 
                    PyBool_FromLong(match->values_arr[val_idx])) < 0)
        {
            Py_DECREF(res);
            return NULL;
        }
    }

    return res;
}

