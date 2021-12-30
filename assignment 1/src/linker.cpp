#include "linker.hpp"

Linker::Linker(Parser& parser_) {
    totalInst = 0;
    instno = 0;
    parser = parser_;
}

void Linker::pass1(Tokenizer& tokenizer) {

    int moduleNumber = 0;
    bool eof = false;
    while (!eof) {
        try {
            pass1DefList(tokenizer, moduleNumber);
            pass1UseList(tokenizer);
            pass1ProgramText(tokenizer);
            ++moduleNumber;
        }
        catch (MyException& ex) {
            //ex.print();
            ErrorCode e = ex.getErrorCode();
            if (e == ErrorCode::eof)
                eof = true;
            else if (e == ErrorCode::symExpected || e == ErrorCode::numExpected || e == ErrorCode::tooManyInstr ||
                    e == ErrorCode::tooManyUse || e == ErrorCode::tooManyDef || e == ErrorCode::addrExpected || e == ErrorCode::symTooLong) {
                string message = ex.getErrCodeMessage();
                printf("%s\n", message.c_str());
                exit(EXIT_SUCCESS);
            }
        }
    }

    // setup base addresses after theg first pass
    setBaseAddr();

    // update the the symbol table after the first pass
    updateSymbolTableWrtBaseAddr();

    printSymbolTable();
}

void Linker::setBaseAddr() {
    if (baseAddr.size() > 0) {
        baseAddr.insert(baseAddr.begin(), 0);
    }
}

void Linker::updateSymbolTableWrtBaseAddr() {
    for (int i = 0; i < symTable.size(); ++i) {
        if (symTable[i].value >= baseAddr[symTable[i].moduleNum+1] - baseAddr[symTable[i].moduleNum]) {
            printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n",
            symTable[i].moduleNum + 1,
            symTable[i].identifier.c_str(),
            symTable[i].value, baseAddr[symTable[i].moduleNum+1] - baseAddr[symTable[i].moduleNum] - 1);
            symTable[i].value = (symTable[i].value / 1000) * 1000;
        }
        symTable[i].value += baseAddr[symTable[i].moduleNum];
    }
}

void Linker::printSymbolTable() {
    printf("Symbol Table\n");
    for (int i = 0; i < symTable.size(); ++i) {
        string message = "";
        if (symTable[i].multidef) {
            MyException ex(ErrorCode::multipleDefinition);
            message = ex.getErrCodeMessage();
        }
        printf(message == "" ? "%s=%d%s\n" : "%s=%d %s\n", symTable[i].identifier.c_str(), symTable[i].value, message.c_str());
    }
}

void Linker::pass1DefList(Tokenizer& tokenizer, int moduleNumber) {
    int defcount = parser.readInt(tokenizer);
    int defcountLineNum = tokenizer.getLineNumber();
    int defcountOffset = tokenizer.getOffset() - to_string(defcount).size() + 1;

    if (defcount > 16) {
        MyException ex(ErrorCode::tooManyDef, defcountLineNum, defcountOffset);
        throw(ex);
    }

    

    for (int i = 0; i < defcount; ++i) {
        //try {
            string sym = parser.readSymbol(tokenizer);
            int value = parser.readInt(tokenizer);
            createSymbol(sym, value, moduleNumber);
    }
}

void Linker::createSymbol(string identifier, int value, int moduleNum) {
    for (int i = 0; i < symTable.size(); ++i) {
        if (symTable[i].identifier == identifier) {
            symTable[i].multidef = true;
            return;
        }
    }
    symbol s(identifier, value, moduleNum);
    symTable.push_back(s);
}

void Linker::pass1UseList(Tokenizer& tokenizer) {
    int usecount = parser.readInt(tokenizer);

    int usecountLineNum = tokenizer.getLineNumber();
    int usecountOffset = tokenizer.getOffset() - to_string(usecount).size() + 1;
    for (int i = 0; i < usecount; ++i) {
        try {
            string sym = parser.readSymbol(tokenizer);
        }
        catch(MyException& ex) {
            if (ex.getErrorCode() == ErrorCode::symExpected && i < usecount-1) {
                ex.setErrorCode(ErrorCode::tooManyUse);
                ex.setLineNumber(usecountLineNum);
                ex.setOffset(usecountOffset);
                throw(ex);
            }
            if (ex.getErrorCode() == ErrorCode::symExpected)
                throw(ex);
        }
    }
}

void Linker::pass1ProgramText(Tokenizer& tokenizer) {

    int instcount = parser.readInt(tokenizer);
    totalInst += instcount;
    if (baseAddr.empty())
        baseAddr.push_back(instcount);
    else
        baseAddr.push_back(baseAddr[baseAddr.size()-1] + instcount);

    if (baseAddr[baseAddr.size()-1] > MACHINE_SIZE) {
        MyException ex(ErrorCode::tooManyInstr, tokenizer.getLineNumber(), tokenizer.getOffset() - to_string(instcount).size() + 1);
        throw(ex);
    }

    for (int i = 0; i < instcount; ++i) {
        char addressmode = parser.readIAER(tokenizer);
        int inst = parser.readInt(tokenizer);
    }
}

