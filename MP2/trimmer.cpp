#include <string>
#include <vector>

// Removes all leading or trailing whitespaces and semicolons
std::string trim(const std::string& str){
    size_t first = str.find_first_not_of(" \t\r\n");
    if(std::string::npos == first) return "";
    size_t last = str.find_last_not_of(" \t\r\n;");

    return str.substr(first,(last - first + 1));
}

// Removes primitive datatypes
std::string stripTypes(std::string line){
    std::vector<std::string> types = {"int ", "float ", "double ", "char ", "long "};
    for(const std::string& type: types){
        if(line.find(type) == 0){
            return trim(line.substr(type.length()));
        }
    }
    return line;
}
