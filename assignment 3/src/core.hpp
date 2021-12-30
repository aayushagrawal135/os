#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include <queue>
#include "pager.hpp"
#include "parser.hpp"
#include "process.hpp"
#include "com_args.hpp"

using namespace std;

class Core {

    private:

        vector<Process> processList;
        Process* currentProcess;
        Parser* parser;
        Pager* pager;

        unsigned long instructionCount = 0;
        unsigned long ctxSwitchCount = 0;
        unsigned long exitCount = 0;
        unsigned long readCount = 0;
        unsigned long writeCount = 0;

        void initProcessList();
        PTE *getPageBeforeRW(Instruction* instruction, unsigned long instructionCount);
        void includePageEntry(PTE *pageEntry, Frame *frame, VMA *vma);
        void detachProcessFromFrame(Frame *frame);

        struct Cost {
            unsigned int ctxSwitch = 130;
            unsigned int exit = 1250;
            unsigned int read = 1;
            unsigned int write = 1;
        } cost;

        unsigned long long getTotalCost();

    public:

        Core(Pager* pager, Parser* parser);
        void simulate();
        void print();
};

#endif
