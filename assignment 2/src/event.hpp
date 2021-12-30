#ifndef EVENT_HPP
#define EVENT_HPP

#include "process.hpp"
#include "com_args.hpp"

enum EventTransition {
    TRANS_TO_READY,
    TRANS_TO_RUN,
    TRANS_TO_BLOCK,
    TRANS_TO_PREEMPT,
    TRANS_TO_DONE
};

class Event {

    private:

        int timestamp;
        Process* process;
        ProcessState oldState;
        ProcessState newState;
        int createdAt;
        EventTransition transition;
        int timeSpan;

    public:

        Event(int createdAt, int timestamp, Process* process, ProcessState state, EventTransition transition);
        friend ostream & operator << (ostream &out, const Event &event);
        static const char * const enumStr[];

        int getTimestamp() const;
        Process* getProcess() const;
        ProcessState getOldState() const;
        ProcessState getNewState() const;
        EventTransition getTransition() const;
        int getCreatedAt() const;
        int getTimeSpan() const;

        void setProcess(Process* process);
        void setNewState(ProcessState state);
        void setTimeSpan(int time);
};

#endif
