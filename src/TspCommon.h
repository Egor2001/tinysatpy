#ifndef TINYSATPY_TSP_COMMON_H_
#define TINYSATPY_TSP_COMMON_H_

#ifndef PY_SSIZE_T_CLEAN
    #define PY_SSIZE_T_CLEAN
#endif

#include <Python.h>

#include "tinySAT/include/core/SFormula.hpp"
#include "tinySAT/include/core/SMatch.hpp"

/// Python's list<list<int>> to SFormula 'O&'-parse-compatible converter
extern "C" int tsp_lst2formula(const PyObject* lst, SFormula* formula);

/// Python's list<bool> to SMatch 'O&'-parse-compatible converter
extern "C" int tsp_lst2match(const PyObject* lst, SMatch* match);

/// SFormula to Python's list<list<int>> converter
extern "C" PyObject* tsp_formula2lst(const SFormula* formula);

/// SMatch to Python's list<bool> converter
extern "C" PyObject* tsp_match2lst(const SMatch* match);

#endif // TINYSATPY_TSP_COMMON_H_
