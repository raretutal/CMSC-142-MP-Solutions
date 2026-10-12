#include "counter.h"
#include "trimmer.h"
#include <cctype>
#include <cmath>
#include <cstdlib>

void Complexity::add(const Complexity& other){
    constantOps += other.constantOps;
    for (const auto& pair : other.terms){
        terms[pair.first] += pair.second;
    }
}

namespace {

std::string formatTerm(const std::string& name, int coeff, bool isFirst) {
    std::string s = "";
    int absCoeff = std::abs(coeff);

    if (coeff < 0) {
        s += isFirst ? "-" : " - ";
    } else {
        if (!isFirst) s += " + ";
    }

    if (name.rfind("log", 0) == 0) {
        if (absCoeff == 1) {
            s += name;
        } else {
            s += std::to_string(absCoeff) + " " + name;
        }
    } else if (name == "sqrt(n)" || name == "cubert(n)") {
        if (absCoeff == 1) {
            s += name;
        } else {
            s += std::to_string(absCoeff) + " " + name;
        }
    } else if (name == "n") {
        if (absCoeff == 1) {
            s += "n";
        } else {
            s += std::to_string(absCoeff) + "n";
        }
    } else if (name.find('/') != std::string::npos) {
        // For fractional term (e.g., "8n/5", "9n/4"), already includes its coefficient
        s += name;
    } else {
        // For Secondary variable terms (e.g., "m", "x", "y", "z")
        if (absCoeff == 1) {
            s += name;
        } else {
            s += std::to_string(absCoeff) + name;
        }
    }
    return s;
}

void parseInitExpr(const std::string& initStr, std::string& startVar, int& startConst, bool& hasN) {
    startVar = "";
    startConst = 0;
    hasN = false;

    size_t eqPos = initStr.find('=');
    if (eqPos == std::string::npos) return;

    std::string rhs = trim(initStr.substr(eqPos + 1));
    if (rhs.find('n') != std::string::npos) {
        hasN = true;
    }

    std::string var = "";
    for (char c : rhs) {
        if (std::isalpha(static_cast<unsigned char>(c)) && c != 'n') {
            var += c;
        }
    }
    startVar = var;

    if (!startVar.empty()) {
        size_t minusPos = rhs.find('-');
        size_t plusPos = rhs.find('+');
        if (minusPos != std::string::npos) {
            try { startConst = -std::stoi(trim(rhs.substr(minusPos + 1))); } catch (...) { startConst = 0; }
        } else if (plusPos != std::string::npos) {
            try { startConst = std::stoi(trim(rhs.substr(plusPos + 1))); } catch (...) { startConst = 0; }
        } else {
            startConst = 0;
        }
    } else if (!hasN) {
        try { startConst = std::stoi(rhs); } catch (...) { startConst = 0; }
    }
}

} // namespace

std::string Complexity::toString() const {
    if (terms.count("infinite") && terms.at("infinite") > 0) {
        return "infinite";
    }

    std::string result = "T(n) = ";
    bool hasTerms = false;

    // 1. Primary terms containing 'n' (n, fractions of n, log, sqrt, cubert)
    for (const auto& pair : terms) {
        if (pair.second == 0 || pair.first == "infinite") continue;
        if (pair.first.find('n') != std::string::npos) {
            result += formatTerm(pair.first, pair.second, !hasTerms);
            hasTerms = true;
        }
    }

    // 2. Secondary variable terms (m, x, y, z)
    for (const auto& pair : terms) {
        if (pair.second == 0 || pair.first == "infinite") continue;
        if (pair.first.find('n') == std::string::npos) {
            result += formatTerm(pair.first, pair.second, !hasTerms);
            hasTerms = true;
        }
    }

    // 3. Constant term
    if (hasTerms) {
        if (constantOps > 0) {
            result += " + " + std::to_string(constantOps);
        } else if (constantOps < 0) {
            result += " - " + std::to_string(-constantOps);
        }
    } else {
        result += std::to_string(constantOps);
    }

    return result;
}

