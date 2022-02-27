#include "scheduler.hpp"

Scheduler::Scheduler() {
    quantum = 100000;
    maxprio = DEFAULT_MAXPRIO;
    numProcesses = 0;
    size = 0;
}

bool Scheduler::isEmpty() const {
    return size == 0;
}

int Scheduler::getQuantum() const {
    return quantum;
}

int Scheduler::getMaxprio() const {
    return maxprio;
}

void Scheduler::addProcess(Process* process) {
    readyQueue.push_back(process);
    ++size;
}

bool Scheduler::isPreemptive() const {
    return false;
}

bool Scheduler::testPreempt() const {
    return false;
}

Process* FCFS::pollProcess() {
    if (readyQueue.empty())
        return (Process *)nullptr;
    Process* process;
    process = readyQueue.front();
    readyQueue.erase(readyQueue.begin());
    --size;
    return process;
}

std::string FCFS::getName() const {
    return "FCFS";
}

Process* LCFS::pollProcess() {
    if (readyQueue.empty())
        return (Process *)nullptr;
    Process* process;
    process = readyQueue.back();
    readyQueue.pop_back();
    --size;
    return process;
}

std::string LCFS::getName() const {
    return "LCFS";
}

Process* SRTF::pollProcess() {
    if (readyQueue.empty())
        return (Process *)nullptr;
    std::list<Process *>::iterator iter = readyQueue.begin();
    std::list<Process *>::iterator srtfIter = iter;
    while (iter != readyQueue.end()) {
        if ((*iter)->getRemCpuTime() < (*srtfIter)->getRemCpuTime())
            srtfIter = iter;
        ++iter;
    }
    Process* process = *srtfIter;
    readyQueue.erase(srtfIter);
    --size;
    return process;
}

std::string SRTF::getName() const {
    return "SRTF";
}

RR::RR(int quantum) {
    this->quantum = quantum;
}

std::string RR::getName() const {
    return "RR " + to_string(quantum);
}

PRIO::PRIO(int quantum) {
    init(quantum, DEFAULT_MAXPRIO);
}

PRIO::PRIO(int quantum, int maxprio) {
    init(quantum, maxprio);
}

void PRIO::init(int quantum, int maxprio) {
    this->quantum = quantum;
    this->maxprio = maxprio;
    for (int i = 0; i < maxprio; ++i) {
        queue1.push_back(std::queue<Process *>());
        queue2.push_back(std::queue<Process *>());
    }
    activeQ = &queue1;
    expiredQ = &queue2;
}

void PRIO::addProcess(Process* process) {
    if (process->getDynamicPriority() == -1) {
        process->initDynamicPriority();
        addProcessUtil(process, false);
    }
    else {
        addProcessUtil(process, true);
    }
    ++size;
}

void PRIO::addProcessUtil(Process* process, bool isActive) {
    int dynPrio = process->getDynamicPriority();
    if (isActive)
        activeQ->at(dynPrio).push(process);
    else
        expiredQ->at(dynPrio).push(process);
}

Process* PRIO::pollProcess() {
    Process* process = pollActiveProcess();
    if (process != nullptr) {
        --size;
        return process;
    }
    exchangeActiveExpiredQueues();
    process = pollActiveProcess();
    if (process != nullptr) {
        --size;
        return process;
    }
    return process;
}

Process* PRIO::pollActiveProcess() {
    for (int i = maxprio - 1; i >= 0; --i) {
        if (!activeQ->at(i).empty()) {
            Process *process = activeQ->at(i).front();
            activeQ->at(i).pop();
            return process;
        }
    }
    return nullptr;
}

void PRIO::exchangeActiveExpiredQueues() {
    std::vector<std::queue<Process *> > *temp = activeQ;
    activeQ = expiredQ;
    expiredQ = temp;
}

std::string PRIO::getName() const {
    return "PRIO " + to_string(quantum);
}

bool PRIO::isPreemptive() const {
    return true;
}

PrePRIO::PrePRIO(int quantum) : PRIO(quantum) {}

PrePRIO::PrePRIO(int quantum, int maxprio) : PRIO(quantum, maxprio) {}

std::string PrePRIO::getName() const {
    return "PREPRIO " + to_string(quantum);
}

bool PrePRIO::testPreempt() const {
    return true;
}
