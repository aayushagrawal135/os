#ifndef TOKENIZER
#define TOKENIZER

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "my_exception.hpp"

using namespace std;

class Tokenizer {

    private:

        fstream reader;
        unsigned int lineNumber;
        unsigned int offset;
        unsigned int current;
        string currentLine;
        bool tokenStarted;

        bool isDelimeter(char);

    public:
        Tokenizer(char* filename);
        ~Tokenizer();
        string getToken();
        void reset();
        int getLineNumber();
        int getOffset();
};

#endif
