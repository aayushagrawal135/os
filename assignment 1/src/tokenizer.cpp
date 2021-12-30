#include "tokenizer.hpp"

Tokenizer::Tokenizer(char* filename) {
    reader.open(filename);
    offset = -1;
    current = -1;
    lineNumber = 0;
    tokenStarted = false;
}

Tokenizer::~Tokenizer() {
    reader.close();
}

int Tokenizer::getLineNumber() {
    return lineNumber;
}

int Tokenizer::getOffset() {
    return offset;
}

string Tokenizer::getToken() {

    if (current == -1) {
        if (getline(reader, currentLine)) {
            ++lineNumber;
            current = 0;
            offset = -1;
            tokenStarted = false;
        }
        else {
            MyException ex(ErrorCode::eof, lineNumber, offset == -1 ? 1 : offset + 1);
            throw ex;
        }
    }

    for (; current < currentLine.size(); ++current) {
        if (!isDelimeter(currentLine[current]) && !tokenStarted) {
            offset = current;
            tokenStarted = true;
        }

        // delimeter detected and we have a token
        else if (isDelimeter(currentLine[current]) && tokenStarted) {
            string token = currentLine.substr(offset, current - offset);
            // printf("Token: %u:%u : %s\n", lineNumber, offset + 1, token.c_str());
            tokenStarted = false;
            offset = current;
            return token;
        }
    }

    if (tokenStarted) {
        string token = currentLine.substr(offset, current - offset);
        // printf("Token: %u:%u : %s\n", lineNumber, offset + 1, token.c_str());
        offset = current;
        current = -1;
        return token;
    }
    else {
        current = -1;
        return getToken();
    }
}

bool Tokenizer::isDelimeter(char c) {
    if (c == ' ' || c == '\t' || c == '\n')
        return true;
    return false;
}

void Tokenizer::reset() {
    reader.clear();
    reader.seekg(0);
    offset = -1;
    current = -1;
    lineNumber = 0;
    tokenStarted = false;
}
