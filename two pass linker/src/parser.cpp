#include "parser.hpp"

int Parser::readInt(Tokenizer& tokenizer) {
    string token = tokenizer.getToken();
    if (!isInt(token)) {
        MyException ex(ErrorCode::numExpected, tokenizer.getLineNumber(), tokenizer.getOffset() - token.size() + 1);
        throw (ex);
    }
    return stoi(token);
}

string Parser::readSymbol(Tokenizer& tokenizer) {
    string token;
    try {
        token = tokenizer.getToken();
    }
    catch (MyException& ex) {
        if (ex.getErrorCode() == ErrorCode::eof) {
            ex.setErrorCode(ErrorCode::symExpected);
            throw(ex);
        }
    }
    if (!isSymbol(token)) {
        MyException ex(ErrorCode::symExpected, tokenizer.getLineNumber(), tokenizer.getOffset() - token.size() + 1);
        throw (ex);
    }
    if (token.size() > MAX_SYMBOL_LEN) {
        MyException ex(ErrorCode::symTooLong, tokenizer.getLineNumber(), tokenizer.getOffset() - token.size() + 1);
        throw (ex);
    }
    return token;
}

char Parser::readIAER(Tokenizer& tokenizer) {
    string token;
    try {
        token = tokenizer.getToken();
    }
    catch(MyException& ex) {
        if (ex.getErrorCode() == ErrorCode::eof) {
            ex.setErrorCode(ErrorCode::addrExpected);
            throw(ex);
        }
    }
    if (!isAddrMode(token)) {
        MyException ex(ErrorCode::addrExpected);
        throw(ex);
    }
    return token[0];
}

string Parser::instToStr(int inst) {
    string inststr = to_string(inst);
    int zeros = 4 - inststr.size();
    for (int i = 1; i <= zeros; ++i)
        inststr = '0' + inststr;
    return inststr;
}

string Parser::memToStr(int mem) {
    string memstr = to_string(mem);
    int zeros = 3 - memstr.size();
    for (int i = 1; i <= zeros; ++i)
        memstr = '0' + memstr;
    return memstr;
}

bool Parser::isAlpha(char c) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
        return true;
    return false;
}

bool Parser::isNum(char c) {
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

bool Parser::isAddrMode(string token) {
    if (token.size() != 1)
        return false;
    if (token[0] == 'I' || token[0] == 'A' || token[0] == 'E' || token[0] == 'R')
        return true;
    return false;
}

bool Parser::isInt(string token) {
    if (token.size() == 0)
        return false;
    for (int i = 0; i < token.size(); ++i) {
        if (isAlpha(token[i])) {
            return false;
        }
    }
    return true;
}

bool Parser::isSymbol(string token) {
    if (token.size() == 0)
        return false;
    if (!isAlpha(token[0]))
        return false;
    for (int i = 1; i < token.size(); ++i) {
        if (!(isAlpha(token[i]) || isNum(token[i])))
            return false;
    }
    return true;
}