// ********************** PASS 2 ********************************

void Linker::pass2(Tokenizer& tokenizer) {

    printf("\nMemory Map\n");
    instno = 0;
    int moduleNumber = 0;
    bool eof = false;
    while (!eof) {
        try {
            pass2DefList(tokenizer);
            vector<pair<string, bool> > uselist = pass2UseList(tokenizer);
            pass2ProgramText(tokenizer, uselist, moduleNumber);
            printWarnings(uselist, moduleNumber + 1);
            ++moduleNumber;
        }
        catch (MyException ex) {
            //ex.print();
            eof = true;
        }
    }
}

void Linker::pass2DefList(Tokenizer& tokenizer) {

    int defcount = parser.readInt(tokenizer);
    for (int i = 0; i < defcount; ++i) {
        string sym = parser.readSymbol(tokenizer);
        int value = parser.readInt(tokenizer);
    }
}

vector<pair<string, bool> > Linker::pass2UseList(Tokenizer& tokenizer) {

    vector<pair<string, bool> > uselist;
    int usecount = parser.readInt(tokenizer);
    for (int i = 0; i < usecount; ++i) {
        string sym = parser.readSymbol(tokenizer);
        pair<string, bool> p;
        p.first = sym;
        p.second = false;
        uselist.push_back(p);
    }
    return uselist;
}

void Linker::pass2ProgramText(Tokenizer& tokenizer, vector<pair<string, bool> >& uselist, int moduleNumber) {

    int instcount = parser.readInt(tokenizer);
    for (int i = 0; i < instcount; ++i) {
        char addressmode = parser.readIAER(tokenizer);
        int inst = parser.readInt(tokenizer);
        string message = "";
        try {
            getGlobalAddr(addressmode, inst, uselist, moduleNumber);
        }
        catch (MyException& ex) {
            message = ex.getErrCodeMessage();
        }
        printMemLoc(instno, inst, message);
        ++instno;
    }
}

int Linker::getSymValue(string identifier) {
    for (int i = 0; i < symTable.size(); ++i) {
        if (symTable[i].identifier == identifier) {
            symTable[i].accessed = true;
            return symTable[i].value;
        }
    }
    return -1;
}

void Linker::getGlobalAddr(char addressmode, int& inst, vector<pair<string, bool> >& uselist, int moduleNumber) {

    if (addressmode == 'I') {
        if (inst / 1000 >= 10) {
            inst = 9999;
            MyException ex(ErrorCode::illegalImmediateValue);
            throw(ex);
        }
    }

    else if (addressmode == 'A') {
        checkForIllegalOpcode(inst);
        if (inst % 1000 >= MACHINE_SIZE) {
            inst = (inst / 1000) * 1000;
            MyException ex(ErrorCode::absAddrExceeds);
            throw(ex);
        }
    }

    else if (addressmode == 'E') {

        if (inst % 1000 >= uselist.size()) {
            MyException ex(ErrorCode::extAddrExceeds);
            throw(ex);
        }

        string identifier = uselist[inst % 1000].first;
        uselist[inst % 1000].second = true;
        int addr = getSymValue(identifier);
        if (addr == -1) {
            inst = ((inst / 1000) * 1000);
            MyException ex(ErrorCode::symNotDefined);
            string message = ex.getErrCodeMessage(identifier);
            ex.setErrCodeMessage(message);
            throw(ex);
        }
        inst = ((inst / 1000) * 1000) + addr;
    }

    else if (addressmode == 'R') {
        int rel = inst % 1000;
        inst = (inst / 1000) * 1000;
        checkForIllegalOpcode(inst);

        if (rel > baseAddr[moduleNumber+1] - baseAddr[moduleNumber]) {
            inst += baseAddr[moduleNumber];
            MyException ex(ErrorCode::relAddrExceeds);
            throw(ex);
        }
        inst += (rel + baseAddr[moduleNumber]);
    }
}

void Linker::checkForIllegalOpcode(int& inst) {
    if (inst / 1000 >= 10) {
        inst = 9999;
        MyException ex(ErrorCode::illegalOpcode);
        throw(ex);
    }
}

void Linker::printMemLoc(int mem, int inst, string message) {
    string memstr = parser.memToStr(mem);
    string inststr = parser.instToStr(inst);
    printf(message == "" ?  "%s: %s%s\n" : "%s: %s %s\n", memstr.c_str(), inststr.c_str(), message.c_str());
}

void Linker::printWarnings() {
    for (int i = 0; i < symTable.size(); ++i) {
        if (!symTable[i].accessed)
            printf("Warning: Module %d: %s was defined but never used\n", symTable[i].moduleNum+1, symTable[i].identifier.c_str());
    }
}

void Linker::printWarnings(vector<pair<string, bool> > uselist, int modnum) {
    for (int i = 0; i < uselist.size(); ++i) {
        pair<string, bool> p = uselist[i];
        if (p.second == false)
            printf("Warning: Module %d: %s appeared in the uselist but was not actually used\n", modnum, p.first.c_str());
    }
}
