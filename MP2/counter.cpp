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
        // Handle '-' and differentiate from negative number literals
        if (c == '-') {
            bool isNegativeLiteral = false;
            if (i + 1 < stmt.length() && std::isdigit(static_cast<unsigned char>(stmt[i + 1]))) {
                int prev = static_cast<int>(i) - 1;
                while (prev >= 0 && (stmt[prev] == ' ' || stmt[prev] == '\t')) {
                    prev--;
                }
                if (prev < 0 || stmt[prev] == '=' || stmt[prev] == '(' || stmt[prev] == ',') {
                    isNegativeLiteral = true;
                }
            }
            if (!isNegativeLiteral) {
                count++;
            }
            i++;
            continue;
        }
        i++;
    }
    return count;
}

void analyzeLoop(const ASTNode& loopNode, std::string& termName, int& step){
    step = 1;

    // Check condition for root terms
    int mulInCond = 0;
    for (char c : loopNode.condition) {
        if (c == '*') mulInCond++;
    }
    
    if(mulInCond >= 2) {
        termName = "cubert(n)";
        return;
    }
    if (mulInCond == 1) {
        termName = "sqrt(n)";
        return;
    }
    
    // Check update for logarithmic step
    if (loopNode.update.find("*=") != std::string::npos) {
        size_t pos = loopNode.update.find("*=");
        std::string baseStr = trim(loopNode.update.substr(pos + 2));
        termName = "log(" + baseStr + ")n";
        return;
    }
    if (loopNode.update.find("/=") != std::string::npos) {
        size_t pos = loopNode.update.find("/=");
        std::string baseStr = trim(loopNode.update.substr(pos + 2));
        termName = "log(" + baseStr + ")n";
        return;
    }

    // Check update for linear step size
    if (loopNode.update.find("+=") != std::string::npos) {
        size_t pos = loopNode.update.find("+=");
        std::string stepStr = trim(loopNode.update.substr(pos + 2));
        try {
            int s = std::stoi(stepStr);
            if (s > 0) step = s;
        } catch (...) {
            step = 1;
        }
    }

    termName = "n";
}