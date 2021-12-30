#include "des.hpp"

void DES::pushEvent(Event event) {
    for (auto iter = eventQueue.begin(); iter != eventQueue.end(); ++iter) {
        if (event.getTimestamp() < (*iter).getTimestamp()) {
            eventQueue.insert(iter, event);
            return;
        }
    }
    eventQueue.push_back(event);
}

Event DES::pollEvent() {
    if (!eventQueue.empty()) {
        Event event = eventQueue.front();
        eventQueue.erase(eventQueue.begin());
        return event;
    }
    else {
        Event e(-1, -1, nullptr, DONE, TRANS_TO_DONE);
        return e;
    }
}

int DES::getNextEventTime() {
    if(!eventQueue.empty())
        return eventQueue.front().getTimestamp();
	return -1;
}

Process* DES::getNextEventProcess() {
    if(!eventQueue.empty())
        return eventQueue.front().getProcess();
	return nullptr;
}

bool DES::isEmpty() {
    return eventQueue.size() == 0;
}

int DES::getEarliestTimestampOfProcess(Process* process) {
    for (auto iter = eventQueue.begin(); iter != eventQueue.end(); ++iter) {
        if ((*iter).getProcess() == process) {
            return (*iter).getTimestamp();
        }
    }
    return -1;
}

void DES::removeEventForProcess(Process* process) {
    for (auto iter = eventQueue.begin(); iter != eventQueue.end(); ++iter) {
        if ((*iter).getProcess() == process) {
            eventQueue.erase(iter);
            break;
        }
    }
}
