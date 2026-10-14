#include <iostream>
#include <vector>
#include <string>
#include "parser.h"
#include "counter.h"

int main() {
    int numLines = 0;
    if (!(std::cin >> numLines)) {
        return 0;
    }

    // Discard sht ahh newline remaining in input buffer (I hate you sm frfr)
    std::cin.ignore();

    std::vector<std::string> lines;
    for (int i = 0; i < numLines; i++) {
        std::string line;
        if (std::getline(std::cin, line)) {
            lines.push_back(line);
        }
    }

    ParsedProgram program = parseCode(lines);
    Complexity complexity = calculateComplexity(program);

    std::cout << complexity.toString() << std::endl;

    return 0;
}