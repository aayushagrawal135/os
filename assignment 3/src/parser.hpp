#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <vector>
#include <fstream>
#include "process.hpp"

using namespace std;

class Parser {

    private:

        fstream reader;
        fstream rfreader;
        long getToken();

    public:

        Parser(char* fname, char* rfname);
        ~Parser();
        vector<Process> parseProcess();
        Instruction* getParsedInstr();
        int myrandom(int num);
};

#endif
