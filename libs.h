//
// Created by James on 2019/5/7.
//

#ifndef UCT_LIBS_H
#define UCT_LIBS_H

#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <string>
#include <cassert>
#include <cstring>

typedef char Buffer[256];

extern Buffer buffer;

extern std::string LOG_DIR;

extern const clock_t CLOCK_LIMIT;

extern std::ofstream fout;

#define debug(...) { sprintf(buffer, __VA_ARGS__); fout << buffer; }

#endif //UCT_LIBS_H
