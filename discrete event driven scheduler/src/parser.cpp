#include "parser.hpp"

Parser::Parser(char* fname, char* rfname) {
    reader.open(fname);
    rfreader.open(rfname);
    string currentLine;
    getline(rfreader, currentLine);
}

Parser::~Parser() {
    rfreader.close();
    reader.close();
}

vector<Process> Parser::parseProcess() {
    vector<Process> processList;
	string token;
	int pid = 0;
	while (reader >> token) {
        int arrivalTime = stoi(token.c_str());
        reader >> token;
        int cpuTime = stoi(token.c_str());
        reader >> token;
        int cpuBurst = stoi(token.c_str());
        reader >> token;
        int ioBurst = stoi(token.c_str());
        Process process(pid, arrivalTime, cpuTime, cpuBurst, ioBurst);
		processList.push_back(process);
		++pid;
	}
    return processList;
}

long Parser::getToken() {
    string currentLine;
    if (getline(rfreader, currentLine)) {
        return stol(currentLine);
    }
    else {
        rfreader.clear();
        rfreader.seekg(0);
        // let go the first line
        getline(rfreader, currentLine);
        return getToken();
    }
    return -1;
}

int Parser::myrandom(int num) {
    long fnumber = getToken();
    return 1 + (fnumber % num);
}
