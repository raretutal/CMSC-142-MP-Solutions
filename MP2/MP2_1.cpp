#include <iostream>
#include <string>
#include <sstream>
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

void parseBlock(const std::vector<std::string>& code, int& index, std::string context, bool singleStatement = false){
    // Replaced printedStatements with needsHeader to track scope returns
    bool needsHeader = (context == "global");

    while (index < code.size()){
        std::string raw_line = code[index];
        std::string line = trim(raw_line);

        if (line.empty()){
            index++;
            continue;
        }

        // Scope boundaries
        if (line == "}"){
            index++;
            break;
        }
        if (line == "{"){
            index++;
            continue;
        }

        // Parse for-loop using the goated stringstream to get 3 parts
        if (line.find("for") == 0){ // Fixed line.find("for" == 0) bug
            std::cout << "for:" << std::endl;
            size_t start = line.find("(");
            size_t end = line.rfind(")");
            std::string content = line.substr(start + 1, end - start - 1);

            std::stringstream ss(content);
            std::string part;
            std::vector<std::string> parts;

            // Tokenize loop components separated by semicolons ;
            while(getline(ss, part, ';')){
                parts.push_back(trim(part));
            }

            std::cout << "initializer: " << stripTypes(parts[0]) << "\n";
            if (parts.size() > 1){
                std::cout << "condition: " << parts[1] << std::endl;
            }
            if (parts.size() > 2){
                std::cout << "update: " << parts[2] << std::endl;
            }
            std::cout << "for statements:" << std::endl;

            index++;
            bool hasBrace = (line.back() == '{');
            if (hasBrace){
                parseBlock(code, index, "for");
            } else{
                std::string nextLine = index < code.size() ? trim(code[index]): "";
                if(nextLine == "{"){
                    index++;
                    parseBlock(code, index, "for");

                } else {
                    parseBlock(code, index, "for", true);
                }
            }
            needsHeader = true; 
            if (singleStatement) break;
        }
        else if (line.find("if") == 0){ // parse if-statement
            std::cout << "if:" << std::endl;
            size_t start = line.find("(");
            size_t end = line.rfind(")");
            std::string condition = line.substr(start + 1, end - start - 1);
            std::cout << "condition: " << condition << std::endl;
            std::cout << "if statements:" << std::endl;

            index++;
            bool hasBrace = (line.back() == '{');
            if (hasBrace){
                parseBlock(code, index, "if");
            } else {
                std::string nextLine = index < code.size() ? trim(code[index]) : "";
                if (nextLine == "{"){
                    index++;
                    parseBlock(code, index, "if");
                } else {
                    parseBlock(code, index, "if", true);
                }
            }
            needsHeader = true; 
            if (singleStatement) break;
        }
        else if (line.find("else") == 0) { // parse else-statement
            std::cout << "else statements:" << std::endl;
            index++;
            bool hasBrace = (line.back() == '{');
            if (hasBrace) {
                parseBlock(code, index, "else");
            } else {
                std::string nextLine = index < code.size() ? trim(code[index]) : "";
                if (nextLine == "{") {
                    index++;
                    parseBlock(code, index, "else");
                } else {
                    parseBlock(code, index, "else", true);
                }
            }
            needsHeader = true;
            if (singleStatement) break;
        }
        else {
            if (needsHeader) {
                if (context == "global") std::cout << "statements:" << std::endl;
                else if (context == "for") std::cout << "for statements continued:" << std::endl;
                else if (context == "if") std::cout << "if statements continued:" << std::endl;
                else if (context == "else") std::cout << "else statements continued:" << std::endl;
                needsHeader = false;
            }

            line = stripTypes(line);
            std::stringstream ss(line);
            std::string token;

            // Tokenize comma-separated variables
            while (getline(ss, token, ',')){ 
                std::string stmt = trim(token);
                // Print only if it performs an assignment or I/O operation fakk
                if (stmt.find("=") != std::string::npos ||
                    stmt.find(">>") != std::string::npos ||
                    stmt.find("<<") != std::string::npos ||
                    stmt.find("++") != std::string::npos ||
                    stmt.find("--") != std::string::npos){
                    std::cout << stmt << std::endl;
                }
            }
            index++;
            if (singleStatement) break;
        }
    }
}

int main(){

    std::vector<std::string> raw_code;
    std::string current_line;
    int n_lines;

    if(!(std::cin >> n_lines)) return 0;
    std::cin.ignore(); // fuckass endline character

    for(int i = 1; i <= n_lines; i++){
        std::getline(std::cin, current_line);
        raw_code.push_back(current_line);
    }

    // Preprocessor: Splits minified code by semicolons and scope braces
    std::vector<std::string> code;
    int paren_count = 0;
    std::string temp = "";
    
    for (const std::string& raw : raw_code) {
        for (char c : raw) {
            if (c == '(') paren_count++;
            else if (c == ')') paren_count--;

            temp += c;

            // Only split if we are outside of parentheses
            if (paren_count == 0) {
                if (c == ';' || c == '{' || c == '}') {
                    if (c == '{' || c == '}') {
                        std::string part = temp.substr(0, temp.length() - 1);
                        if (!trim(part).empty()) code.push_back(trim(part));
                        code.push_back(std::string(1, c));
                        temp = "";
                    } else { // Handle semicolons
                        if (!trim(temp).empty()) code.push_back(trim(temp));
                        temp = "";
                    }
                } else if (c == ')') {
                    std::string t = trim(temp);
                    // Split immediately after ')' if it's a control structure
                    if (t.find("if") == 0 || t.find("for") == 0 || t.find("else if") == 0) {
                        code.push_back(t);
                        temp = "";
                    }
                }
            }
        }
        temp += " ";
    }
    if (!trim(temp).empty()) {
        code.push_back(trim(temp));
    }

    int startIndex = 0;
    parseBlock(code, startIndex, "global");

    /*  TESST PRINT FOR ALL LINES OF CODE
    for(int i = 0; i < code.size(); i++){
        std::cout << code[i] << std::endl;
    } 
    */
    
    return 0;
}