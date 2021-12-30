#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <iostream>
#include <string>

using namespace std;

enum ProcessState {
    CREATED,
    READY,
    RUNNING,
    BLOCKED,
    DONE
};

class Process {

    private:

        int pid;
        int arrivalTime;
        int cpuTimeReq;
        int remCpuTime;
        int maxCpuBurst;
        int currCpuBurstTime;
        int remCurrCpuBurstTime;
        int maxIoBurst;
        int currIoBurstTime;
        ProcessState state;
        int stateTimestamp;
        int staticPriority;
        int dynamicPriority;

        int finishTime;
        int turnAroundTime;
        int totalIoTime;
        int totalWaitTime;
        int cpuUsage;

    public:

        Process(int pid, int arrivalTime, int cpuTimeReq, int maxCpuBurst, int maxIoBurst);
        friend ostream & operator << (ostream &out, const Process &process);

        int getPid() const;
        int getArrivalTime() const;
        int getCpuTimeReq() const;
        int getRemCpuTime() const;
        int getMaxCpuBurst() const;
        int getCurrCpuBurstTime() const;
        int getRemCurrCpuBurstTime() const;
        int getMaxIoBurst() const;
        int getCurrIoBurstTime() const;
        int getStateTimestamp() const;
        int getStaticPriority() const;
        int getFinishTime() const;
        int getTurnAroundTime() const;
        int getTotalIoTime() const;
        int getTotalWaitTime() const;
        int getDynamicPriority() const;

        void decRemCpuTime(int time);
        void setCurrCpuBurstTime(int time);
        void setRemCurrCpuBurstTime(int time);
        void setCurrIoBurstTime(int time);
        void setState(ProcessState state);
        void setStateTimestamp(int time);
        void setStaticPriority(int priority);
        void setFinishTime(int time);
        void setTurnAroundTime(int time);
        void incTotalIoTimeBy(int time);
        void decDynamicPriority();
        void initDynamicPriority();
        void incTotalWaitTime(int time);
        void incCpuUsage(int time);
};

#endif
