#include <iostream>
#include <string>
#include <getopt.h>

#include "des.hpp"
#include "core.hpp"
#include "scheduler.hpp"
#include "parser.hpp"
#include "com_args.hpp"

using namespace std;

Scheduler* getScheduler(string args);
char* getInputFile(int argc, char** argv);
char* getRFile(int argc, char** argv);
void setArgs(int argc, char** argv);

bool verbose = false;
Scheduler* scheduler = nullptr;

int main(int argc, char** argv) {
    setArgs(argc, argv);
    DES des;
    Parser parser(getInputFile(argc, argv), getRFile(argc, argv));
    Core core(scheduler, &des, &parser);
    core.simulate();
    core.printReport();

    if (scheduler != nullptr)
        free(scheduler);
    return 0;
}

void setArgs(int argc, char** argv) {
    int opt;
    while ((opt = getopt(argc, argv, "vteps:")) != -1) 
    {
        switch (opt) {
            case 'v':
                verbose = true;
                break;
            case 't':
                break;
            case 'e':
                break;
            case 'p':
                break;
            case 's':
                scheduler = getScheduler(optarg);
                break;
        }
    }

    if (scheduler == nullptr)
        scheduler = getScheduler("F");
}

Scheduler* getScheduler(string args) {
    if (args[0] == 'F')
        return (new FCFS());
    if (args[0] == 'L')
        return (new LCFS());
    if (args[0] == 'S')
        return (new SRTF());
    if (args[0] == 'R') {
        int quantum = stoi(args.substr(1));
        return new RR(quantum);
    }
    if (args[0] == 'P') {
        int index;
        if ((index = args.find(":")) == string::npos) {
            int quantum = stoi(args.substr(1));
            return new PRIO(quantum);
        }
        else {
            int quantum = stoi(args.substr(1, index - 1));
            int maxprio = stoi(args.substr(index + 1));
            return new PRIO(quantum, maxprio);
        }
    }
    if (args[0] == 'E') {
        int index;
        if ((index = args.find(":")) == string::npos) {
            int quantum = stoi(args.substr(1));
            return new PrePRIO(quantum);
        }
        else {
            int quantum = stoi(args.substr(1, index - 1));
            int maxprio = stoi(args.substr(index + 1));
            return new PrePRIO(quantum, maxprio);
        }
    }
    return nullptr;
}

char* getInputFile(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-')
            return argv[i];
    }
    return nullptr;
}

char* getRFile(int argc, char** argv) {
    for (int i = argc-1; i >= 0; --i) {
        if (argv[i][0] != '-')
            return argv[i];
    }
    return nullptr;
}
