#include "core.hpp"

Core::Core(Parser* parser, Scheduler* scheduler) {
    this->parser = parser;
    this->scheduler = scheduler;
    currentTime = 0;
    fetchedRequest = nullptr;
    runningRequest = nullptr;
    requestCount = 0;
    currentTrack = 0;
    moveCount = 0;
}

void Core::simulate() {

    bool requestExhausted = false;

    while (true) {
        
        // fetch request from parser if ready to fetch
        if (fetchedRequest == nullptr && !requestExhausted)
            fetchRequest(requestExhausted);

        // Push the fetched request to scheduler if it has "arrived"
        if (fetchedRequest != nullptr && fetchedRequest->arrivalTime == currentTime) {
            fetchedRequest->requestNum = requestCount;
            scheduler->pushRequest(fetchedRequest);
            fetchedRequest = nullptr;
            ++requestCount;
        }

        // Running request found the target track, hence completed
        if (runningRequest != nullptr && runningRequest->trackNum == currentTrack) {
            runningRequest->finishTime = currentTime;
            pushToFinishedRequestQueue(runningRequest);
            // scheduler->popRequest();
            runningRequest = nullptr;
        }

        // No running request, thus get a request from scheduler
        if (runningRequest == nullptr && !scheduler->empty()) {
            runningRequest = scheduler->getNextRequest(currentTrack);
            scheduler->popRequest();
            if (runningRequest != nullptr) {
                runningRequest->startTime = currentTime;
                runningRequest->waitTime = (currentTime - runningRequest->arrivalTime);
                if (runningRequest->trackNum == currentTrack)
                    --currentTime;
            }
        }

        // Request is still running, move "hand" in the "direction"
        if (runningRequest != nullptr && runningRequest->trackNum != currentTrack) {
            currentTrack += (scheduler->direction == UP ? 1 : -1);
            ++moveCount;
        }
        // Nothing to read from file, scheduler is empty and no running request
        if (runningRequest == nullptr && scheduler->empty() && requestExhausted) {
            break;
        }
        ++currentTime;
    }
}

void Core::pushToFinishedRequestQueue(Request* request) {
    for (deque<Request*>::iterator iter = finishedRequestQueue.begin(); iter != finishedRequestQueue.end(); ++iter) {
        Request* iterRequest = *iter;
        if (iterRequest->requestNum > request->requestNum) {
            finishedRequestQueue.insert(iter, request);
            return;
        }
    }
    finishedRequestQueue.push_back(request);
}

void Core::fetchRequest(bool& requestExhausted) {
    fetchedRequest = parser->getParsedRequest();
    if (fetchedRequest == nullptr)
        requestExhausted = true;
}

void Core::printReport() {
    for (deque<Request*>::iterator it = finishedRequestQueue.begin(); it != finishedRequestQueue.end(); ++it) {
        Request* request = *it;
        printf("%5d: %5d %5d %5d\n", request->requestNum, request->arrivalTime, request->startTime, request->finishTime);
    }
    printSummary();
}

void Core::printSummary() {
    long totalTurnAroundTime = 0;
    long totalWaitTime = 0;
    int maxWaitTime = 0;

    for (deque<Request *>::iterator it = finishedRequestQueue.begin(); it != finishedRequestQueue.end(); ++it) {
        Request *request = *it;
        totalTurnAroundTime += (request->finishTime - request->arrivalTime);
        totalWaitTime += request->waitTime;
        if (request->waitTime > maxWaitTime)
            maxWaitTime = request->waitTime;
    }
    printf("SUM: %d %d %.2lf %.2lf %d\n",
        currentTime,
        moveCount,
        double(totalTurnAroundTime) / requestCount,
        double(totalWaitTime) / requestCount,
        maxWaitTime);
}

Core::~Core() {
    for (deque<Request *>::iterator iter = finishedRequestQueue.begin(); iter != finishedRequestQueue.end(); ++iter) {
        Request *request = *iter;
        free(request);
    }
}
