#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <iostream>
#include <vector>
using namespace std;

#define NUM_PAGES 64

struct PTE {
    unsigned int present : 1;
    unsigned int writeProtect : 1;
    unsigned int modified : 1;
    unsigned int referenced : 1;
    unsigned int pagedout : 1;
    unsigned int fileMapped : 1;
    unsigned int pageNumber : 6;
    unsigned int frameNumber : 7;
    unsigned int empty: 13;

    PTE() {
        init(0);
    }

    PTE(int pageNumber) {
        init(pageNumber);
    }

    void flush() {
        present = 0;
        modified = 0;
        frameNumber = 0;
        referenced = 0;
    }

    private:
        void init(int pageNumber) {
            present = 0;
            writeProtect = 0;
            modified = 0;
            referenced = 0;
            pagedout = 0;
            this->pageNumber = pageNumber;
        }
};

struct VMA {
    unsigned int startVPage : 7;
    unsigned int endVPage : 7;
    unsigned int writeProtected : 1;
    unsigned int fileMapped : 1;

    VMA(unsigned int startVPage, unsigned int endVPage, unsigned int writeProtected, unsigned int fileMapped) {
        this->startVPage = startVPage;
        this->endVPage = endVPage;
        this->writeProtected = writeProtected;
        this->fileMapped = fileMapped;
    }
};

class Process {

    private:

        int pid;
        vector<PTE> pageTable;
        vector<VMA> virtualMemAreaList;

    public:

        Process(int pid);
        void addVirtualMemArea(VMA virtualMemArea);
        PTE* getPageAt(int pageNum);
        int getPid();
        VMA* isAccessible(int pageNum);
        struct Info {
            unsigned long maps = 0;
            unsigned long unmaps = 0;
            unsigned long ins = 0;
            unsigned long outs = 0;
            unsigned long fins = 0;
            unsigned long fouts = 0;
            unsigned long zeros = 0;
            unsigned long segv = 0;
            unsigned long segprot = 0;
        } info;
        struct Cost {
            unsigned int maps = 300;
            unsigned int unmaps = 400;
            unsigned int ins = 3100;
            unsigned int outs = 2700;
            unsigned int fins = 2800;
            unsigned int fouts = 2400;
            unsigned int zeros = 140;
            unsigned int segv = 340;
            unsigned int segprot = 420;
        } cost;
        void printPageTable();
        void printStats();
        unsigned long long getTotalCost();
};


struct Instruction {
    char instrType;
    int value;

    Instruction(char instrType, int value) {
        this->instrType = instrType;
        this->value = value;
    }
};

struct Frame {
    
    public:

    Process *process = nullptr;
    PTE *vpage = nullptr;
    int frameNum;
    unsigned int age = 0;
    unsigned long time = 0;

    Frame(int frameNum) {
        this->frameNum = frameNum;
    }

    void reset() {
        process = nullptr;
        vpage = nullptr;
    }

    void reset(Process* process, PTE* vpage) {
        this->process = process;
        this->vpage = vpage;
        vpage->frameNumber = frameNum;
    }
};

#endif
