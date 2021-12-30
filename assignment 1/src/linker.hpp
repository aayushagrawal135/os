#ifndef LINKER
#define LINKER

#include <string>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "parser.hpp"
#include "tokenizer.hpp"
#include "my_exception.hpp"

using namespace std;

const int MACHINE_SIZE = 512;

class Linker {

    private:

        struct symbol {
            symbol(string identifier_, int value_, int moduleNum_) {
                identifier = identifier_;
                value = value_;
                moduleNum = moduleNum_;
                accessed = false;
                multidef = false;
            }
            string identifier;
            int value;
            int moduleNum;
            bool accessed;
            bool multidef;
        };

        Parser parser;

        vector<symbol> symTable;
        unordered_map<string, pair<int, int> > symbolTable;
        vector<int> baseAddr;
        int totalInst;
        int instno;

        void pass1DefList(Tokenizer&, int);
        void pass1UseList(Tokenizer&);
        void pass1ProgramText(Tokenizer&);
        void setBaseAddr();
        void updateSymbolTableWrtBaseAddr();
        void printSymbolTable();

        void pass2DefList(Tokenizer&);
        vector<pair<string, bool> > pass2UseList(Tokenizer&);
        void pass2ProgramText(Tokenizer&, vector<pair<string, bool> >&, int);
        void getGlobalAddr(char, int&, vector<pair<string, bool> >&, int);
        void printMemLoc(int, int, string);

        void checkForIllegalOpcode(int&);

    public:

        Linker(Parser&);
        void pass1(Tokenizer&);
        void pass2(Tokenizer&);
        void createSymbol(string, int, int);
        int getSymValue(string);
        void printWarnings();
        void printWarnings(vector<pair<string, bool> >, int); 
};

#endif
