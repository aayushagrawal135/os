#include "core.hpp"

Core::Core(Pager* pager, Parser* parser) {
    this->pager = pager;
    this->parser = parser;
}

void Core::simulate() {

    initProcessList();
    pager->initFrameTable();

    Instruction* instruction;
    while ((instruction = parser->getParsedInstr()) != nullptr) {
        
        if (option)
            printf("%lu: ==> %c %d\n", instructionCount, instruction->instrType, instruction->value);
        ++instructionCount;

        switch (instruction->instrType) {
            // `value` corresponds to the process number to which we need to switch to
            case 'c':
                currentProcess = &processList[instruction->value];
                ++ctxSwitchCount;
                //continue;
                break;

            // `currentProcess` number and `value` are guaranteed to be same (as per question) - which is exited
            case 'e':
                printf("EXIT current process %d\n", currentProcess->getPid());
                for (int i = 0; i < NUM_PAGES; ++i) {
                    PTE* pageEntry = currentProcess->getPageAt(i);
                    if (pageEntry->present == 1) {
                        pageEntry->present = 0;
                        pageEntry->referenced = 0;
                        pager->resetFrame(pageEntry->frameNumber);
                        pager->addFreeFrame(pageEntry->frameNumber);
                        ++(currentProcess->info.unmaps);

                        if (option)
                            printf(" UNMAP %d:%d\n", currentProcess->getPid(), pageEntry->pageNumber);

                        if (pageEntry->modified == 1 && pageEntry->fileMapped == 1) {
                            ++(currentProcess->info.fouts);
                            printf(" FOUT\n");
                        }
                    }
                    pageEntry->pagedout = 0;
                }
                currentProcess = nullptr;
                ++exitCount;
                break;

            case 'r':
            case 'w':
                PTE* pageEntry = getPageBeforeRW(instruction, instructionCount);
                pageEntry->referenced = 1;
                if (instruction->instrType == 'r')
                    ++readCount;
                else {
                    if (pageEntry->writeProtect == 0) {
                        pageEntry->modified = 1;
                    }
                    else {
                        ++(currentProcess->info.segprot);
                        if (option)
                            printf(" SEGPROT\n");
                    }
                    ++writeCount;
                }
                break;
        } 
    }
}

PTE* Core::getPageBeforeRW(Instruction* instruction, unsigned long instructionCount) {

    // page has a frame mapping in the main memory 
    PTE* pageEntry = currentProcess->getPageAt(instruction->value);
    if (pageEntry->present == 1) {
        return pageEntry;
    }

    // page doesn't have a frame mapping in the main memory
    else {
        // so before picking a frame, check if the page is indeed accesible to the process
        VMA *vma = currentProcess->isAccessible(instruction->value);
        if (vma == nullptr) {
            ++(currentProcess->info.segv);
            if (option)
                printf(" SEGV\n");
            return pageEntry;
        }
        // now that page is accessbile, pick a frame
        else {
            Frame *frame = pager->getFrame(instructionCount);
            // the picked frame does not have a reverse mapped page, so there is nothing to unmap.
            // we can set the new mapping and return the page
            if (frame->vpage == nullptr) {
                includePageEntry(pageEntry, frame, vma);
                return pageEntry;
            }
            // the picked frame has a reversed mapped page, so first we need to remove that mapping
            // and swap that page out (if dirty/modified) back to disk (OUT) or to file (FOUT)
            else {
                detachProcessFromFrame(frame);
                includePageEntry(pageEntry, frame, vma);
                return pageEntry;
            }            
        }
    }
}

void Core::detachProcessFromFrame(Frame* frame) {
    if (option)
        printf(" UNMAP %d:%d\n", frame->process->getPid(), frame->vpage->pageNumber);
    ++(frame->process->info.unmaps);
    frame->vpage->frameNumber = 0;
    frame->vpage->present = 0;
    frame->vpage->referenced = 0;
    if (frame->vpage->modified == 1) {
        if (frame->vpage->fileMapped == 1) {
            ++(frame->process->info.fouts);
            if (option)
                printf(" FOUT\n");
        }
        else {
            ++(frame->process->info.outs);
            frame->vpage->pagedout = 1;
            if (option)
                printf(" OUT\n");
        }
    }
}

void Core::includePageEntry(PTE *pageEntry, Frame *frame, VMA *vma) {
    frame->reset(currentProcess, pageEntry);
    pageEntry->present = 1;
    pageEntry->frameNumber = frame->frameNum;
    pageEntry->referenced = 0;
    pageEntry->modified = 0;
    pageEntry->fileMapped = vma->fileMapped;
    pageEntry->writeProtect = vma->writeProtected;

    // if page present in the disk, we need to bring the page in, from there
    if (pageEntry->pagedout == 1) {
        ++(currentProcess->info.ins);
        if (option)
            printf(" IN\n");
    }
    // if page present in the cache, we need to bring the page in, from there
    else if (pageEntry->fileMapped == 1) {
        ++(currentProcess->info.fins);
        if (option)
            printf(" FIN\n");
    }
    else {
        ++(currentProcess->info.zeros);
        if (option)
            printf(" ZERO\n");
    }
    // cost of mapping the page with the frame
    ++(currentProcess->info.maps);
    if (option)
        printf(" MAP %d\n", frame->frameNum);
    
    //frame->setAge(0);
    frame->age = 0;
    frame->time = instructionCount;
    pageEntry->referenced = 1;
}

void Core::initProcessList() {
    this->processList = parser->parseProcess();
}

void Core::print() {
    if (pageTableOption) {
        for (int i = 0; i < processList.size(); ++i)
            processList[i].printPageTable();
    }
    if (frameTableOption)
        pager->printFrameTable();
    if (statAndSummary) {
        for (int i = 0; i < processList.size(); ++i)
            processList[i].printStats();
    }
    if (statAndSummary)
        printf("TOTALCOST %lu %lu %lu %llu 4\n", instructionCount, ctxSwitchCount, exitCount, getTotalCost());
}

unsigned long long Core::getTotalCost() {
    unsigned long long sum = 0;
    sum += ctxSwitchCount * cost.ctxSwitch;
    sum += exitCount * cost.exit;
    sum += readCount * cost.read;
    sum += writeCount * cost.write;
    for (int i = 0; i < processList.size(); ++i)
        sum +=  processList[i].getTotalCost();
    return sum;
}