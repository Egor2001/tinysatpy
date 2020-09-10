#include <vector>
#include "data.h"

extern "C" int ts_solve(const struct SFormula*, struct SMatch*);
extern "C" int ts_match(const struct SFormula*, const struct SMatch*);

extern "C" int destroy_match(struct SMatch*);
extern "C" int destroy_clause(struct SClause*);
extern "C" int destroy_formula(struct SFormula*);

int ts_solve(const struct SFormula*, struct SMatch*)
{
    std::vector<int> vec(10);
    for (int i = 0; i < 10; ++i)
        vec[i] = i;

    return vec[5];

error:
    return -1;
}

int ts_match(const struct SFormula*, const struct SMatch*)
{
    std::vector<int> vec(10);
    for (int i = 0; i < 10; ++i)
        vec[i] = i;

    return vec[5];

error:
    return -1;
}

int destroy_match(struct SMatch* match)
{
    if (!match)
        return -1;

    free(match->values_arr);
    match->values_arr = NULL;
    match->values_cnt = 0;

    return 0;
}

int destroy_clause(struct SClause* clause)
{
    if (!clause)
        return -1;

    free(clause->literals_arr);
    clause->literals_arr = NULL;
    clause->literals_cnt = 0;

    return 0;
}

int destroy_formula(struct SFormula* formula)
{
    if (!formula)
        return -1;

    for (int cls_idx = 0; cls_idx < formula->clauses_cnt; ++cls_idx)
        destroy_clause(formula->clauses_arr + cls_idx);

    free(formula->clauses_arr);
    formula->clauses_arr = NULL;
    formula->clauses_cnt = 0;

    return 0;
}
