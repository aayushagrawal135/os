#ifndef MY_EXCEPTION
#define MY_EXCEPTION

#include <iostream>
#include <string>

using namespace std;

enum ErrorCode {
    absAddrExceeds,
    multipleDefinition,
    symNotDefined,
    relAddrExceeds,
    extAddrExceeds,
    symExpected,
    eof,
    numExpected,
    tooManyInstr,
    illegalOpcode,
    illegalImmediateValue,
    tooManyUse,
    tooManyDef,
    addrExpected,
    symTooLong
};

class MyException : public std::exception {

    int lineNumber;
    int offset;
    ErrorCode errorCode;
    string message;

    public:

    MyException(int, int);
    MyException(ErrorCode);
    MyException(ErrorCode, int, int);
    void print();
    ErrorCode getErrorCode();
    void setErrorCode(ErrorCode);
    string getErrCodeMessage();
    string getErrCodeMessage(string);
    void setErrCodeMessage();
    void setErrCodeMessage(string);
    void setLineNumber(int);
    void setOffset(int);
};

#endif
