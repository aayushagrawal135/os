#ifndef OPERATION_HPP
#define OPERATION_HPP

#include <iostream>

using namespace std;

struct Request {

    int requestNum;
    int trackNum;
    int finishTime;
    int startTime;
    int arrivalTime;
    int waitTime;

    friend ostream &operator<<(ostream &out, const Request &request) {
        printf("# Request: %d, Arrival Time: %d, # Track %d\n",
        request.requestNum, request.arrivalTime, request.trackNum);
        return out;
    }

    Request(int arrivalTime, int trackNum) {
        this->arrivalTime = arrivalTime;
        this->trackNum = trackNum;
    }
};

#endif
