#include "core.hpp"

Core::Core(Scheduler* scheduler, DES* des, Parser* parser) {
    this->parser = parser;
    this->scheduler = scheduler;
    this->des = des;
    currentTime = -1;
    runningProcess = nullptr;
    farthestBlockedProcess = nullptr;
    totalIoTime = 0;
    startTime = -1;
    farthestBlockTime = 0;
    callScheduler = false;
}

void Core::simulate() {
    initProcessList();
    Event currentEvent = des->pollEvent();
    Process* currentProcess;

    while (currentEvent.getTimestamp() != -1) {
        currentTime = currentEvent.getTimestamp();
        currentProcess = currentEvent.getProcess();
        int waitTime = currentTime - currentEvent.getProcess()->getStateTimestamp();
        currentEvent.setTimeSpan(waitTime);
        currentProcess->setStateTimestamp(currentTime);

        switch (currentEvent.getTransition()) {
            case TRANS_TO_READY: {
                currentEvent.setNewState(READY);
                currentProcess->setState(READY);
                std::cout << currentEvent;
                if (farthestBlockedProcess == currentProcess)
                    farthestBlockedProcess = nullptr;
                scheduler->addProcess(currentProcess);
                callScheduler = true;
                break;
            }
            /*
            Process is not actually run in this transition; we generate events with params as runtime
            and the following event runs the process before doing its own work. This lazy running is
            done to incorporate the fact that in PrePRIO running process can be preempted, and then
            the following preemption event can actually run the segment of time the process actually ran
            */
            case TRANS_TO_RUN: {
                currentEvent.setNewState(RUNNING);
                currentProcess->setState(RUNNING);
                runningProcess = currentProcess;
                int cpuBurst = setCpuBurst();
                runningProcess->incTotalWaitTime(waitTime);
                int quantum = scheduler->getQuantum();
                std::cout << currentEvent;

                //complete
                if (min(cpuBurst, quantum) >= runningProcess->getRemCpuTime())
                    genEventAfterRun(min(cpuBurst, quantum), TRANS_TO_DONE);
                // blocking
                else if (cpuBurst <= quantum)
                    genEventAfterRun(cpuBurst, TRANS_TO_BLOCK);
                // preemption
                else if (cpuBurst > quantum)
                    genEventAfterRun(quantum, TRANS_TO_PREEMPT);
                break;
            }
            case TRANS_TO_BLOCK: {
                // run process before blocking
                runProcess(currentTime - currentEvent.getCreatedAt(), 0);
                // block process
                currentEvent.setNewState(BLOCKED);
                std::cout << currentEvent;
                int ioBurst = parser->myrandom(currentProcess->getMaxIoBurst());
                blockProcess(currentProcess, ioBurst);
                incNonOverlappingTotalIo(currentProcess, ioBurst);
                callScheduler = true;
                break;
            }
            case TRANS_TO_PREEMPT: {
                // run process before preempt
                runProcess(waitTime, runningProcess->getRemCurrCpuBurstTime() - waitTime);
                // preempt
                currentEvent.setNewState(READY);
                currentProcess->setState(READY);
                std::cout << currentEvent;
                if (scheduler->isPreemptive())
                    currentProcess->decDynamicPriority();
                runningProcess = nullptr;
                scheduler->addProcess(currentProcess);
                callScheduler = true;
                break;
            }
            case TRANS_TO_DONE: {
                // run process before done
                runProcess(currentTime - currentEvent.getCreatedAt(), 0);
                // done
                currentEvent.setNewState(DONE);
                std::cout << currentEvent;
                currentProcess->setState(DONE);
                currentProcess->setFinishTime(currentTime);
                currentProcess->setTurnAroundTime(currentTime - currentProcess->getArrivalTime());
                runningProcess = nullptr;
                callScheduler = true;
                break;
            }
        }
        if (callScheduler)
            callSchedulerUtil(currentProcess);
        currentEvent = des->pollEvent();
    }
}

void Core::callSchedulerUtil(Process* currentProcess) {
    // Can the scheduler preempt a running process
    if (scheduler->testPreempt() && runningProcess != nullptr) {
        preemptVerbose(currentProcess);
        /*
        A (running) process who has completed event for `trans_to_run`, would have generated either
        a `trans_to_done`, `trans_to_block` or `trans_to_preempt` event. Since we are preempting this
        running process now, the corresponding "future" event becomes invalid (we need to remove this event)
        and push a new `trans_to_preempt` event at `currentTime` for the `runningProcess`
        */
        if (isRunningProcessPreemptableBy(currentProcess)) {
            des->removeEventForProcess(runningProcess);
            Event newEvent(currentTime, currentTime, runningProcess, RUNNING, TRANS_TO_PREEMPT);
            des->pushEvent(newEvent);
        }
    }
    else if (shouldPollEvent()) {
        // currentEvent = des->pollEvent();
        //continue;
    }
    else if (runningProcess == nullptr) {
        runningProcess = scheduler->pollProcess();
        Event newEvent(currentTime, currentTime, runningProcess, READY, TRANS_TO_RUN);
        des->pushEvent(newEvent);
        runningProcess = nullptr;
        callScheduler = false;
    }
}

bool Core::isRunningProcessPreemptableBy(Process* currentProcess) {
    return currentProcess->getDynamicPriority() > runningProcess->getDynamicPriority() &&
                currentTime < des->getEarliestTimestampOfProcess(runningProcess);
}

