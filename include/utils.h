#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <array>
#include <memory>
#include <cstdio>
#include <string.h>
#include <fstream>
#include <unistd.h>

#include "Model.h"

namespace XAIPasS{

struct Double{
public:
    int i;
    double d;
    Double(){}; 

    friend bool operator==(const Double& d1, const Double& d2);
};

template <typename T> std::string NumberToString (T Number);

std::vector<std::string> runCommand(const std::string & cmd);
std::string getFrameworkPath();
std::string getExePath();
static inline void ltrim(std::string &s);
static inline void rtrim(std::string &s);
static inline void trim(std::string &s);
void writeStringsToConsole(std::vector<std::string> strings);
void writeStringsToFile( const std::string &  file, std::vector<std::string> strings);
bool double_equals(double a, double b, double epsilon = 0.0001);
bool removeFile(const std::string& path);
bool string_replace(std::string& str, const std::string& from, const std::string& to);
}

#endif // UTILS_H
