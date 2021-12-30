#include <iostream>
#include <iostream>
#include <string>
#include <getopt.h>

#include "parser.hpp"
#include "scheduler.hpp"
#include "core.hpp"

using namespace std;

Scheduler* getScheduler(string args);
char *getInputFile(int argc, char **argv);
void setArgs(int argc, char **argv);
void setOptions(string args);
void clean();

Scheduler *scheduler = nullptr;
Parser *parser = nullptr;
Core *core = nullptr;

int main(int argc, char **argv) {
    parser = new Parser(getInputFile(argc, argv));
    setArgs(argc, argv);
    if (scheduler == nullptr)
        scheduler = new FIFO();
    core = new Core(parser, scheduler);
    core->simulate();
    core->printReport();
    clean();
    return 0;
}

void clean() {
    free(core);
    free(parser);
    free(scheduler);
}

void setArgs(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "s:vqf")) != -1) {
        switch (opt) {
            case 'v':
            case 'q':
            case 'f':
                break;
            case 's':
                scheduler = getScheduler(optarg);
                break;
        }
    }
}

Scheduler* getScheduler(string args) {
    switch (args[0]) {
        case 'i':
            return new FIFO();
        case 'j':
            return new SSTF();
        case 's':
            return new Look();
        case 'f':
            return new FLook();
        case 'c':
            return new CLook();
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
