#include "event.hpp"

Event::Event(int createdAt, int timestamp, Process* process, ProcessState state, EventTransition transition) {
    this->createdAt = createdAt;
    this->timestamp = timestamp;
    this->process = process;
    this->oldState = state;
    this->transition = transition;
}

int Event::getTimestamp() const {
    return timestamp;
}

Process* Event::getProcess() const {
    return process;
}

ProcessState Event::getOldState() const {
    return oldState;
}

ProcessState Event::getNewState() const {
    return newState;
}

EventTransition Event::getTransition() const {
    return transition;
}

int Event::getCreatedAt() const {
    return createdAt;
}

int Event::getTimeSpan() const {
    return timeSpan;
}

void Event::setProcess(Process* process) {
    this->process = process;
}

void Event::setNewState(ProcessState state) {
    this->newState = state;
}

void Event::setTimeSpan(int time) {
    timeSpan = time;
}

const char * const Event::enumStr[] = { "CREATED", "READY", "RUNNING", "BLOCK", "Done"};

ostream & operator << (ostream &out, const Event &event) {
    if (!verbose)
        return out;

    if (event.getNewState() == READY && event.getTransition() == TRANS_TO_PREEMPT) {
        printf("%d\t%d\t%d: %s\t-> %s\tcb=%d\trem=%d\tprio=%d\n",
        event.getTimestamp(),
        event.getProcess()->getPid(),
        event.getTimeSpan(),
        Event::enumStr[event.getOldState()],
        Event::enumStr[event.getNewState()],
        event.getProcess()->getRemCurrCpuBurstTime(),
        event.getProcess()->getRemCpuTime(),
        event.getProcess()->getDynamicPriority());
    }

    else if (event.getNewState() == READY) {
        printf("%d\t%d\t%d: %s\t-> %s\n",
        event.getTimestamp(),
        event.getProcess()->getPid(),
        event.getTimeSpan(),
        Event::enumStr[event.getOldState()],
        Event::enumStr[event.getNewState()]);
    }
    else if (event.getNewState() == RUNNING) {
        printf("%d\t%d\t%d: %s\t-> %s\tcb=%d\trem=%d\tprio=%d\n",
        event.getTimestamp(),
        event.getProcess()->getPid(),
        event.getTimeSpan(),
        Event::enumStr[event.getOldState()],
        Event::enumStr[event.getNewState()],
        event.getProcess()->getRemCurrCpuBurstTime(),
        event.getProcess()->getRemCpuTime(),
        event.getProcess()->getDynamicPriority());
    }

    else if (event.getNewState() == BLOCKED) {
        printf("%d\t%d\t%d: %s\t-> %s\tib=%d\trem=%d\n",
        event.getTimestamp(),
        event.getProcess()->getPid(),
        event.getTimeSpan(),
        Event::enumStr[event.getOldState()],
        Event::enumStr[event.getNewState()],
        event.getProcess()->getRemCurrCpuBurstTime(),
        event.getProcess()->getRemCpuTime());
    }

    else if (event.getNewState() == DONE) {
        printf("%d\t%d\t%d: Done\n",
        event.getTimestamp(),
        event.getProcess()->getPid(),
        event.getTimeSpan());
    }
    return out;
}
