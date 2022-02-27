#ifndef CORE_HPP
#define CORE_HPP

#include <vector>
#include "des.hpp"
#include "scheduler.hpp"
#include "parser.hpp"
#include "com_args.hpp"

using namespace std;

class Core {

    private:

        std::vector<Process> processList;
        Scheduler* scheduler;
        DES* des;
        Parser* parser;
        Process* runningProcess;
        Process* farthestBlockedProcess;
        int currentTime;
        int startTime;
        int totalIoTime;
        int farthestBlockTime;
        bool callScheduler;

        void updateStats(Event event);
        int setCpuBurst();
        void initProcessList();
        void runProcess(int runtime, int remCurrBurst, EventTransition transition);
        void genEventAfterRun(int runtime, EventTransition transition);
        void runProcess(int runtime, int remCurrBurst);
        bool shouldPollEvent();
        void incNonOverlappingTotalIo(Process* currentProcess, int ioBurst);
        void blockProcess(Process* currentProcess, int ioBurst);
        void preemptVerbose(Process* currentProcess);
        bool isRunningProcessPreemptableBy(Process* currentProcess);
        void callSchedulerUtil(Process* currentProcess);

    public:

        Core(Scheduler* scheduler, DES* des, Parser* parser);
        void simulate();
        void printReport();
};

#endif
