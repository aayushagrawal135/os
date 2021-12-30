#ifndef PAGER_HPP
#define PAGER_HPP

#include <vector>
#include <queue>
#include <iostream>
#include "process.hpp"
#include "parser.hpp"
using namespace std;

#define DEFAULT_FRAMES 4
#define RESET_PERIOD 50
#define INF 4

class Pager {
    
    Frame* allocateFrameFromFreeList();

    protected:
        int hand;
        queue<Frame *> freeFrames;
        int numFrames;
        unsigned long instructionCount;

        Pager();
        ~Pager();
        void setInstructionCount(int instructionCount);

    public:
        vector<Frame *> frameTable;
        virtual Frame *getVictimFrame() = 0;
        void resetFrame(int frameNum);
        void addFreeFrame(int frameNum);
        Frame* getFrame(unsigned long instructionCount);
        void setNumFrames(int numFrames);
        void initFrameTable();
        void printFrameTable();
};

class FIFO : public Pager {

    public:
        Frame* getVictimFrame();
};

class Random : public Pager {

    private:

        Parser* parser;

    public:
        Random(Parser* parser);
        Frame* getVictimFrame();
};

class Clock : public Pager {

    public:
        Frame* getVictimFrame();
};

class NRU : public Pager {

    unsigned long lastInstructionRest;

    public:
        NRU();
        Frame* getVictimFrame();
};

class Aging : public Pager {

    public:
        Frame* getVictimFrame();
        void reduceAge(int frameNum);
        void incAgeInExchangeOfRefBit(int frameNum);
};

class WorkingSet : public Pager {

    public:
        Frame *getVictimFrame();
        void refreshInExchangeOfRefBit(int frameNum);
};

#endif
