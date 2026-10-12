#ifndef COUNTER_H
#define COUNTER_H

#include <string>
#include <map>
#include "parser.h"

struct Complexity {
    int constantOps = 0; // for keeping track of all operations executed outside loops, plus loop entry/exit conditions
    std::map<std::string, int> terms; // Maps variable terms like "n" to their integer multiplier :)

    void add(const Complexity& other); // for merging another complexity object into this one
    std::string toString() const; // for foarmatting final answer into T(n) = ... something like that 

};

int countStatementOps(const std::string& stmt);
Complexity calculateComplexity(const ParsedProgram& program);

#endif