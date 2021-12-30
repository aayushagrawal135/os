#ifndef DES_HPP
#define DES_HPP

#include <list>
#include "event.hpp"

class DES {

    private:

        std::list<Event> eventQueue;

    public:

        void pushEvent(Event event);
        Event pollEvent();
        Process* getNextEventProcess();
        int getNextEventTime();
        bool isEmpty();
        int getEarliestTimestampOfProcess(Process* process);
        void removeEventForProcess(Process* process);
};

#endif
