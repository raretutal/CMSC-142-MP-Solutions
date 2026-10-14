#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

enum class NodeType {
    STATEMENT, 
    IF_BLOCK,
    FOR_LOOP
};

struct ASTNode {
    NodeType type;
    std::string text; // for raw clean expressions

    // components for for() header 
    std::string init; 
    std::string condition;
    std::string update;

    
    std::vector<ASTNode> body; // storing statements inside a for loop / any 'then' branch of an an 'if' statement
    std::vector <ASTNode> elseBody;

    
};

using ParsedProgram = std::vector<ASTNode>;

ParsedProgram parseCode(const std::vector<std::string>& lines);

#endif