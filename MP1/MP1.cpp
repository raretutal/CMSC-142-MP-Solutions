#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void readFile(string, vector<string>&);

int main(){
    string fileName;
    vector<string> existingFiles;
    cin >> fileName;
    readFile(fileName, existingFiles);
}

void readFile(string file, vector<string>& allFiles){
    ifstream reader(file);
    
    if(reader.is_open()){
        string line, token;
        
        while(getline(reader, line)){
            stringstream ss(line);
            ss >> token;

            if(token.compare("#include") == 0){
                ss >> token;
                string newFile = token.substr(1,token.length() - 2);

                bool alreadyExists = false;
                for(const string& f : allFiles){
                    if(newFile == f){
                        alreadyExists = true;
                    }
                }
                if(alreadyExists) continue;

                readFile(newFile, allFiles);
                allFiles.push_back(newFile);
                continue;
            }
        cout << line << endl;
        }
    }
    reader.close();
};


