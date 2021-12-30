#include "scheduler.hpp"

Scheduler::Scheduler() {
    requestIterToRemove = requestQueue.end();
}

void Scheduler::pushRequest(Request* request) {
    requestQueue.push_back(request);
}

void Scheduler::popRequest() {
    requestQueue.pop_front();
}

bool Scheduler::empty() {
    return requestQueue.empty();
}

void Scheduler::resetDirection(int requestTrack, int currentTrack) {
    if (requestTrack < currentTrack)
        direction = DOWN;
    else if (requestTrack > currentTrack)
        direction = UP;
}

void Scheduler::printQ() {
    for (deque<Request*>::iterator iter = requestQueue.begin(); iter != requestQueue.end(); ++iter) {
        Request* request = *iter;
        cout << *request;
    }
}

Request *FIFO::getNextRequest(int trackNum) {

    if (requestQueue.empty())
        return nullptr;

    Request* request = requestQueue.front();

    resetDirection(request->trackNum, trackNum);
    return request;
}

void FIFO::popRequest() {
    requestQueue.pop_front();
}

Request *SSTF::getNextRequest(int trackNum) {

    if (requestQueue.empty())
        return nullptr;

    deque<Request *>::iterator nearestRequest = requestQueue.begin();
    int shortestDist = INT_MAX;

    for (deque<Request*>::iterator iter = requestQueue.begin(); iter != requestQueue.end(); ++iter) {
        Request* request = *iter;
        int len = abs(request->trackNum - trackNum);
        if (len < shortestDist) {
            shortestDist = len;
            nearestRequest = iter;
        }
    }
    requestIterToRemove = nearestRequest;
    Request* request = *nearestRequest;
    resetDirection(request->trackNum, trackNum);
    return request;
}

void SSTF::popRequest() {
    requestQueue.erase(requestIterToRemove);
}

Look::Look() {
    primaryQ = &queue1;
}

bool Look::empty() {
    return primaryQ->empty();
}

void Look::pushRequest(Request* request) {
    primaryQ->push_back(request);
}

Request *Look::getNextRequest(int trackNum) {
    return pollRequestUtil(trackNum);
}

Request* Look::pollRequestUtil(int trackNum) {
    if (primaryQ->empty())
        return nullptr;

    deque<Request *>::iterator nearestRequest = primaryQ->end();
    if (direction == UP)
        nearestRequest = nearestRequestAbove(trackNum);
    else
        nearestRequest = nearestRequestBelow(trackNum);

    // not found yet? reverse the direction
    if (nearestRequest == primaryQ->end())
    {
        direction = direction == UP ? DOWN : UP;
        return this->pollRequestUtil(trackNum);
    }
    Request *request = *nearestRequest;
    requestIterToRemove = nearestRequest;
    return request;
}

void Look::popRequest() {
    primaryQ->erase(requestIterToRemove);
}

deque<Request *>::iterator Look::nearestRequestAbove(int trackNum) {
    deque<Request *>::iterator nearestRequest = primaryQ->end();
    int shortestDist = INT_MAX;

    for (deque<Request*>::iterator iter = primaryQ->begin(); iter != primaryQ->end(); ++iter) {
        Request* request = *iter;
        if (request->trackNum - trackNum >= 0) {
            int len = request->trackNum - trackNum;
            if (len < shortestDist) {
                shortestDist = len;
                nearestRequest = iter;
            }
        }
    }
    return nearestRequest;
}

deque<Request *>::iterator Look::nearestRequestBelow(int trackNum) {
    deque<Request *>::iterator nearestRequest = primaryQ->end();
    int shortestDist = INT_MAX;

    for (deque<Request*>::iterator iter = primaryQ->begin(); iter != primaryQ->end(); ++iter) {
        Request* request = *iter;
        if (trackNum - request->trackNum >= 0) {
            int len = trackNum - request->trackNum;
            if (len < shortestDist) {
                shortestDist = len;
                nearestRequest = iter;
            }
        }
    }
    return nearestRequest;
}

FLook::FLook() {
    primaryQ = &queue1;
    secondaryQ = &queue2;
}

void Look::printQ(deque<Request *> * queue) {
    for (deque<Request*>::iterator iter = queue->begin(); iter != queue->end(); ++iter) {
        Request* request = *iter;
        cout << *request;
    }
}

void FLook::pushRequest(Request* request) {
    secondaryQ->push_back(request);
}

Request *FLook::getNextRequest(int trackNum) {

    trySwapQ();
    return pollRequestUtil(trackNum);
}

void FLook::trySwapQ() {
    if (primaryQ->empty()) {
        deque<Request *> *temp;
        temp = primaryQ;
        primaryQ = secondaryQ;
        secondaryQ = temp;
    }
}

void FLook::popRequest() {
    primaryQ->erase(requestIterToRemove);
}

bool FLook::empty() {
    if (primaryQ->empty() && secondaryQ->empty())
        return true;

    trySwapQ();
    return false;
}

Request *CLook::getNextRequest(int trackNum) {

    deque<Request *>::iterator nearestRequestAbove = requestQueue.end();
    deque<Request *>::iterator farthestRequestBelow = requestQueue.end();
    int shortestDist = INT_MAX;
    int farthestDist = INT_MIN;

    for (deque<Request*>::iterator iter = requestQueue.begin(); iter != requestQueue.end(); ++iter) {
        Request* request = *iter;
        if (request->trackNum - trackNum >= 0) {
            int len = request->trackNum - trackNum;
            if (len < shortestDist) {
                shortestDist = len;
                nearestRequestAbove = iter;
            }
        }
        else {
            int len = trackNum - request->trackNum;
            if (len > farthestDist) {
                farthestDist = len;
                farthestRequestBelow = iter;
            }
        }
    }
    if (nearestRequestAbove != requestQueue.end()) {
        requestIterToRemove = nearestRequestAbove;
        direction = UP;
    }
    else {
        requestIterToRemove = farthestRequestBelow;
        direction = DOWN;
    }
    return *requestIterToRemove;
}

void CLook::popRequest() {
    requestQueue.erase(requestIterToRemove);
}
