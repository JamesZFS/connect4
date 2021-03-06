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
#include <cstring>

template<typename T>
using List = std::vector<T>;

inline size_t randint(size_t x)
{
	return rand() % x;
}

//unsigned int round_cnt = 0;
//char __buffer__[256];
//std::string LOG_DIR = "/Users/james/Test/AI/Chess/Compete/logs/";
//std::string LOG_DIR = "/Users/james/Test/AI/Chess/Strategy/logs/";
//
//std::ofstream fout;
//
//#define openLogger { sprintf(__buffer__, "%s/move - %d.txt", LOG_DIR.data(), round_cnt); fout.open(__buffer__); if (!fout.is_open()) exit(1); }
//
//#define closeLogger { fout.close(); }
//
//#define logger(...) { sprintf(__buffer__, __VA_ARGS__); fout << __buffer__; }
//
//#define debug(...) { sprintf(__buffer__, __VA_ARGS__); fout << __buffer__; fout.flush(); }
//
//#define assure(x, y) { if (!(x)) { debug(y) closeLogger exit(1); } }
#define logger(...)
#define assure(x, y)

#endif //UCT_LIBS_H
