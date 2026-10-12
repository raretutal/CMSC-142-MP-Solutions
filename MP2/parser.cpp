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
    
    
}