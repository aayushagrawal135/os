#ifndef PARSER
#define PARSER

#include <string>
#include <iostream>

#include "tokenizer.hpp"

using namespace std;

const int MAX_SYMBOL_LEN = 16;

class Parser {

    private:

        bool isAlpha(char);
        bool isAddrMode(string);
        bool isInt(string);
        bool isSymbol(string);
        bool isNum(char);

    public:
        int readInt(Tokenizer&);
        string readSymbol(Tokenizer&);
        char readIAER(Tokenizer&);
        string instToStr(int);
        string memToStr(int);
};

#endif
