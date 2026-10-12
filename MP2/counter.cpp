#include "counter.h"
#include "trimmer.h"
#include <cctype>

void Complexity::add(const Complexity& other){
    constantOps += other.constantOps;
    for (const auto& pair : other.terms){
        terms[pair.first] += pair.second;
    }
}

std::string Complexity::toString() const {
    std::string result = "T(n)=";
    bool hasTerms = false;

    for (const auto& pair : terms){
        if (pair.second == 0) continue;
            if (hasTerms) result += "+";

            if (pair.second == 1) {
                result += pair.first;
            } else {
                result +=  std::to_string(pair.second) + pair.first;
            }
            hasTerms = true;
    }
    if (constantOps > 0) {
        result += std::to_string(constantOps);
    } else if (!hasTerms) {
        result += "0";
    }
    return result;
}

int countStatementOps(const std::string& stmt){
    int count = 0;
    size_t i = 0;

    while (i < stmt.length()){
        // Checking two-character operators first (because they count as 1 by convention)
        if (i + 1 < stmt.length()) {
            std::string op2 = stmt.substr(i, 2);
            if (op2 == "++" || op2 == "--" || op2 == "+=" || op2 == "-=" ||
                op2 == "*=" || op2 == "/=" || op2 == "%=" || op2 == "<=" ||
                op2 == ">=" || op2 == "==" || op2 == "!=" || op2 == "<<" ||
                op2 == ">>") {
                count++;
                i += 2;
                continue;
            }
        }
        char c = stmt[i];

        // Check single-character operators
        if (c == '+' || c == '*' || c == '/' || c == '%' || c == '<' || c == '>' || c == '=') {
            count++;
            i++;
            continue;
        }

    }
}