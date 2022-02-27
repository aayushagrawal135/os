#include "process.hpp"

Process::Process(int pid) {
    this->pid = pid;
    for (int i = 0; i < NUM_PAGES; ++i) {
        PTE entry(i);
        pageTable.push_back(entry);
    }
}

void Process::addVirtualMemArea(VMA virtualMemArea) {
    virtualMemAreaList.push_back(virtualMemArea);
}

PTE* Process::getPageAt(int pageNum) {
    return &pageTable[pageNum];
}

int Process::getPid() {
    return pid;
}

VMA* Process::isAccessible(int pageNum) {
    for(int i = 0; i < virtualMemAreaList.size(); ++i) {
        if (pageNum >= virtualMemAreaList[i].startVPage && pageNum <= virtualMemAreaList[i].endVPage)
            return &virtualMemAreaList[i];
    }
    return nullptr;
}

void Process::printPageTable() {
    printf("PT[%d]: ", pid);
    for (int i = 0; i < pageTable.size(); ++i) {
        if (pageTable[i].present == 1) {
            printf("%d:", i);
            printf(pageTable[i].referenced == 1 ? "R" : "-");
            printf(pageTable[i].modified == 1 ? "M" : "-");
            printf(pageTable[i].pagedout == 1 ? "S" : "-");
        }
        else
            printf(pageTable[i].pagedout == 1 ? "#" : "*");
        printf(i == pageTable.size()-1 ? "" : " ");
    }
    printf("\n");
}

void Process::printStats() {
    printf("PROC[%d]: U=%lu M=%lu I=%lu O=%lu FI=%lu FO=%lu Z=%lu SV=%lu SP=%lu\n",
    pid, info.unmaps, info.maps, info.ins, info.outs, info.fins, info.fouts, info.zeros, info.segv, info.segprot);
}

unsigned long long Process::getTotalCost() {
    unsigned long long sum = 0;
    sum += info.maps * cost.maps;
    sum += info.unmaps * cost.unmaps;
    sum += info.ins * cost.ins;
    sum += info.outs * cost.outs;
    sum += info.fins * cost.fins;
    sum += info.fouts * cost.fouts;
    sum += info.zeros * cost.zeros;
    sum += info.segv * cost.segv;
    sum += info.segprot * cost.segprot;
    return sum;
}