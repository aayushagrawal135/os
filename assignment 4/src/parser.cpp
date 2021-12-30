#include "parser.hpp"

Parser::Parser(char* fname) {
    reader.open(fname);
}

Parser::~Parser() {
    reader.close();
}

Request* Parser::getParsedRequest() {
    string currentLine;
    if (getline(reader, currentLine)) {
        // let go commented line
        if (currentLine.size() > 0 && currentLine[0] == '#')
            return getParsedRequest();
        // otherwise return instruction
        int delimenterPos = currentLine.find(" ");
        int arrivalTime = stoi(currentLine.substr(0, delimenterPos));
        int trackNum = stoi(currentLine.substr(delimenterPos+1));
        return new Request(arrivalTime, trackNum);
    }
    else
        return nullptr;
}

queue<Request*> Parser::getRequests() {
    queue<Request*> requestQueue;
    Request* request;
    while ((request = getParsedRequest()) != nullptr)
        requestQueue.push(request);
    return requestQueue;
}
