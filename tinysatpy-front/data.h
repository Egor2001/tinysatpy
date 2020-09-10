#ifndef DATA_H_
#define DATA_H_

struct SFormula
{
    struct SClause* clauses_arr;
    int clauses_cnt;
};

struct SClause
{
    int* literals_arr;
    int literals_cnt;
};

struct SMatch
{
    int* values_arr;
    int values_cnt;
};

#endif // DATA_H_