void Core::blockProcess(Process* currentProcess, int ioBurst) {
    runningProcess = nullptr;
    currentProcess->setCurrIoBurstTime(ioBurst);
    currentProcess->setState(BLOCKED);
    currentProcess->incTotalIoTimeBy(ioBurst);
    currentProcess->initDynamicPriority();
    Event newEvent(currentTime, currentTime + ioBurst, currentProcess, BLOCKED, TRANS_TO_READY);
    des->pushEvent(newEvent);
}

void Core::incNonOverlappingTotalIo(Process* currentProcess, int ioBurst) {
    if (farthestBlockedProcess != nullptr && (currentTime + ioBurst > farthestBlockTime)) {
        farthestBlockedProcess = currentProcess;
        totalIoTime += (currentTime + ioBurst - farthestBlockTime);
        farthestBlockTime = currentTime + ioBurst;
    }
    else if (farthestBlockedProcess == nullptr) {
        farthestBlockedProcess = currentProcess;
        farthestBlockTime = currentTime + ioBurst;
        totalIoTime += ioBurst;
    }
}

// Is the next event happening now OR has the scheduler run out of processes?
bool Core::shouldPollEvent() {
    return (!des->isEmpty() && des->getNextEventTime() == currentTime) ||
            (runningProcess == nullptr && scheduler->isEmpty());
}

void Core::runProcess(int runtime, int remCurrBurst, EventTransition transition) {
    runningProcess->decRemCpuTime(runtime);
    runningProcess->incCpuUsage(runtime);
    runningProcess->setRemCurrCpuBurstTime(remCurrBurst);
    Event newEvent(currentTime, currentTime + runtime, runningProcess, RUNNING, transition);
    des->pushEvent(newEvent);
}

void Core::genEventAfterRun(int runtime, EventTransition transition) {
    Event newEvent(currentTime, currentTime + runtime, runningProcess, RUNNING, transition);
    des->pushEvent(newEvent);
}

void Core::runProcess(int runtime, int remCurrBurst) {
    runningProcess->decRemCpuTime(runtime);
    runningProcess->incCpuUsage(runtime);
    runningProcess->setRemCurrCpuBurstTime(remCurrBurst);
}

/*
If some of currCpuBurst is remaining from last time; use it as cpu burst for this time
Else renew cpu Burst
Reset cpu burst if required cpu time is less than cpu burst
*/
int Core::setCpuBurst() {
    int cpuBurst = 0;
    bool renewBurst = false;

    if (runningProcess->getRemCurrCpuBurstTime() > 0)
        cpuBurst = runningProcess->getRemCurrCpuBurstTime();
    else {
        cpuBurst = parser->myrandom(runningProcess->getMaxCpuBurst());
        runningProcess->setCurrCpuBurstTime(cpuBurst);
        runningProcess->setRemCurrCpuBurstTime(cpuBurst);
        renewBurst = true;
    }
    if (runningProcess->getRemCpuTime() < cpuBurst) {
        cpuBurst = runningProcess->getRemCpuTime();
        runningProcess->setRemCurrCpuBurstTime(cpuBurst);
        if (renewBurst)
            runningProcess->setCurrCpuBurstTime(cpuBurst);
    }
    return cpuBurst;
}

void Core::initProcessList() {
    this->processList = parser->parseProcess();
    startTime = processList[0].getArrivalTime();
    for (int i = 0; i < processList.size(); ++i) {
        int staticPriority = parser->myrandom(scheduler->getMaxprio());
        processList[i].setStaticPriority(staticPriority);
        processList[i].initDynamicPriority();
        Event newEvent(processList[i].getArrivalTime(), processList[i].getArrivalTime(), &processList[i],
        CREATED, TRANS_TO_READY);
        des->pushEvent(newEvent);
    }
}

void Core::printReport() {
    std::printf("%s\n", scheduler->getName().c_str());
    for (int i = 0; i < processList.size(); ++i) {
        std::printf("%04d:\t%d\t%d\t%d\t%d\t%d\t|\t%d\t%d\t%d\t%d\n",
            processList[i].getPid(),
            processList[i].getArrivalTime(),
            processList[i].getCpuTimeReq(),
            processList[i].getMaxCpuBurst(),
            processList[i].getMaxIoBurst(),
            processList[i].getStaticPriority(),
            processList[i].getFinishTime(),
            processList[i].getTurnAroundTime(),
            processList[i].getTotalIoTime(),
            processList[i].getTotalWaitTime()
        );
    }

    int totalTTtime = 0, totalWaitTime = 0, totalCpu = 0;
    for (int i = 0; i < processList.size(); ++i) {
        totalTTtime += processList[i].getTurnAroundTime();
        totalWaitTime += processList[i].getTotalWaitTime();
        totalCpu += processList[i].getCpuTimeReq();
    }

    std::printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
        currentTime,
        ((double)(100 * totalCpu)/currentTime),
        ((double)(100 * totalIoTime)/currentTime),
        ((double)totalTTtime/processList.size()),
        ((double)totalWaitTime/processList.size()),
        ((double)(100 * processList.size())/currentTime)
    );
}

void Core::updateStats(Event event) {
    Process* process = event.getProcess();
    process->setFinishTime(currentTime);
    process->setTurnAroundTime(process->getFinishTime() - process->getArrivalTime());
    event.setProcess(process);
}

void Core::preemptVerbose(Process* currentProcess) {
    if (verbose) {
        printf("---> PRIO preemption %d by %d ? %d TS=%d now=%d) --> %s\n",
        runningProcess->getPid(),
        currentProcess->getPid(),
        currentProcess->getDynamicPriority() > runningProcess->getDynamicPriority() ? 1 : 0,
        des->getEarliestTimestampOfProcess(runningProcess),
        currentTime,
        isRunningProcessPreemptableBy(currentProcess) ? "YES" : "NO");
    }
}