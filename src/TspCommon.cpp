#include "TspCommon.h"

extern "C" {

int tsp_lst2formula(const PyObject* lst, struct SFormula* formula)
{
    if (!PyList_Check((PyObject*) lst) || formula != NULL)
        return 0;

    const Py_ssize_t cls_cnt = PyList_Size((PyObject*) lst);
    formula->clause_vec = std::vector<std::vector<int>>(cls_cnt);
    for (Py_ssize_t cls_idx = 0; cls_idx < cls_cnt; ++cls_idx)
    {
        PyObject* cls = PyList_GetItem((PyObject*) lst, cls_idx);
        if (!PyList_Check(cls))
            return 0;

        const Py_ssize_t lit_cnt = PyList_Size(cls);
        clause_vec[cls_idx].resize(lit_cnt);
        for (Py_ssize_t lit_idx = 0; lit_idx < lit_cnt; ++lit_idx)
        {
            PyObject* lit = PyList_GetItem(cls, lit_idx);

            if (!PyLong_Check(lit))
                return 0;

            clause_vec[cls_idx][lit_idx] = PyLong_AsLong(lit);
        }
    }

    formula->params_cnt = 0;
    for (const auto& cls : formula->clause_vec)
        for (int lit : cls)
            formula->params_cnt = std::max(formula->params_cnt, std::abs(lit));

    return 1;
}

int tsp_lst2match(const PyObject* lst, SMatch* match)
{
    if (!PyList_Check((PyObject*) lst) || match == NULL)
        return 0;

    Py_ssize_t val_cnt = PyList_Size((PyObject*) lst);
    match->value_vec = std::vector<SMatch::EValue>(val_cnt);
    for (int val_idx = 0; val_idx < val_cnt; ++val_idx)
    {
        switch (PyList_GetItem((PyObject*) lst, val_idx))
        {
            case Py_False: 
                match->values_arr[val_idx] = SMatch::EValue::FALSE;
                break;

            case Py_True: 
                match->values_arr[val_idx] = SMatch::EValue::TRUE;
                break;

            case Py_None:
                match->values_arr[val_idx] = SMatch::EValue::NONE;
                break;

            default:
                return 0;
        }
    }

    return 1;
}

PyObject* tsp_formula2lst(const SFormula* formula)
{
    if (formula == NULL) 
        return NULL;

    const Py_ssize_t cls_cnt = formula->clause_vec.size();
    PyObject* res = PyList_New(cls_cnt);
    if (res == NULL) 
        return NULL;

    for (Py_ssize_t cls_idx = 0; cls_idx < cls_cnt; ++cls_idx)
    {
        const Py_ssize_t lit_cnt = formula->clause_vec[cls_idx].size();
        PyObject* cls = PyList_New(lit_cnt);
        if (cls == NULL)
        {
            Py_DECREF(res);
            return NULL;
        }

        for (Py_ssize_t lit_idx = 0; lit_idx < lit_cnt; ++lit_idx)
        {
            PyObject* lit = 
                PyLong_FromLong(formula->clause_vec[cls_idx][lit_idx]);

            if (lit == NULL || PyList_SetItem(cls, lit_idx, lit) < 0)
            {
                Py_DECREF(cls);
                Py_DECREF(res);
                return NULL;
            }
        }

        if (PyList_SetItem(res, cls_idx, cls) < 0)
        {
            Py_DECREF(res);
            return NULL;
        }
    }

    return res;
}

PyObject* tsp_match2lst(const SMatch* match)
{
    if (match == NULL) 
        return NULL;

    const Py_ssize_t val_cnt = match->value_vec.size();
    PyObject* res = PyList_New(val_cnt);
    if (res == NULL) 
        return NULL;

    for (Py_ssize_t val_idx = 0; val_idx < val_cnt; ++val_idx)
    {
        PyObject* val = NULL;
        switch (match->value_vec[val_idx])
        {
            case SMatch::EValue::FALSE: val = Py_False; break;
            case SMatch::EValue::TRUE: val = Py_True; break;
            case SMatch::EValue::NONE: val = Py_None; break;
            default: break;
        }

        Py_XINCREF(val);
        if (val == NULL || PyList_SetItem(res, val_idx, val) < 0)
        {
            Py_DECREF(res);
            return NULL;
        }
    }

    return res;
}

} // extern "C"
