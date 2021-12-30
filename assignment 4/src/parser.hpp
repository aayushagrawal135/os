#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <queue>
#include <fstream>
#include "request.hpp"

using namespace std;

class Parser {

    private:

        fstream reader;

    public:

        Parser(char* fname);
        ~Parser();
        Request* getParsedRequest();
        queue<Request*> getRequests();
};

#endif
