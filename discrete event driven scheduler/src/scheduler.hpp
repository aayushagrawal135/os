#ifndef SCHEDULER
#define SCHEDULER

#include <list>
#include <vector>
#include <queue>
#include "process.hpp"

#define DEFAULT_MAXPRIO 4

class Scheduler {

    protected:

        Scheduler();
        std::list<Process *> readyQueue;
        int quantum;
        int maxprio;
        int numProcesses;
        int size;

    public:

        virtual Process* pollProcess() = 0;
        virtual std::string getName() const = 0;
        virtual void addProcess(Process* process);
        virtual bool isEmpty() const;
        virtual bool isPreemptive() const;
        virtual bool testPreempt() const;
        int getQuantum() const;
        int getMaxprio() const;
};

class FCFS : public Scheduler {

    public:

        Process* pollProcess();
        std::string getName() const;
};

class LCFS : public Scheduler {

    public:

        Process* pollProcess();
        std::string getName() const;
};

class SRTF : public Scheduler {

    public:

        Process* pollProcess();
        std::string getName() const;
};

class RR : public FCFS {

    public:

        RR(int quantum);
        std::string getName() const;
};

class PRIO : public Scheduler {

    protected:

        std::vector<std::queue<Process *> > queue1;
        std::vector<std::queue<Process *> > queue2;
        std::vector<std::queue<Process *> > *activeQ, *expiredQ;

        void addProcessUtil(Process* process, bool isActive);
        Process* pollActiveProcess();
        void exchangeActiveExpiredQueues();
        void init(int quantum, int maxprio);

    public:

        PRIO(int quantum);
        PRIO(int quantum, int maxprio);
        void addProcess(Process* process);
        Process* pollProcess();
        std::string getName() const;
        bool isPreemptive() const;
};

class PrePRIO : public PRIO {

    public:

        PrePRIO(int quantum);
        PrePRIO(int quantum, int maxprio);
        std::string getName() const;
        bool testPreempt() const;
};

#endif