int countStatementOps(const std::string& stmt){
    int count = 0;
    size_t i = 0;

    while (i < stmt.length()){
        // Skip string and character literals
        if (stmt[i] == '"' || stmt[i] == '\'') {
            char quote = stmt[i++];
            while (i < stmt.length() && stmt[i] != quote) {
                if (stmt[i] == '\\' && i + 1 < stmt.length()) i++;
                i++;
            }
            if (i < stmt.length()) i++;
            continue;
        }

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
                if (prev < 0 || stmt[prev] == '=' || stmt[prev] == '(' || stmt[prev] == ',' ||
                    stmt[prev] == '[' || stmt[prev] == '+' || stmt[prev] == '-' || stmt[prev] == '*' ||
                    stmt[prev] == '/' || stmt[prev] == '%' || stmt[prev] == '<' || stmt[prev] == '>' ||
                    stmt[prev] == '!') {
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
        termName = "log(" + baseStr + ") n";
        return;
    }
    if (loopNode.update.find("/=") != std::string::npos) {
        size_t pos = loopNode.update.find("/=");
        std::string baseStr = trim(loopNode.update.substr(pos + 2));
        termName = "log(" + baseStr + ") n";
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
    } else if (loopNode.update.find("-=") != std::string::npos) {
        size_t pos = loopNode.update.find("-=");
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

Complexity calculateNodeCost(const ASTNode& node) {
    Complexity comp;

    if (node.type == NodeType::STATEMENT) {
        comp.constantOps = countStatementOps(node.text);
        return comp;
    }

    if (node.type == NodeType::IF_BLOCK) {
        int condOps = countStatementOps(node.condition);

        Complexity thenComp = calculateComplexity(node.body);
        Complexity elseComp = calculateComplexity(node.elseBody);

        long long thenScore = thenComp.constantOps;
        for (const auto& pair : thenComp.terms) thenScore += pair.second * 10000000LL;

        long long elseScore = elseComp.constantOps;
        for (const auto& pair : elseComp.terms) elseScore += pair.second * 10000000LL;

        if (thenScore >= elseScore) {
            comp = thenComp;
        } else {
            comp = elseComp;
        }

        comp.constantOps += condOps;
        return comp;
    }

    if (node.type == NodeType::FOR_LOOP) {
        int initOps = countStatementOps(node.init);
        int condOps = countStatementOps(node.condition);
        int updateOps = countStatementOps(node.update);

        Complexity bodyComp = calculateComplexity(node.body);

        int overhead = initOps + condOps;
        int perIterOps = condOps + updateOps + bodyComp.constantOps;

        std::string startVar = "";
        int startConst = 0;
        bool hasNInInit = false;
        parseInitExpr(node.init, startVar, startConst, hasNInInit);

        // Case 1: Iterator starts at 'n' and condition checks <= or < with a small number (0 iterations)
        if (hasNInInit && (node.condition.find("<=") != std::string::npos ||
                           node.condition.find('<') != std::string::npos)) {
            comp.constantOps = overhead;
            return comp;
        }

        // Case 2: Infinite loops (decrementing update while condition checks <= or < against 'n')
        bool isDec = (node.update.find("--") != std::string::npos ||
                      node.update.find("-=") != std::string::npos);
        if (isDec && (node.condition.find("<=") != std::string::npos ||
                      node.condition.find('<') != std::string::npos)) {
            if (!hasNInInit) {
                comp.terms["infinite"] = 1;
                return comp;
            }
        }

        // Case 3: Loop with constant bounds (neither init nor condition contains 'n')
        if (node.condition.find('n') == std::string::npos && !hasNInInit) {
            bool isLeOrGe = (node.condition.find("<=") != std::string::npos ||
                             node.condition.find(">=") != std::string::npos);
            size_t opPos = node.condition.find_first_of("<>");
            int limit = 0;
            if (opPos != std::string::npos) {
                try {
                    limit = std::stoi(trim(node.condition.substr(opPos + (isLeOrGe ? 2 : 1))));
                } catch (...) {
                    limit = 0;
                }
            }

            int step = 1;
            if (node.update.find("+=") != std::string::npos) {
                try { step = std::stoi(trim(node.update.substr(node.update.find("+=") + 2))); } catch (...) {}
            } else if (node.update.find("-=") != std::string::npos) {
                try { step = std::stoi(trim(node.update.substr(node.update.find("-=") + 2))); } catch (...) {}
            }
            if (step <= 0) step = 1;

            int iterations = isLeOrGe ? (std::abs(limit - startConst) + 1) / step
                                      : std::abs(limit - startConst) / step;

            comp.constantOps = overhead + iterations * perIterOps;
            return comp;
        }

        // Case 4: Non-linear complexity (sqrt, cubert, log)
        std::string termName;
        int step = 1;
        analyzeLoop(node, termName, step);

        if (termName.rfind("log", 0) == 0) {
            comp.terms[termName] = perIterOps;
            comp.constantOps = overhead + perIterOps;
            return comp;
        }

        if (termName == "sqrt(n)" || termName == "cubert(n)") {
            comp.terms[termName] = perIterOps;
            comp.constantOps = overhead;
            return comp;
        }

        // Case 5: Linear loops (with 'n' and potential variable bounds)
        bool isLe = (node.condition.find("<=") != std::string::npos ||
                     node.condition.find(">=") != std::string::npos);
        int inclusiveOffset = isLe ? 1 : 0;

        int nOffset = 0;
        size_t nPos = node.condition.find('n');
        if (nPos != std::string::npos) {
            size_t plusPos = node.condition.find('+', nPos);
            size_t minusPos = node.condition.find('-', nPos);
            if (plusPos != std::string::npos) {
                try { nOffset = std::stoi(trim(node.condition.substr(plusPos + 1))); } catch (...) {}
            } else if (minusPos != std::string::npos) {
                try { nOffset = -std::stoi(trim(node.condition.substr(minusPos + 1))); } catch (...) {}
            }
        }

        int totalConstantShift = nOffset - startConst + inclusiveOffset;

        if (step == 1) {
            comp.terms["n"] = perIterOps;
            if (!startVar.empty()) {
                comp.terms[startVar] = -perIterOps;
            }
            comp.constantOps = overhead + perIterOps * totalConstantShift;
        } else {
            // Fraction formatting for linear steps > 1
            if (perIterOps % step == 0) {
                comp.terms["n"] = perIterOps / step;
            } else {
                comp.terms[std::to_string(perIterOps) + "n/" + std::to_string(step)] = 1;
            }
            if (!startVar.empty()) {
                comp.terms[startVar] = -perIterOps / step;
            }
            // For step > 1, overhead is not modified by start constant shift
            comp.constantOps = overhead;
        }

        return comp;
    }

    return comp;
}

Complexity calculateComplexity(const ParsedProgram& program){
    Complexity total;
    for (const auto& node : program) {
        total.add(calculateNodeCost(node));
    }
    return total;
}