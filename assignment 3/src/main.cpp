#include <iostream>
#include <iostream>
#include <string>
#include <getopt.h>

#include "core.hpp"
#include "parser.hpp"
#include "process.hpp"
#include "pager.hpp"
#include "com_args.hpp"

using namespace std;

Pager* getPager(string args);
char* getInputFile(int argc, char **argv);
char* getRFile(int argc, char **argv);
void setArgs(int argc, char **argv);
void setOptions(string args);

int numFrames = -1;
bool option = false;
bool pageTableOption = false;
bool frameTableOption = false;
bool statAndSummary = false;

Pager *pager = nullptr;
Parser *parser = nullptr;

int main(int argc, char **argv) {
    parser = new Parser(getInputFile(argc, argv), getRFile(argc, argv));
    setArgs(argc, argv);
    if (pager == nullptr)
        pager = new FIFO();
    
    if (numFrames != -1)
        pager->setNumFrames(numFrames);

    Core core(pager, parser);
    core.simulate();
    core.print();
    free(parser);
    free(pager);
    return 0;
}

void setArgs(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "f:a:o:")) != -1) {
        switch (opt) {
            case 'f':
                numFrames = stoi(optarg);
                break;
            case 'a':
                pager = getPager(optarg);
                break;
            case 'o':
                setOptions(optarg);
                break;
        }
    }
}

void setOptions(string args) {
    if (args.find('O') != string::npos)
        option = true;
    if (args.find('P') != string::npos)
        pageTableOption = true;
    if (args.find('F') != string::npos)
        frameTableOption = true;
    if (args.find('S') != string::npos)
        statAndSummary = true;
}

Pager* getPager(string args)
{
    switch (args[0]) {
        case 'f':
            return (new FIFO());
        case 'r':
            return (new Random(parser));
        case 'c':
            return (new Clock());
        case 'e':
            return (new NRU());
        case 'a':
            return (new Aging());
        case 'w':
            return (new WorkingSet());
    }
    return nullptr;
}

char *getInputFile(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-')
            return argv[i];
    }
    return nullptr;
}

char *getRFile(int argc, char **argv) {
    for (int i = argc - 1; i >= 0; --i) {
        if (argv[i][0] != '-')
            return argv[i];
    }
    return nullptr;
}
