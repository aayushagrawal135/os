#ifndef CORE_HPP
#define CORE_HPP

#include <iostream>
#include <queue>
#include <deque>
#include "request.hpp"
#include "parser.hpp"
#include "scheduler.hpp"

using namespace std;

class Core {

    private:
        Parser* parser;
        Scheduler* scheduler;
        int currentTime;
        Request* fetchedRequest;
        Request* runningRequest;
        int currentTrack;
        int requestCount;
        queue<Request*> requestQueue;
        deque<Request*> finishedRequestQueue;
        int moveCount;

        void fetchRequest(bool &requestExhausted);
        void pushToFinishedRequestQueue(Request* request);
        void printSummary();

    public:
        Core(Parser* parser, Scheduler* scheduler);
        void simulate();
        void printReport();
        ~Core();
};

#endif
