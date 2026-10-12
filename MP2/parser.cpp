#include "parser.h"
#include "trimmer.h"

// Moves forward pos past any whitespace characters
void skipSpaces(const std::string& str, size_t& pos){
    while (pos < str.length() && (str[pos] == ' ' || str[pos] == '\t' ||
    str[pos] == '\n' || str[pos] == '\r')){
        pos++;
    }
}

// Checks if a specific word starts at the current position
bool isKeyword(const std::string& str, size_t pos, const std::string& word){
    if(pos + word.length() > str.length()) return false;
    if(str.substr(pos, word.length()) != word) return false;
    
    // Making sure that keyword is not just a prefix of a variable name (because yes)
    size_t nextPos = pos + word.length();
    if (nextPos < str.length()){
        char c = str[nextPos];
        bool isLetterOrDigit = (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_');
        if (isLetterOrDigit) return false;
    }
    return true;
}

// Finds the matching closing character (for thingies with parentheses or curly braces)
size_t findClosing(const std::string& str, size_t openPos, char openChar, char closeChar){
    int balance = 0;
    for(size_t i = openPos; i < str.length(); i++){
        if (str[i] == openChar) balance++;
        if (str[i] == closeChar) balance--;
        if (balance == 0) return i;
    }
    return std::string::npos;
}

ParsedProgram parseString(const std::string& code);
ASTNode parseOne(const std::string& code, size_t& pos);

// Parsing a for-loop header and its body
ASTNode parseFor(const std::string& code, size_t& pos){
    ASTNode node;
    node.type = NodeType::FOR_LOOP;

    pos += 3; // Skipping "for"
    skipSpaces(code, pos);

    size_t openParen = code.find('(', pos);
    size_t closeParen = findClosing(code, openParen, '(', ')');

    std::string header = code.substr(openParen + 1, closeParen - openParen - 1);
    size_t semi1 = header.find(';');
    size_t semi2 = header.find(':', semi1 + 1);

    node.init = stripTypes(trim(header.substr(0, semi1)));
    node.condition = trim(header.substr(semi1 + 1, semi2 - semi1 - 1));
    node.update = trim(header.substr(semi2 + 1));

    pos = closeParen + 1;
    skipSpaces(code, pos);

    
}