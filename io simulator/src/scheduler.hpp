#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <iostream>
#include <queue>
#include <deque>
#include <climits>
#include "request.hpp"

using namespace std;

enum HandDirection {
    UP,
    DOWN
};

class Scheduler {
    
    protected:

        deque<Request *>::iterator requestIterToRemove;
        void resetDirection(int requestTrack, int currentTrack);

        Scheduler();

    public:
        HandDirection direction;
        deque<Request *> requestQueue;
        void printQ();
        virtual Request* getNextRequest(int trackNum) = 0;
        virtual void pushRequest(Request* request);
        virtual void popRequest();
        virtual bool empty();
};

class FIFO : public Scheduler {

    public:
        Request *getNextRequest(int trackNum);
        void popRequest();
};

class SSTF : public Scheduler {

    public:
        Request *getNextRequest(int trackNum);
        void popRequest();
};

class Look : public Scheduler {

    protected:
        deque<Request *> queue1;
        deque<Request *> *primaryQ;

        deque<Request *>::iterator nearestRequestAbove(int trackNum);
        deque<Request *>::iterator nearestRequestBelow(int trackNum);
        Request* pollRequestUtil(int trackNum);

    public:
        Look();
        Request *getNextRequest(int trackNum);
        void pushRequest(Request *request);
        void popRequest();
        bool empty();
        void printQ(deque<Request *> *queue);
};

class FLook : public Look {

    private:
        deque<Request *> queue2;
        deque<Request *> *secondaryQ;
        void trySwapQ();

    public:
        FLook();
        Request *getNextRequest(int trackNum);
        void pushRequest(Request* request);
        void popRequest();
        bool empty();
};

class CLook : public Scheduler {

    public:
        Request *getNextRequest(int trackNum);
        void popRequest();
};

#endif
