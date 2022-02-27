#include "process.hpp"

Process::Process(int pid, int arrivalTime, int cpuTimeReq, int maxCpuBurst, int maxIoBurst) {
    this->pid = pid;
    this->arrivalTime = arrivalTime;
    this->cpuTimeReq = cpuTimeReq;
    this->maxCpuBurst = maxCpuBurst;
    this->maxIoBurst = maxIoBurst;
    this->remCpuTime = cpuTimeReq;
    state = CREATED;
    this->currCpuBurstTime = 0;
    this->remCurrCpuBurstTime = currCpuBurstTime;
    this->currIoBurstTime = 0;
    this->finishTime = 0;
    this->turnAroundTime = 0;
    this->totalIoTime = 0;
    this->totalWaitTime = 0;
    this->cpuUsage = 0;
    this->stateTimestamp = arrivalTime;
}

int Process::getPid() const {
    return pid;
}

int Process::getArrivalTime() const {
    return arrivalTime;
}

int Process::getCpuTimeReq() const {
    return cpuTimeReq;
}

int Process::getRemCpuTime() const {
    return remCpuTime;
}

int Process::getMaxCpuBurst() const {
    return maxCpuBurst;
}

int Process::getRemCurrCpuBurstTime() const {
    return remCurrCpuBurstTime;
}

int Process::getCurrCpuBurstTime() const {
    return currCpuBurstTime;
}

int Process::getMaxIoBurst() const {
    return maxIoBurst;
}

int Process::getCurrIoBurstTime() const {
    return currIoBurstTime;
}

int Process::getStateTimestamp() const {
    return stateTimestamp;
}

int Process::getStaticPriority() const {
    return staticPriority;
}

int Process::getFinishTime() const {
    return finishTime;
}

int Process::getTurnAroundTime() const {
    return turnAroundTime;
}

int Process::getTotalIoTime() const {
    return totalIoTime;
}

int Process::getTotalWaitTime() const {
    return totalWaitTime;
}

int Process::getDynamicPriority() const {
    return dynamicPriority;
}

void Process::decRemCpuTime(int time) {
    this->remCpuTime -= time;
}

void Process::setCurrCpuBurstTime(int time) {
    this->currCpuBurstTime = time;
}

void Process::setRemCurrCpuBurstTime(int time) {
    this->remCurrCpuBurstTime = time;
}

void Process::setCurrIoBurstTime(int time) {
    this->currIoBurstTime = time;
}

void Process::setState(ProcessState state) {
    this->state = state;
}

void Process::setStateTimestamp(int timestamp) {
    stateTimestamp = timestamp;
}

void Process::setStaticPriority(int priority) {
    staticPriority = priority;
}

void Process::setFinishTime(int time) {
    finishTime = time;
}

void Process::setTurnAroundTime(int time) {
    turnAroundTime = time;
}

void Process::incTotalIoTimeBy(int time) {
    totalIoTime += time;
}

void Process::decDynamicPriority() {
    --dynamicPriority;
}

void Process::initDynamicPriority() {
    dynamicPriority = staticPriority - 1;
}

void Process::incTotalWaitTime(int time) {
    totalWaitTime += time;
}

void Process::incCpuUsage(int time) {
    cpuUsage += time;
}

ostream & operator << (ostream &out, const Process &process) {
    printf("PID: %d\tArrival: %d\tTotal CPU: %d\tMax CPU Burst: %d\tMax IO Burst: %d\n",
    process.getPid(), process.getArrivalTime(), process.getCpuTimeReq(), process.getMaxCpuBurst(),
    process.getMaxIoBurst());
    return out;
}
