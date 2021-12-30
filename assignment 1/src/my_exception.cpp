#include "my_exception.hpp"

MyException::MyException(int lineNumber_, int offset_) {
    lineNumber = lineNumber_;
    offset = offset_;
}

MyException::MyException(ErrorCode errorCode_, int lineNumber_, int offset_) {
    errorCode = errorCode_;
    lineNumber = lineNumber_;
    offset = offset_;
    setErrCodeMessage();
}

MyException::MyException(ErrorCode errorCode_) {
    errorCode = errorCode_;
    setErrCodeMessage();
}

void MyException::setErrCodeMessage() {
    if (errorCode == ErrorCode::absAddrExceeds)
        message = "Error: Absolute address exceeds machine size; zero used";
    else if (errorCode == ErrorCode::multipleDefinition)
        message = "Error: This variable is multiple times defined; first value used";
    else if (errorCode == ErrorCode::symNotDefined)
        message = "Error: {} is not defined; zero used";
    else if (errorCode == ErrorCode::relAddrExceeds)
        message = "Error: Relative address exceeds module size; zero used";
    else if (errorCode == ErrorCode::extAddrExceeds)
        message = "Error: External address exceeds length of uselist; treated as immediate";
    else if (errorCode == ErrorCode::symExpected)
        message = "Parse Error line " + to_string(lineNumber) + " offset " + to_string(offset) + ": SYM_EXPECTED";
    else if (errorCode == ErrorCode::eof)
        message = "EOF reached";
    else if (errorCode == ErrorCode::numExpected)
        message = "Parse Error line " + to_string(lineNumber) + " offset " + to_string(offset) + ": NUM_EXPECTED";
    else if (errorCode == ErrorCode::tooManyInstr)
        message = "Parse Error line " + to_string(lineNumber) + " offset " + to_string(offset) + ": TOO_MANY_INSTR";
    else if (errorCode == ErrorCode::illegalOpcode)
        message = "Error: Illegal opcode; treated as 9999";
    else if (errorCode == ErrorCode::illegalImmediateValue)
        message = "Error: Illegal immediate value; treated as 9999";
    else if (errorCode == ErrorCode::tooManyUse)
        message = "Parse Error line " + to_string(lineNumber) + " offset " + to_string(offset) + ": TOO_MANY_USE_IN_MODULE";
    else if (errorCode == ErrorCode::tooManyDef)
        message = "Parse Error line " + to_string(lineNumber) + " offset " + to_string(offset) + ": TOO_MANY_DEF_IN_MODULE";
    else if (errorCode == ErrorCode::addrExpected)
        message = "Parse Error line " + to_string(lineNumber) + " offset " + to_string(offset) + ": ADDR_EXPECTED";
    else if (errorCode == ErrorCode::symTooLong)
        message = "Parse Error line " + to_string(lineNumber) + " offset " + to_string(offset) + ": SYM_TOO_LONG";
    else
        message = "";
}

void MyException::setErrCodeMessage(string message_) {
    message = message_;
}

string MyException::getErrCodeMessage() {
    return message;
}

string MyException::getErrCodeMessage(string param) {
    string result = "";
    string m = getErrCodeMessage();
    int start = m.find_first_of('{');
    result = result + m.substr(0, start) + param;
    int end = m.find_first_of("}", start + 1);
    result = result + m.substr(end + 1);
    return result;
}

ErrorCode MyException::getErrorCode() {
    return errorCode;
}

void MyException::setErrorCode(ErrorCode errorCode_) {
    errorCode = errorCode_;
    setErrCodeMessage();
}

void MyException::setLineNumber(int lineNumber_) {
    lineNumber = lineNumber_;
    setErrCodeMessage();
}

void MyException::setOffset(int offset_) {
    offset = offset_;
    setErrCodeMessage();
}

void MyException::print() {
    printf("Line: %d, Offset: %d\n", lineNumber, offset);
}
