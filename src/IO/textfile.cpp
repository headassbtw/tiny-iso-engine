#include <IO/textfile.hpp>
#include <iostream>
#include <fstream>
#include <string>
using namespace IO::TextFile;
std::string IO::TextFile::ReadAllLines(const char* location){
    std::ifstream inFile;
    inFile.open(location);
    std::string str;
    std::string file_contents;
    while (std::getline(inFile, str))
    {
        file_contents += str;
        file_contents.push_back('\n');
    }
    //std::cout << file_contents.c_str() << std::endl;
    return file_contents;
}
