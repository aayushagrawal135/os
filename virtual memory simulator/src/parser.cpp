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
	string currentLine;
    int numProcesses = -1;
    int countProcess = -1;
    Process * process;
    int vmaCount = 0;

    while (getline(reader, currentLine)) {
        // let go commented line
        if (currentLine.size() > 0 && currentLine[0] == '#')
            continue;

        // `numProcess` is the first input
        else if (numProcesses == -1) {
            numProcesses = stoi(currentLine);
            countProcess = 0;
        }

        // single token in line should be #VMAs for that process
        else if (currentLine.find(' ') == string::npos) {
            process = new Process(countProcess);
            vmaCount = stoi(currentLine);
        }

        // first char is not an instruction, then its a vma, read and feed to process
        else if (currentLine.size() > 0 && currentLine[0] < 'c') {
            int values[4], begin = 0, end = 0;
            for (int i = 0; i < 4; ++i) {
                end = currentLine.find(' ', begin);
                if (end != string::npos)
                    values[i] = stoi(currentLine.substr(begin, end - begin));
                else
                    values[i] = stoi(currentLine.substr(begin));
                begin = end + 1;
            }
            VMA vma(values[0], values[1], values[2], values[3]);
            process->addVirtualMemArea(vma);
            --vmaCount;
            if (vmaCount == 0) {
                processList.push_back(*process);
                process = nullptr;
                ++countProcess;
                if (countProcess == numProcesses)
                    break;
            }
        }
    }
    return processList;
}

Instruction *Parser::getParsedInstr() {
    string currentLine;
    if (getline(reader, currentLine)) {
        // let go commented line
        if (currentLine.size() > 0 && currentLine[0] == '#')
            return getParsedInstr();
        // otherwise return instruction
        return new Instruction(currentLine[0], stoi(currentLine.substr(2)));
    }
    else {
        return nullptr;
    }
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
    return (fnumber % num);
}
